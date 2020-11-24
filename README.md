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

    enable_uart=1

TODO: should be possible to set these up through mailbox commands instead of
  relying on config.txt.
