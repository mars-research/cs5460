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
$ readelf -a elf
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

readelf -a elf

```

If you scroll down to the **Section headers** you will see all "sections" of the ELF file that the linker can use:

 ``` {style="position: relative;"}

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .note.gnu.build-i NOTE            080480b4 0000b4 000024 00   A  0   0  4
  [ 2] .gnu.hash         GNU_HASH        080480d8 0000d8 000018 04   A  3   0  4
  [ 3] .dynsym           DYNSYM          080480f0 0000f0 000010 10   A  4   1  4
  [ 4] .dynstr           STRTAB          08048100 000100 000001 00   A  0   0  1
  [ 5] .rel.text         REL             08048104 000104 000020 08  AI  3   6  4
  [ 6] .text             PROGBITS        08048124 000124 00002d 00 WAX  0   0  1
  [ 7] .eh_frame         PROGBITS        08048154 000154 000000 00   A  0   0  4
  [ 8] .dynamic          DYNAMIC         08048154 000154 000080 08  WA  4   0  4
  [ 9] .data             PROGBITS        080481d4 0001d4 000008 00  WA  0   0  4
  [10] .bss              NOBITS          080481dc 0001dc 000004 00  WA  0   0  4
  [11] .comment          PROGBITS        00000000 0001dc 00002d 01  MS  0   0  1
  [12] .symtab           SYMTAB          00000000 00020c 000180 10     13  16  4
  [13] .strtab           STRTAB          00000000 00038c 000050 00      0   0  1
  [14] .shstrtab         STRTAB          00000000 0003dc 000079 00      0   0  1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  p (processor specific)
```

The elf.c is a simple program, but it still has .text, `.data`, `.bss`, and `.rel.text` (relocation) sections and a bunch of sections that contain symbol information and a ton of other stuff.

Since `elf.c` has two initialized and one uninitialized global variables the ELF has data section of 8 bytes and the BSS section of 4 bytes (each variable is a 4 byte integer).

The program is linked elf.c to run at address 0x08048000 which is the default program linking address on UNIX systems (the reasons are choosing this address are historical and not entirely clear but it is what it is (see this short email for some quesses [Reorganizing the address space](https://lwn.net/Articles/91985/)

The symbol table contains the following symbols

``` {style="position: relative;"}

Symbol table '.symtab' contains 24 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
     0: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND
     1: 080480b4     0 SECTION LOCAL  DEFAULT    1
     2: 080480d8     0 SECTION LOCAL  DEFAULT    2
     3: 080480f0     0 SECTION LOCAL  DEFAULT    3
     4: 08048100     0 SECTION LOCAL  DEFAULT    4
     5: 08048104     0 SECTION LOCAL  DEFAULT    5
     6: 08048124     0 SECTION LOCAL  DEFAULT    6
     7: 08048154     0 SECTION LOCAL  DEFAULT    7
     8: 08048154     0 SECTION LOCAL  DEFAULT    8
     9: 080481d4     0 SECTION LOCAL  DEFAULT    9
    10: 080481dc     0 SECTION LOCAL  DEFAULT   10
    11: 00000000     0 SECTION LOCAL  DEFAULT   11
    12: 00000000     0 FILE    LOCAL  DEFAULT  ABS elf.c
    13: 00000000     0 FILE    LOCAL  DEFAULT  ABS
    14: 08048154     0 OBJECT  LOCAL  DEFAULT    8 _DYNAMIC
    15: 080481d4     0 OBJECT  LOCAL  DEFAULT    9 _GLOBAL_OFFSET_TABLE_
    16: 080481d4     4 OBJECT  GLOBAL DEFAULT    9 b
    17: 08048124    32 FUNC    GLOBAL DEFAULT    6 quadruple
    18: 080481d8     4 OBJECT  GLOBAL DEFAULT    9 c
    19: 080481dc     0 NOTYPE  GLOBAL DEFAULT   10 __bss_start
    20: 080481dc     4 OBJECT  GLOBAL DEFAULT   10 d
    21: 08048144    13 FUNC    GLOBAL DEFAULT    6 magic
    22: 080481dc     0 NOTYPE  GLOBAL DEFAULT    9 _edata
    23: 080481e0     0 NOTYPE  GLOBAL DEFAULT   10 _end
```

I.e., it contains names of the two functions quadruple and magic, three global variables b, c, and d. Other symbols like \_\_bss\_start, \_edata, and \_end are added by the linker to mark the start and end of the BSS, TEXT, and DATA sections.

Execution view: Program Header Table (PHT)
------------------------------------------

The Program Header Table contains information for the kernel on how to start the program. The `LOAD` directives determinate what parts of the ELF file get mapped into program memory.

In our elf example the program header defines four segments, but only two of them should be loaded by the operating system in memory to run.

``` {style="position: relative;"}

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x0000b4 0x080480b4 0x080480b4 0x00128 0x0012c RWE 0x4
  DYNAMIC        0x000154 0x08048154 0x08048154 0x00080 0x00080 RW  0x4
  NOTE           0x0000b4 0x080480b4 0x080480b4 0x00024 0x00024 R   0x4
  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RW  0x10
