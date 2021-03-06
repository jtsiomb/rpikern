	.include "def.inc"

	.equ SCTLR_DCACHE, 0x0004
	.equ SCTLR_ICACHE, 0x1000

	.section .startup
	.code 32

	.global startup
startup:
	@ park all but one of the cores (TODO unpark later)
	mrc p15, 0, r0, c0, c0, 5
	ands r0, r0, #0xff
	bne park

	@ disable caches
	mrc p15, 0, r0, c1, c0, 0
	bic r0, #SCTLR_DCACHE
	bic r0, #SCTLR_ICACHE
	mcr p15, 0, r0, c1, c0, 0

	@ put the stacktop at 4000h
	mov sp, #0x4000

	@ move the boot loader out of the way to load the kernel into 8000h later
	mov r8, sp
	ldr r9, =_kern_start
	ldr r10, =_kern_end	@ r10 will act as sentinel
0:	ldmia r9!, {r0-r7}
	stmia r8!, {r0-r7}
	cmp r9, r10
	blo 0b

	@ continue executing in the copy 4000h lower
	sub pc, #0x4000

	bl init_serial

	ldr r0, =hello
	bl ser_printstr

	@ macro to send a XON and update r11 to reflect the flow control state
	.macro flow_xon
	mov r11, #1
	mov r0, #17
	bl ser_putchar
	.endm
	@ macro to send a XOFF and update r11 to reflect the flow control state
	.macro flow_xoff
	mov r11, #0
	mov r0, #19
	bl ser_putchar
	.endm

	flow_xon
	ldr r8, =buf		@ initialize the input pointer
mainloop:
	bl ser_getchar

	@ if there are more characters pending, send a XOFF and read them all
	mov r4, r0		@ save the char we just read
.Lcheck_pending:
	bl ser_pending
	cmp r0, #0
	beq .Lnot_pending
	flow_xoff
	mov r0, r4
	bl proc_char
	bl ser_getchar		@ get the first pending char
	mov r4, r0
	b .Lcheck_pending
.Lnot_pending:
	@ re-enable flow control if we disabled it previously
	cmp r11, #0
	bne 0f
	flow_xon
0:	mov r0, r4		@ process the last char
	bl proc_char
	b mainloop

proc_char:
	stmfd sp!, {lr}

	@bl ser_putchar

	cmp r0, #':'		@ ignore colons
	beq .Lproc_char_end

	cmp r0, #13		@ check for end of line
	cmpne r0, #10
	strneb r0, [r8], #1	@ not EOL ? store it in the buffer
	bne .Lproc_char_end	@ and return

	@ if it was EOL, send XOFF, process the line and jump back
	mov r0, #19
	bl ser_putchar

	bl procline		@ procline also restores r8 to the start of buf

	@ if previously input was enabled (XON), restore it
	cmp r11, #0
	beq .Lproc_char_end
	flow_xon
.Lproc_char_end:
	ldmfd sp!, {pc}

	@ macro for converting a single hex digit to binary in the same register
	.macro hex2bin_digit reg
	cmp \reg, #'F'		@ r > 'F' ? mark it zero dude
	bhi 0f
	cmp \reg, #'A'		@ r >= 'A' ? r -= 'A' - 10
	subhs \reg, #'A' - 10
	bhs 1f
	cmp \reg, #'9'		@ r > '9' ? mark it zero dude
	bhi 0f
	cmp \reg, #'0'		@ r >= '0' ? r -= '0'
	subhs \reg, #'0'
	bhs 1f
0:	mov \reg, #0
1:
	.endm

procline:
	stmfd sp!, {r4, lr}
	@ convert hex->bin in place and calculate the CRC at the same time
	ldr r9, =buf
	mov r10, r9
	mov r7, #0
.Lprocline_loop:
	ldrh r0, [r9], #2
	mov r1, r0, LSR #8	@ second char in r1
	and r0, #0xff		@ first char in r0
	hex2bin_digit r0
	hex2bin_digit r1
	orr r0, r1, r0, LSL #4
	add r7, r0		@ add to the running sum
	strb r0, [r10], #1
	cmp r9, r8		@ see if we reached the end
	blo .Lprocline_loop

	ands r7, #0xff
	ldrne r0, =str_crcfail
	beq 0f
	bl ser_printstr
	b .Lprocline_end	@ CRC failed
0:
	@ move the sentinel to the CRC field to ignore it from now on
	sub r8, #1

	@ check the command type
	ldr r9, =buf
	ldrb r0, [r9, #3]
	cmp r0, #5		@ valid: 0-5
	ldrls pc, [pc, r0, LSL #2]
	b .Lprocline_err
	.long .Lprocline_data - 0x4000
	.long .Lprocline_eof - 0x4000
	.long .Lprocline_end - 0x4000
	.long .Lprocline_end - 0x4000
	.long .Lprocline_base_lin - 0x4000
	.long .Lprocline_start_lin - 0x4000

.Lprocline_data:
	@ copy data to specified address
	ldrb r0, [r9, #1]
	ldrb r1, [r9, #2]
	orr r0, r1, r0, LSL #8
	ldr r10, base_addr
	add r10, r0		@ dest addr in r10
	add r9, #4		@ move src ptr to start of data
0:	ldmia r9!, {r0-r3}	@ copy 16 bytes at a time
	stmia r10!, {r0-r3}
	cmp r9, r8		@ did we reach the sentinel?
	blo 0b
	b .Lprocline_end

.Lprocline_eof:
	@ EOF reached, jump to the entry point (default: 0x8000)
	bl ser_flow_start	@ send XON before branching to the kernel
	ldr r0, start_addr
	bx r0

.Lprocline_base_lin:
	@ linear base address high halfword in big endian at buf + 4
	ldrh r0, [r9, #4]
	rev r0, r0
	str r0, base_addr
	b .Lprocline_end

.Lprocline_start_lin:
	@ linear start address in big endian at buf + 4
	ldr r0, [r9, #4]
	rev r0, r0
	str r0, start_addr

.Lprocline_end:
	ldr r8, =buf
	ldmfd sp!, {r4, lr}
	bx lr

.Lprocline_err:
	ldr r0, =str_inval
	bl ser_printstr
	b .Lprocline_end

park:	wfe
	b park

hello:	.ascii "Simple raspberry pi serial port boot loader\n"
	.asciz "by John Tsiombikas <nuclear@member.fsf.org>\n"

str_inval: .asciz "Invalid command\n"
str_crcfail: .asciz "CRC failed, ignoring line\n"

	.align 2
echo:	.long 0
base_addr: .long 0
start_addr: .long 0x8000
buf:

@ vi:set filetype=armasm:
