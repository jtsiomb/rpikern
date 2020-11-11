Raspberry Pi2 notes
===================
The following is not all confirmed.

Memory map
----------
RAM is mapped to 0, 40000000 (cache-coherent) and c0000000h (uncached) in VC
space. In the ARM address space is mapped to 0(?).

I/O base is at 7e000000 in VC address space (rpi1?), and mapped to different
ranges for each model in ARM space.

Memory map summary:
 - load address: 8000h
 - I/O base:
   * rpi0/1: 20000000h
   * rpi2/3: 3f000000h
   * rpi4:   fe000000h
