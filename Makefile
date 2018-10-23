csrc = $(wildcard src/*.c) $(wildcard src/libc/*.c)
ssrc = $(wildcard src/*.s) $(wildcard src/libc/*.s)
Ssrc = $(wildcard src/*.S) $(wildcard src/libc/*.S)
obj = $(csrc:.c=.o) $(ssrc:.s=.o) $(Ssrc:.S=.o)
dep = $(csrc:.c=.d)
elf = rpikern.elf
bin = rpikern.bin

ifneq ($(shell uname -m | sed 's/^arm.*/arm/'), arm)
	toolprefix = arm-linux-gnueabihf-
	CPP = $(toolprefix)cpp
	CC = $(toolprefix)gcc
	AS = $(toolprefix)as
	LD = $(toolprefix)ld
	OBJCOPY = $(toolprefix)objcopy
endif

warn = -pedantic -Wall
dbg = -g
inc = -Isrc/libc
gccopt = -marm -fpic -ffreestanding -nostdinc
#arch = -mcpu=arm1176jzf-s
arch = -mcpu=cortex-a7

CFLAGS = $(arch) $(warn) $(opt) $(dbg) $(gccopt) $(inc) $(def)
ASFLAGS = $(arch) $(dbg) $(inc)
LDFLAGS = -nostdlib -T rpikern.ld -print-gc-sections

QEMU_FLAGS = -m 256 -M raspi2 -serial stdio


$(bin): $(elf)
	$(OBJCOPY) -O binary $< $@

$(elf): $(obj)
	$(LD) -o $@ $(obj) -Map link.map $(LDFLAGS)

-include $(dep)

%.d: %.c
	@echo 'gen dep $@ ...'
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj) $(bin) $(elf) link.map

.PHONY: cleandep
cleandep:
	rm -f $(dep)

.PHONY: run
run: $(elf)
	qemu-system-arm $(QEMU_FLAGS) -kernel $(elf)
