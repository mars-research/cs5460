 CS5460/6450 Operating Systems   

![](./images/spacer.gif)

[Home](../../index.html) ![](./images/spacer.gif)


Homework 2: ELF files
=====================

This assignment will make you more familiar with organisation of ELF files. Technically, you can do this assignment on any operating system that supports the Unix API (Linux CADE machines, your laptop that runs Linux or Linux VM, and even MacOS, etc.). **You don't need to set up xv6 for this assignment** Submission through Gradescope.

**YOU CANNOT PUBLICLY RELEASE SOLUTIONS TO THIS HOMEWORK**. It's ok to show your work to your future employer as a private Git repo, however any public release is prohibited.


Part 1: Take a look at ELF files
--------------------------------

At a high level this homework first shows you how to implement a simple ELF loader [main.c](./main.c) file) (we use it to load a simple ELF object file compiled from [elf.c](./elf.c)), and then asks you to relocate it to run at the address at which you load it. However, before starting on this lets make ourselves familiar with ELF files.

We provide a simple [Makefile](Makefile) that compiles [elf.c](./elf.c) and [main.c](./main.c) as ELF executables. Look over the makefile and then compile both files by running:

``` {style="position: relative;"}
make
```

Lets take a look at the ELF files we compiled. We will use the readelf tool

``` {style="position: relative;"}
$ readelf -a elf_explain
```
ELF is the file format used for object files (`.o`'s), binaries, shared libraries and core dumps in Linux.

It's actually pretty simple and well thought-out.

ELF has the same layout for all architectures, however endianness and word size can differ; relocation types, symbol types and the like may have platform-specific values, and of course the contained code is arch specific.

The ELF files are used by two tools: 1) linker and 2) loader. A linker combines multiple ELF files into an executable or a library and a loader loads the executable ELF file in the memory of the process. On real operating systems loading may require relocation (e.g., if the file is dynamically linked it has to be linked again with all the shared libraries it depends on). 

Linker and loader need two different views of the ELF file, i.e., they access it differently---the linker needs to know where the DATA, TEXT, BSS, and other sections are to merge them with sections from other libraries. If relocation is required the linker needs to know where the symbol tables and relocation information is.

The loader, however, does not need any of these details. It simply needs to know which parts of the ELF file are code (executable), which are data and read-only data, and where to put the BSS in the memory of a process.

Hence the ELF file provides two separate views on the data inside the ELF file: 1) a more detailed view for the linker, and 2) a bit more high-level view for the loader. To provide these views each ELF file contains two arrays: Section Header Table (for the linker), and Program Header Table (for the loader). Both tables are simply arrays of entries that contain information about each part of the ELF file (e.g., where the sections for the linker and section for the loader are inside the ELF file).

Here is a simple figure of a typical ELF file that starts with the ELF header. The header contains pointers to the locations of Section Header Table and Program Header Table within the ELF file. Then each tables have entries that point to the starting locations of individual sections and segments.

![](./hw2-elf.png)

Lets take a look at both arrays.

Linking view: Section Header Table (SHT)
----------------------------------------

The Section Header Table is an array in which every entry contains a pointer to one of the sections of the ELF file. **It's a bit annoying but the parts of the ELF file used by the linker are called "sections", and the parts used by the loader are called "segments"** (my guess is that different CPU segments were configured in the past for each part of the program loaded in memory, hence the name "segments", for example, an executable CPU segment was created for the executable parts of the ELF file (i.e., one segment that contained all executable sections like .text, .init, etc.).

Also don't get confused: sections and segments overlap. I.e., typically multiple sections (.text, .init) are all contained in one executable segment. Confusing, huh? It will become clear soon.

Lets take a look at what inside the ELF file. Run this command

``` {style="position: relative;"}

readelf -a elf_explain

```

If you scroll down to the **Section headers** you will see all "sections" of the ELF file that the linker can use:

