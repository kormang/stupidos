# Basic OS

## Get started

The OS is developed under Ubuntu 18.04, but other Linux operating systems should work too.
If you are not using Debian based system then you should adopt commands for installing tools and software (if you are not using apt but rpm for example).

### Installing tools

* # `apt-get install build-essential`
* # `apt-get install nasm`

### Building bootloader and installing

To compile bootloader:
`make boot.bin`

To install bootloader to some device (for example USB):
`./write-mbr.sh /dev/sdb`

Here /dev/sdb is optional, and default value is /dev/sdb.
Old content of MBR are saved to ./mbr-backup.bin.

## Credits

[OSDev wiki](https://wiki.osdev.org/)
[JamesM' osdev tutorials](http://www.jamesmolloy.co.uk/tutorial_html/9.-Multitasking.html)
[Intel software developer manual](https://software.intel.com/en-us/articles/intel-sdm)
