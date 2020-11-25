SECTIONS {
	. = 0x00008000;
	_stacktop = .;
	_kern_start = .;

	.text : {
		KEEP(* (.startup*));
		* (.text*);
	}

	.rodata ALIGN(4): { * (.rodata*); }
	.data ALIGN(4): { * (.data*); }

	.bss ALIGN(4): {
		_bss_start = .;
		* (.bss*);
		. = ALIGN(4);
		_bss_end = .;
	}
	_bss_size = SIZEOF(.bss);

	. = ALIGN(4);
	_kern_end = .;
	_kern_size = _kern_end - _kern_start;
}
