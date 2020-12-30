# serial device to be used by the boot rule, to transfer the kernel image
SERDEV ?= /dev/ttyS0

csrc = $(wildcard src/*.c) $(wildcard src/libc/*.c)
ssrc = $(wildcard src/*.s) $(wildcard src/libc/*.s)
Ssrc = $(wildcard src/*.S) $(wildcard src/libc/*.S)
obj = $(csrc:.c=.o) $(ssrc:.s=.o) $(Ssrc:.S=.o)
dep = $(csrc:.c=.d)
elf = rpikern.elf
bin = rpikern.bin
hex = rpikern.hex

ifneq ($(shell uname -m | sed 's/^arm.*/arm/'), arm)
	toolprefix = arm-linux-gnueabihf-
	CPP = $(toolprefix)cpp
	CC = $(toolprefix)gcc
	AS = $(toolprefix)as
	LD = $(toolprefix)ld
	OBJCOPY = $(toolprefix)objcopy
else
	OBJCOPY = objcopy
endif

warn = -pedantic -Wall
dbg = -g
inc = -Isrc -Isrc/libc
gccopt = -marm -fno-pic -ffreestanding -nostdinc -ffast-math -fno-math-errno -MMD
#arch = -mcpu=arm1176jzf-s
arch = -mcpu=cortex-a7

CFLAGS = $(arch) $(warn) $(opt) $(dbg) $(gccopt) $(inc) $(def)
ASFLAGS = $(arch) $(dbg) $(inc)
LDFLAGS = -nostdlib -T rpikern.ld -print-gc-sections

QEMU_FLAGS = -vnc :0 -m 1024 -M raspi2 -serial stdio -d guest_errors

$(bin): $(elf)
	$(OBJCOPY) -O binary $< $@

$(hex): $(elf)
	$(OBJCOPY) -O ihex $< $@

$(elf): $(obj) rpikern.ld
	$(LD) -o $@ $(obj) -Map link.map $(LDFLAGS)

-include $(dep)

.PHONY: clean
clean:
	rm -f $(obj) $(bin) $(elf) link.map

.PHONY: cleandep
cleandep:
	rm -f $(dep)

.PHONY: run
run: $(elf)
	qemu-system-arm $(QEMU_FLAGS) -kernel $(elf)

.PHONY: disasm
disasm: $(elf)
	$(toolprefix)objdump -D $<

.PHONY: install
install: $(bin)
	cp $(bin) /srv/tftp/$(bin)

.PHONY: boot
boot: $(hex)
	stty -F $(SERDEV) 115200 ixon
	ascii-xfr -dsv $< >$(SERDEV)
	#cat $< >$(SERDEV)

.PHONY: hex
hex: $(hex)
