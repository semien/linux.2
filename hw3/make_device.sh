#!/bin/bash
make
make instil

dev="keystats"
major="$(grep "$dev" /proc/devices | cut -d ' ' -f 1)"
sudo mknod "/dev/$dev" c "$major" 0

sudo chmod +x /dev/$dev