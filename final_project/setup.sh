#!/bin/bash

cd final_project
make all>/dev/null
chmod u+x ./bench.o ./test.o
dmesg -C
insmod hack_syscall.ko
./test.o
./bench.o
rmmod hack_syscall.ko
dmesg
dmesg -C
cd ..
git clean -d -f>/dev/null