```
Section Headers:
  [Nr] Name              Type             Address           Offset Size              EntSize          Flags  Link  Info  Align
  [ 0]                   NULL             0000000000000000  00000000 0000000000000000  0000000000000000           0     0     0
  [ 1] .note.gnu.pr[...] NOTE             0000000000400190  00000190 0000000000000020  0000000000000000   A       0     0     8
  [ 2] .note.gnu.bu[...] NOTE             00000000004001b0  000001b0 0000000000000024  0000000000000000   A       0     0     4
  [ 3] .gnu.hash         GNU_HASH         00000000004001d8  000001d8 000000000000001c  0000000000000000   A       4     0     8
  [ 4] .dynsym           DYNSYM           00000000004001f8  000001f8 0000000000000018  0000000000000018   A       5     1     8
  [ 5] .dynstr           STRTAB           0000000000400210  00000210 0000000000000001  0000000000000000   A       0     0     1
  [ 6] .rela.text        RELA             0000000000400218  00000218 0000000000000060  0000000000000018  AI       4     7     8
  [ 7] .text             PROGBITS         0000000000400278  00000278 0000000000000055  0000000000000000 WAX       0     0     1
  [ 8] .eh_frame         PROGBITS         00000000004002d0  000002d0 0000000000000000  0000000000000000   A       0     0     8
  [ 9] .dynamic          DYNAMIC          00000000004002d0  000002d0 0000000000000110  0000000000000010  WA       5     0     8
  [10] .data             PROGBITS         00000000004003e0  000003e0 0000000000000008  0000000000000000  WA       0     0     4
  [11] .bss              NOBITS           00000000004003e8  000003e8 0000000000000008  0000000000000000  WA       0     0     4
  [12] .comment          PROGBITS         0000000000000000  000003e8 000000000000002b  0000000000000001  MS       0     0     1
  [13] .symtab           SYMTAB           0000000000000000  00000418 0000000000000120  0000000000000018          14     4     8
  [14] .strtab           STRTAB           0000000000000000  00000538 0000000000000042  0000000000000000           0     0     1
  [15] .shstrtab         STRTAB           0000000000000000  0000057a 000000000000008d  0000000000000000           0     0     1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  D (mbind), l (large), p (processor specific)
```

The elf_explain.c is a simple program, but it still has .text, `.data`, `.bss`, and `.rel.text` (relocation) sections and a bunch of sections that contain symbol information and a ton of other stuff.

Since `elf_explain.c` has two initialized and one uninitialized global variables the ELF has data section of 8 bytes and the BSS section of 4 bytes (each variable is a 4 byte integer).

The program is linked elf.c to run at address 0x400000, which is the default base address for 64-bit ELF executables on UNIX-like systems.

The symbol table contains the following symbols

``` {style="position: relative;"}
Symbol table '.symtab' contains 12 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS elf_explain.c
     2: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS 
     3: 00000000004002d0     0 OBJECT  LOCAL  DEFAULT    9 _DYNAMIC
     4: 00000000004003e0     4 OBJECT  GLOBAL DEFAULT   10 b
     5: 0000000000400278    60 FUNC    GLOBAL DEFAULT    7 quadruple
     6: 00000000004003e4     4 OBJECT  GLOBAL DEFAULT   10 c
     7: 00000000004003e8     0 NOTYPE  GLOBAL DEFAULT   11 __bss_start
     8: 00000000004003e8     4 OBJECT  GLOBAL DEFAULT   11 d
     9: 00000000004002b4    25 FUNC    GLOBAL DEFAULT    7 magic
    10: 00000000004003e8     0 NOTYPE  GLOBAL DEFAULT   10 _edata
    11: 00000000004003f0     0 NOTYPE  GLOBAL DEFAULT   11 _end
```

I.e., it contains names of the two functions quadruple and magic, three global variables b, c, and d. Other symbols like \_\_bss\_start, \_edata, and \_end are added by the linker to mark the start and end of the BSS, TEXT, and DATA sections.

Execution view: Program Header Table (PHT)
------------------------------------------

The Program Header Table contains information for the kernel on how to start the program. The `LOAD` directives determinate what parts of the ELF file get mapped into program memory.

In our elf example the program header defines four segments, but only two of them should be loaded by the operating system in memory to run.

``` {style="position: relative;"}

Program Headers:
  Type           Offset             VirtAddr           PhysAddr           FileSiz            MemSiz              Flags  Align
  LOAD           0x0000000000000190 0x0000000000400190 0x0000000000400190 0x0000000000000258 0x0000000000000260  RWE    0x8
  DYNAMIC        0x00000000000002d0 0x00000000004002d0 0x00000000004002d0 0x0000000000000110 0x0000000000000110  RW     0x8
  NOTE           0x0000000000000190 0x0000000000400190 0x0000000000400190 0x0000000000000020 0x0000000000000020  R      0x8
  NOTE           0x00000000000001b0 0x00000000004001b0 0x00000000004001b0 0x0000000000000024 0x0000000000000024  R      0x4
  GNU_PROPERTY   0x0000000000000190 0x0000000000400190 0x0000000000400190 0x0000000000000020 0x0000000000000020  R      0x8
  GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000 0x0000000000000000 0x0000000000000000  RW     0x10
```


The only loadable section is linked to be loaded at address 0x400190. We can also look at the mappings of sections to segments (we will discuss what sections are below):

``` {style="position: relative;"}
 Section to Segment mapping:
  Segment Sections...
   00     .note.gnu.property .note.gnu.build-id .gnu.hash .dynsym .dynstr .rela.text .text .eh_frame .dynamic .data .bss 
   01     .dynamic 
   02     .note.gnu.property 
   03     .note.gnu.build-id 
   04     .note.gnu.property 
   05     
```


In other works, inside the first loadable segment the linker placed the following sections: .note.gnu.build-id, .gnu.hash, .dynsym, .dynstr, .rel.text, .text, .eh\_frame, .dynamic, .data, and .bss. We can inspect the elf binary with the objdump tool to see what is inside the `.text` section:


``` {style="position: relative;"}

$ objdump -d -M intel elf_explain

elf_explain:     file format elf64-x86-64


Disassembly of section .text:

0000000000400278 <quadruple>:
  400278:       f3 0f 1e fa             endbr64
  40027c:       55                      push   rbp
  40027d:       48 89 e5                mov    rbp,rsp
  400280:       89 7d fc                mov    DWORD PTR [rbp-0x4],edi
  400283:       48 b8 e4 03 40 00 00    movabs rax,0x4003e4
  40028a:       00 00 00 
  40028d:       8b 00                   mov    eax,DWORD PTR [rax]
  40028f:       0f af 45 fc             imul   eax,DWORD PTR [rbp-0x4]
  400293:       89 c1                   mov    ecx,eax
  400295:       48 b8 e0 03 40 00 00    movabs rax,0x4003e0
  40029c:       00 00 00 
  40029f:       8b 10                   mov    edx,DWORD PTR [rax]
  4002a1:       48 b8 e8 03 40 00 00    movabs rax,0x4003e8
  4002a8:       00 00 00 
  4002ab:       8b 00                   mov    eax,DWORD PTR [rax]
  4002ad:       0f af c2                imul   eax,edx
  4002b0:       01 c8                   add    eax,ecx
  4002b2:       5d                      pop    rbp
  4002b3:       c3                      ret

00000000004002b4 <magic>:
  4002b4:       f3 0f 1e fa             endbr64
  4002b8:       55                      push   rbp
  4002b9:       48 89 e5                mov    rbp,rsp
  4002bc:       48 b8 e0 03 40 00 00    movabs rax,0x4003e0
  4002c3:       00 00 00 
  4002c6:       8b 00                   mov    eax,DWORD PTR [rax]
  4002c8:       6b c0 0e                imul   eax,eax,0xe
  4002cb:       5d                      pop    rbp
  4002cc:       c3                      ret
```

Well, no surprises: it's the code of the two functions we defined in `elf_explain.c`.

Putting it all together: the ELF header
---------------------------------------

Neither the SHT nor the PHT have fixed positions, they can be located anywhere in an ELF file. To find them the ELF header is used, which is located at the very start of the file.

The first bytes contain the elf magic `"\x7fELF"`, followed by the class ID (32 or 64 bit ELF file), the data format ID (little endian/big endian), the machine type, etc.

At the end of the ELF header are then pointers to the SHT and PHT. Specifically, the Section Header Table which is used by the linker starts at byte 256 in the ELF file, and the Program Header Table starts at byte 52 (right after the ELF header)

``` {style="position: relative;"}
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0x400278
  Start of program headers:          64 (bytes into file)
  Start of section headers:          1544 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           56 (bytes)
  Number of program headers:         6
  Size of section headers:           64 (bytes)
  Number of section headers:         16
  Section header string table index: 15
```

