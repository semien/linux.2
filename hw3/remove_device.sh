#!/bin/bash
dev="keystats"
sudo rm /dev/$dev

make remove
make clean