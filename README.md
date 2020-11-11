RPi Configuration
-----------------
In `config.txt`:
    init_uart_clock=3000000
    enable_uart=1
    core_freq=250

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