```


The only loadable section is linked to be loaded at address 0x080480b4. We can also look at the mappings of sections to segments (we will discuss what sections are below):

``` {style="position: relative;"}
 Section to Segment mapping:
  Segment Sections...
   00     .note.gnu.build-id .gnu.hash .dynsym .dynstr .rel.text .text .eh_frame .dynamic .data .bss
   01     .dynamic
   02     .note.gnu.build-id
   03
```


In other works, inside the first loadable segment the linker placed the following sections: .note.gnu.build-id, .gnu.hash, .dynsym, .dynstr, .rel.text, .text, .eh\_frame, .dynamic, .data, and .bss. We can inspect the elf binary with the objdump tool to see what is inside the `.text` section:


``` {style="position: relative;"}

$ objdump -d -M intel elf

elf:     file format elf32-i386


Disassembly of section .text:

08048124 :
 8048124:       55                      push   ebp
 8048125:       89 e5                   mov    ebp,esp
 8048127:       a1 d8 81 04 08          mov    eax,ds:0x80481d8
 804812c:       0f af 45 08             imul   eax,DWORD PTR [ebp+0x8]
 8048130:       89 c2                   mov    edx,eax
 8048132:       8b 0d d4 81 04 08       mov    ecx,DWORD PTR ds:0x80481d4
 8048138:       a1 dc 81 04 08          mov    eax,ds:0x80481dc
 804813d:       0f af c1                imul   eax,ecx
 8048140:       01 d0                   add    eax,edx
 8048142:       5d                      pop    ebp
 8048143:       c3                      ret

08048144 :
 8048144:       55                      push   ebp
 8048145:       89 e5                   mov    ebp,esp
 8048147:       a1 d4 81 04 08          mov    eax,ds:0x80481d4
 804814c:       6b c0 0e                imul   eax,eax,0xe
 804814f:       5d                      pop    ebp
 8048150:       c3                      ret
```

Well, no surprises: it's the code of the two functions we defined in `elf.c`.

Putting it all together: the ELF header
---------------------------------------

Neither the SHT nor the PHT have fixed positions, they can be located anywhere in an ELF file. To find them the ELF header is used, which is located at the very start of the file.

The first bytes contain the elf magic `"\x7fELF"`, followed by the class ID (32 or 64 bit ELF file), the data format ID (little endian/big endian), the machine type, etc.

At the end of the ELF header are then pointers to the SHT and PHT. Specifically, the Section Header Table which is used by the linker starts at byte 256 in the ELF file, and the Program Header Table starts at byte 52 (right after the ELF header)

``` {style="position: relative;"}
ELF Header:
  Magic:   7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00
  Class:                             ELF32
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           Intel 80386
  Version:                           0x1
  Entry point address:               0x8048124
  Start of program headers:          52 (bytes into file)
  Start of section headers:          1112 (bytes into file)
  Flags:                             0x0
  Size of this header:               52 (bytes)
  Size of program headers:           32 (bytes)
  Number of program headers:         4
  Size of section headers:           40 (bytes)
  Number of section headers:         15
  Section header string table index: 14
```

The entry point of this file is at address 0x8048124. This is exactly what we told the linker to do --- make the function `quadruple` the entry point ( this is done with the following directive to the linker from the Makefile `-e quadruple`.

You might ask a question: why are we making the `quadruple` function to be the entry point? Otherwise, it's not `main`, and really nothing is special about this function. The reason is that it gives us the way to know where quadruple is in the text section so we can invoke it after loading the file. Without making it an entry point we would have to read the symbol table which is doable but would take some additional time.

Program loading in the kernel
-----------------------------

The execution of a program starts inside the kernel, in the exec("/bin/wc",...) system call takes a path to the executable file. The kernel reads the ELF header and the program header table (PHT), followed by lots of sanity checks.

The kernel then loads the parts specified in the `LOAD` directives in the PHT into memory. After that the control can be transferred to the entry point of the program.

## Using AI + Codex
For this assignment you are allowed to use AI + Codex to implement certain aspects on your code. This section will teach you how to get codex installed in your system. 
To get started we have created a small video on how you can install codex in VS code and start to play around: 
[Installing and Using Codex: Sample Prompt](https://drive.google.com/file/d/1z8Nbytbv5Bl7_j9UPSewapzwZF95UVvP/view?usp=drive_link)

Once you have installed codex, we suggest writing small programs and ask codex to complete/edit files. Give it small tasks as you start to get a feel for it. As always there is the option of first trying to implement the shell on your own from scratch and when you get stuck on certain parts, prompting and asking codex to help you out. Essentially think of codex as your programming buddy. 

Part 1: Build the loader
=========================
In this part of the assignment, your task is to build the ELF loader from scratch. You are given only the required header files, structure definitions, and some helper functions. All loader functionality must be implemented by you.
Use the loader to load `elf.c`
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
Modify your ELF loader to handle relocation for ELF binaries that make use of the Global Offset Table (GOT).


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
