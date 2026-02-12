# Use gcc as a compiler
CC=gcc
# CFLAGS will be the options we'll pass to the compiler
CFLAGS=-Wall -g -fno-pic -no-pie

all: main elf elf1 elf_explain elf_extra_credit

main: main.c
	$(CC) $(CFLAGS) main.c -o main

elf: elf.c
	$(CC) -nostdlib -fno-asynchronous-unwind-tables -fno-plt -fno-pic -mcmodel=large -static-pie -Wl,-N -e add -o $@ $<

elf1: elf1.c
	$(CC) -nostdlib -fno-asynchronous-unwind-tables -fno-plt -fno-pic -mcmodel=large -static-pie -Wl,-N -e linear_transform -o $@ $<

elf_explain: elf_explain.c
	$(CC) -nostdlib -fno-asynchronous-unwind-tables -fno-plt -fno-pic -mcmodel=large -static-pie -Wl,-N -e quadruple -o $@ $<

elf_extra_credit: elf_extra_credit.c
	$(CC) -Wl,--no-relax -nostdlib -fpic -e linear_transform -o $@ $<
	
clean:
	rm -f *.o main elf elf1 elf_explain elf_extra_credit
