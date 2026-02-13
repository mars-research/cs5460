# Use gcc as a compiler
CC=gcc
# CFLAGS will be the options we'll pass to the compiler
CFLAGS=-Wall -g -fno-pic -no-pie

all: main elf elf1 elf_explain elf_extra_credit elf_extra_credit2.o  elf_extra_credit2lib.o

main: main.c
	$(CC) $(CFLAGS) main.c -o main

elf: elf.c
	$(CC) -ffreestanding -mcmodel=large -nostdlib -Wl,-pie -Wl,-e,add -o $@ $<

elf1: elf1.c
	$(CC) -ffreestanding -mcmodel=large -nostdlib -Wl,-pie -Wl,-e,linear_transform -o $@ $<

elf_explain: elf_explain.c
	$(CC) -ffreestanding -mcmodel=large -nostdlib -Wl,-pie -Wl,-e,quadruple -o $@ $<

elf_extra_credit: elf_extra_credit.c
	$(CC) -Wl,-pie -Wl,--no-relax -nostdlib -fpic -e linear_transform -o $@ $<


elf_extra_credit2.o: elf_extra_credit2.c
		$(CC) -mcmodel=large -c -o $@ $<

elf_extra_credit2lib.o: elf_extra_credit2lib.c
		$(CC) -mcmodel=large -c -o $@ $<

clean:
	rm -f *.o main elf elf1 elf_explain elf_extra_credit elf_extra_credit2.o elf_extra_credit2lib.o

