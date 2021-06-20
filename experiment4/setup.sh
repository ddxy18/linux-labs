#!/bin/bash

set -e

root_path=/tmp/linux_lab_project4

apt-get install genromfs>/dev/null
make all
mkdir $root_path
touch $root_path/aa $root_path/bb $root_path/ft
echo abc>$root_path/bb
genromfs –V "vromfs" -d $root_path -f test.img
echo ------------------------------------------------------------------------------
insmod hack_romfs.ko hided_file_name=aa encrypted_file_name=bb exec_file_name=ft
mount -o loop test.img $root_path
ls -l $root_path
cat $root_path/bb
umount $root_path
rmmod hack_romfs.ko
rm -rf $root_path