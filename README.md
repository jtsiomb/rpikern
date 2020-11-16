Raspberry Pi kernel hacking experiments
=======================================
Just having some fun and rage with the raspberry pi. The rest of this readme
file is basically a bunch of notes.

Copyright (C) 2018-2020 John Tsiombikas <nuclear@member.fsf.org>  
Free software, use/modify/redistribute under the terms of the GNU General Public
License v3, or later. See COPYING for details.

RPi Configuration
-----------------
In `config.txt`:

    init_uart_clock=3000000
    enable_uart=1
    core_freq=250

TODO: should be possible to set these up through mailbox commands instead of
  relying on config.txt.

netboot with uboot
------------------
Put `rpikern.bin` in `/srv/tftp/`  (`make install`)

Once:

    setenv serverip 192.168.0.4
    setenv ipaddr 192.168.0.25
    saveenv

Then:

    tftpboot 0x8000 rpikern.bin
    go 0x8000

TODO: make it automatic with a boot script
