To build a simple object file that fits on one screen
(-c -- compile but don't link): 

  gcc -arch x86_64 -O -g -fno-pic -fno-omit-frame-pointer -c main.c
  gcc -arch x86_64 -O -fno-omit-frame-pointer -c foo.c

To look at disassembly -d disas (Intel syntax), -r (show relocations):

  objdump -M intel -d -r main.o

