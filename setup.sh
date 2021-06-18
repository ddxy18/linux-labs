#!/bin/bash

make all>/dev/null
chmod u+x ./bench.o ./test.o
dmesg -C
insmod hack_syscall.ko
./test.o
./bench.o
rmmod hack_syscall.ko
dmesg
dmesg -C
git clean -d -f>/dev/null