#! /bin/sh

# Usage example: ./write-kernel.sh /dev/sdb

MBR_DEVICE=/dev/sdb

if [ $# -gt 0 ]
then
	MBR_DEVICE=$1
fi

sudo dd bs=512 count=15 seek=1 if=$MBR_DEVICE of=./kernel-sectors-backup.bin
sudo dd bs=512 count=15 seek=1 if=./kernel of=$MBR_DEVICE

