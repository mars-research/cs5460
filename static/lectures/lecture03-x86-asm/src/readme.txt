To build a simple object file that fits on one screen
(-c -- compile but don't link): 

  gcc -m32 -O -c main.c

To look at disassembly -d disas (GCC syntax), use -r to show relocations:
  
  objdump -d -r main.o

Same as above but Intel syntax:

  objdump -M intel -d -r main.o

Build with debug info
  gcc -m32 -fno-pic -O -g -c main.c

Interleave with source
  objdump -M intel -S -d main.o
