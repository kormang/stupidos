
all: boot.bin kernel

boot.bin: boot.asm
	nasm boot.asm -f bin -o boot.bin

kernel: kernelc.o kernelasm.o
	cp kernelasm.o kernel

kernelc.o: kernel.c
	gcc -m32 -c kernel.c -o kernelc.o

kernelasm.o: kernel.asm print.inc.asm
	nasm -f bin kernel.asm -o kernelasm.o

.PHONY clean:
	rm *.o
	rm kernel
	rm boot.bin
