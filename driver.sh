#! /bin/bash

rm *~ 
make -C /usr/src/linux-headers-3.8.0-29-generic M=~/kernel_modules_hacking/
sudo rm *.o *.order *.sym* data.txt *.mod.c .*.cmd

sudo dmesg --clear

sudo insmod ./$1.ko

sleep 1

sudo rmmod $1

dmesg > data.txt

subl data.txt

