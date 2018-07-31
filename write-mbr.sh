#! /bin/sh

# Usage example: ./write-mbr.sh /dev/sdb

MBR_DEVICE=/dev/sdb

if [ $# -gt 0 ]
then
	MBR_DEVICE=$1
fi

sudo dd bs=512 count=1 if=$MBR_DEVICE of=./mbr-backup.bin
sudo dd bs=512 count=1 if=./boot.bin of=$MBR_DEVICE

