
CFLAGS = -Wall -m32 -fno-pie -I./ -c

CSRCS=$(wildcard *.c)
CHEADERS=$(wildcard *.h)
COBJS=$(CSRCS:.c=.o)
ASMOBJS=$(wildcard *.asm)


all: boot.bin kernel

boot.bin: boot.asm
	nasm boot.asm -f bin -o boot.bin

kernel: kernelasm.o israsm.o $(COBJS)
	ld -m elf_i386 -T link.ld -o kernel kernelasm.o israsm.o $(COBJS)

%.o: %.c $(CHEADERS)
	gcc $(CFLAGS) $< -o $@

kernelasm.o: kernel.asm print.inc.asm
	nasm -f elf32 kernel.asm -o kernelasm.o

israsm.o: isr.asm
	nasm -f elf32 isr.asm -o israsm.o

.PHONY clean:
	rm *.o
	rm kernel
	rm boot.bin