The entry point of this file is at address 0x400278. This is exactly what we told the linker to do --- make the function `quadruple` the entry point ( this is done with the following directive to the linker from the Makefile `-e quadruple`.

You might ask a question: why are we making the `quadruple()` function to be the entry point? Or in other words, why do we pick it instead of `main()`? The reason is that it gives us the way to know where `quadruple()` is in the text section so we can invoke it right after loading the file. Without making it an entry point we would have to read the symbol table which is doable but would take some additional time.

Program loading in the kernel
-----------------------------

The execution of a program starts inside the kernel, in the exec("/bin/wc",...) system call takes a path to the executable file. The kernel reads the ELF header and the program header table (PHT), followed by lots of sanity checks.

The kernel then loads the parts specified in the `LOAD` directives in the PHT into memory. After that the control can be transferred to the entry point of the program.

Example: load an ELF file
=============================
While ELF might look a bit intimidating, in practice the loading algorithm is rather simple:

- Read the ELF header (This Wiki page should help).
- One of the ELF header fields tells you the offset of the program header table inside the file.
- Read each entry of the program header table (i.e., read each program header)
Each program header has an offset and size of a specific segment inside the ELF file (e.g., a executable code). You have to read it from the file and load it in memory.
- When done with all segments, jump to the entry point of the program. (Note since we don’t control layout of the address space at the moment, we load the segments at some random place in memory (the place that is allocated for us by the mmap() function). Obviously the address of the entry point should be an offset within that random area.

## Using AI + Codex

For this assignment you are allowed to use AI + Codex to implement certain aspects on your code. This section will teach you how to get codex installed in your system. 
To get started we have created a small video on how you can install codex in VS code and start to play around: 
[Installing and Using Codex: Sample Prompt](https://drive.google.com/file/d/1z8Nbytbv5Bl7_j9UPSewapzwZF95UVvP/view?usp=drive_link)

Once you have installed codex, we suggest writing small programs and ask codex to complete/edit files. Give it small tasks as you start to get a feel for it. As always there is the option of first trying to implement the shell on your own from scratch and when you get stuck on certain parts, prompting and asking codex to help you out. Essentially think of codex as your programming buddy. 

Part 1: Build a simple ELF loader
=========================

In this part of the assignment, your task is to build the ELF loader. Use [main.c](./main.c) as a template that provides required header files, structure definitions, and some helper functions and extend it with the functionality of the loader.

Specifically, we ask you to load an ELF file like `elf` which you can compile from [elf.c](./elf.c)

Note: You don't need to perform relocation yet!



Part 2: Explain the crash
=========================
Test your loader with `elf1.c`
Before relocation, your loader will:

- successfully load the program
- crash or compute the wrong result

Use a debugger and disassembly to explain:

- Which instruction fails
- what address it is trying to access
- why that address is invalid
- how this relates to virtual addresses and loading location
- Try to perform relocation and then try again! Explain why it works now!
Submit this explanation as `explain.(txt/md/pdf)`

Perform relocation and compute the result of `linear_transform(5)`

Part 3: ELF Analysis
==================
Answer the following in `explain.(txt/md/pdf)`

- Why does the kernel ignore section headers when loading a program?
- What is the difference between sections and segments?
- Why is `e_entry`? What does it point to?
- Why does loading at a different base address (no relocation) break the program?
- Show Process Virtual Memory Layout Before Loading and After Relocation.  


Part 4: (Extra Credit, 30%):
==================
For extra credit, extend your loader to support position-independent code (PIC). To do this, compile the input binary without the `-no-pic` and `-mcmodel=large` flag. In this case, the compiler will generate RIP-relative memory accesses that go through the Global Offset Table (GOT) instead of using absolute addresses.

When loading such a binary, the GOT will initially contain unresolved entries. Your loader must identify the GOT and apply the appropriate relocations so that each entry points to the correct runtime address. Once the GOT is properly populated, the PIC code should execute correctly from any load address.

[Useful Reference](https://eli.thegreenplace.net/2011/11/11/position-independent-code-pic-in-shared-libraries-on-x64)


Submit your work 
----------------
Submit your solution through Gradescope. Please zip all of your files (main.c, Makefile) and submit them. The structure of the zip file should be the following:
``` {style="position: relative;"}

  - Makefile
  - main.c
  - explain.(txt/md/pdf)
```

![](./images/spacer.gif)

Updated: February, 2025
