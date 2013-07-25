#! /bin/bash

rm *~ 
make -C /usr/src/linux-headers-3.2.0-36-generic/ M=/home/ritesh/Programs/Linux/Modules

sudo rm *.o *.order *.sym* data.txt .* #*.mod.c

sudo dmesg --clear

sudo insmod ./$1.ko

sleep 1

sudo rmmod $1

dmesg > data.txt

sublime data.txt

