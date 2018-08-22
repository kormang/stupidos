CFLAGS = -Wall -m32 -fno-pie -fno-stack-protector -ffreestanding -I./ -c

CSRCS=$(wildcard *.c)
CHEADERS=$(wildcard *.h)
COBJS=$(CSRCS:.c=.o)
ASMSRCS=
ASMOBJS=$(ASMSRCS:.asm=asm.o)

all: program

program: meelibcasm.o $(ASMOBJS) $(COBJS) template.S
	ld -m elf_i386 -T link.ld -o program_linked meelibcasm.o $(ASMOBJS) $(COBJS)
	gcc template.S $(CFLAGS) -c -o program

meelibcasm.o: meelibc.asm
	nasm meelibc.asm -f elf32 -o meelibcasm.o

%.o: %.c $(CHEADERS)
	gcc $(CFLAGS) $< -o $@

.PHONY clean:
	rm *.o
	rm program_linked
	rm program

