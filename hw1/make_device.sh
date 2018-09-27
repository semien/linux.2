#!/bin/bash
make
make instil

dev="phone_book"
major="$(grep "$dev" /proc/devices | cut -d ' ' -f 1)"
sudo mknod "/dev/$dev" c "$major" 0

sudo chmod +x /dev/$dev