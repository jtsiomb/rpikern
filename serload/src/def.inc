	@ ---- Change IOBASE according to the device you wish to build for -----
	@.equ IOBASE, 0x20000000	@ RPI 1/zero
	.equ IOBASE, 0x3f000000		@ RPI 2/3
	@.equ IOBASE, 0xfe000000	@ RPI 4
	@ ----------------------------------------------------------------------

@ vi:set filetype=armasm:
