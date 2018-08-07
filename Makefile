
all: boot.bin kernel

boot.bin: boot.asm
	nasm boot.asm -f bin -o boot.bin

kernel: kernelc.o kernelasm.o
	ld -m elf_i386 -T link.ld -o kernel kernelasm.o kernelc.o

kernelc.o: kernel.c
	gcc -m32 -fno-pie -c kernel.c -o kernelc.o

kernelasm.o: kernel.asm print.inc.asm
	nasm -f elf32 kernel.asm -o kernelasm.o

.PHONY clean:
	rm *.o
	rm kernel
	rm boot.bin

