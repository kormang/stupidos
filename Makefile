
CFLAGS = -Wall -m32 -fno-pie -fno-stack-protector -ffreestanding -I./ -c

CSRCS=$(wildcard *.c)
CHEADERS=$(wildcard *.h)
COBJS=$(CSRCS:.c=.o)
ASMSRCS= gdt.asm isr.asm process.asm
ASMOBJS=$(ASMSRCS:.asm=asm.o)

all: boot.bin kernel

boot.bin: boot.asm
	nasm boot.asm -f bin -o boot.bin

kernel: kernelasm.o $(ASMOBJS) $(COBJS) usr/program
	ld -m elf_i386 -T link.ld -o kernel kernelasm.o $(ASMOBJS) $(COBJS) usr/program

%.o: %.c $(CHEADERS)
	gcc $(CFLAGS) $< -o $@

kernelasm.o: kernel.asm print.inc.asm
	nasm -f elf32 $< -o $@

israsm.o: isr.asm
	nasm -f elf32 $< -o $@

processasm.o: process.asm
	nasm -f elf32 $< -o $@

gdtasm.o: gdt.asm
	nasm -f elf32 $< -o $@

.PHONY: usr/program clean

usr/program:
	make -C usr/

clean:
	rm *.o
	rm kernel
	rm boot.bin
