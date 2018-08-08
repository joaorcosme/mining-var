#!/bin/bash
set -x

# Force driver canpro_usb to work...

if [[ ! `lsmod` =~ "canpro_usb" ]]; then
    sudo modprobe -v canpro_usb
fi

sleep 1
cd /etc/init.d/
sudo insserv softing_canprousb

sleep 1
sudo ./softing_canprousb restart

sleep 1
cd /etc/softing
sudo ./mk_devices 15
