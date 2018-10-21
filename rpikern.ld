SECTIONS {
	. = 0x00008000;
	_kern_start = .;

	.startup : { KEEP(* (.startup)) }
	.text : { * (.text); }
	.rotdata : { * (.rodata); }
	.data : { * (.data); }

	.bss ALIGN(4): {
		_bss_start = .;
		* (.bss);
		. = ALIGN(4);
		_bss_end = .;
	}
	_bss_size = SIZEOF(.bss);

	_kern_size = . - _kern_start;
	_mem_start = .;
}
