src = $(wildcard src/*.s)
obj = $(src:.s=.o)
name = serload

ifneq ($(shell uname -m | sed 's/^arm.*/arm/'), arm)
	toolprefix = arm-linux-gnueabihf-
	AS = $(toolprefix)as
	LD = $(toolprefix)ld
	OBJCOPY = $(toolprefix)objcopy
endif

ASFLAGS = -mcpu=cortex-a7
LDFLAGS = -nostdlib -T ../rpikern.ld -print-gc-sections

$(name).bin: $(name).elf
	$(OBJCOPY) -O binary $< $@

$(name).elf: $(obj)
	$(LD) -o $@ $^ -Map link.map $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) $(obj) $(name).elf $(name).bin

.PHONY: run
run: $(name).elf
	qemu-system-arm -vnc :0 -m 1024 -M raspi2 -serial stdio -d guest_errors -kernel $<

.PHONY: disasm
disasm: $(name).elf
	$(toolprefix)objdump -D $<
