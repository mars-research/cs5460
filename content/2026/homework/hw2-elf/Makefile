# Use gcc as a compiler
CC=gcc
# CFLAGS will be the options we'll pass to the compiler
CFLAGS=-Wall -g -fno-pic -no-pie

all: main elf elf1 elf_explain

main: main.c
	$(CC) $(CFLAGS) main.c -o main

elf: elf.c
	$(CC) -nostdlib -fno-asynchronous-unwind-tables -fno-plt -fno-pic -mcmodel=large -static-pie -Wl,-N -e add -o $@ $<

elf1: elf1.c
	$(CC) -nostdlib -fno-asynchronous-unwind-tables -fno-plt -fno-pic -mcmodel=large -static-pie -Wl,-N -e linear_transform -o $@ $<

elf_explain: elf_explain.c
	$(CC) -nostdlib -fno-asynchronous-unwind-tables -fno-plt -fno-pic -mcmodel=large -static-pie -Wl,-N -e quadruple -o $@ $<

clean:
	rm -f *.o main elf elf1 elf_explain