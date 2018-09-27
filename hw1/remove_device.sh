#!/bin/bash
dev="phone_book"
sudo rm /dev/$dev

make remove
make clean