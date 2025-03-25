

# HW4: System Calls

This homework asks you to extend the xv6 kernel with several simple system calls.

You will program the xv6 operating system. We are supplying a custom version of xv6 that includes VSCode Debugger integrations natively. For this assignment (and for future xv6 assignments), follow the xv6 [setup instructions](https://mars-research.github.io/cs5460/xv6-setup/). After you\'re done with them, you\'ll be ready to start working on the assignment.

- Custom XV6 Repo: [here](https://github.com/mars-research/xv6-cs5460)
- Native Debug VSCode Extension (make sure you have this for better VSCode->GDB integration): [here](https://marketplace.visualstudio.com/items?itemName=webfreak.debug)

## Exercise 1: Debugging xv6

Use the shortcut `Ctrl/Cmd + K Ctrl/Cmd + O` to open a directory after launching VSCode and connecting to your remote server (CADE presumably). The shortcut will open a panel for you to navigate into the correct directory. If your directory does not exist yet, you can use the Terminal / Command-Line, and follow the xv6 [setup instructions](https://mars-research.github.io/cs5460/xv6-setup/). 

This first part of the assignment teaches you to debug the xv6 kernel with VSCode and GDB. First, let's start the debugger and set a breakpoint on the `main` function.

From inside your `xv6-public` folder, launch QEMU with a VSCode-integrated GDB server:

``` {style="position: relative;"}
CADE$ make qemu-nox-vscode
...
```

Now open the **Run and Debug** tab on VSCode (`Ctrl/Cmd + Shift + D`), and hit the **Attach to QEMU** button. 

Congratulations! The VSCode debugger is connected to xv6. If you shift to the **Debug Console** tab on the VSCode Terminal, you might see the following: 


``` {style="position: relative;"}
GNU gdb 6.8-debian
Copyright (C) 2008 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later 
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
+ target remote localhost:26000
The target architecture is assumed to be i8086
[f000:fff0]    0xffff0: ljmp   $0xf000,$0xe05b
0x0000fff0 in ?? ()
+ symbol-file kernel
```

What you see on the screen is the assembly code of the BIOS that QEMU executes as part of the platform initialization. The BIOS starts at address `0xfff0` (you can read more about it in the [How Does an Intel Processor Boot?](https://binarydebt.wordpress.com/2018/10/06/how-does-an-x86-processor-boot/) blog post. You can single step through the BIOS machine code with the `si` (single instruction) GDB command if you like, but it\'s hard to make sense of what is going on so lets skip it for now and get to the point when QEMU starts executing the xv6 kernel.

> **Note:** if you need to exit GDB you can press **Ctrl-C** and then **Ctrl-D**. To exit xv6 running under QEMU you can terminate it with **Ctrl-A X**.

Now, set a breakpoint at `main()` in `main.c` by clicking on the red circle to the left of the function signature of `main()`. You may need to restart the debugging session for the breakpoint to come into effect. 


## Exercise 2: Breaking inside the bootloader

This exercise asks you to break at the address early in the boot chain of the kernel, i.e., the bootloader and the entry point of the kernel.

Remember that the BIOS loads the kernel bootloader at the address `0x7c00`. The kernel bootloader is implemented in the file `bootasm.S`

``` {style="position: relative;"}
# Start the first CPU: switch to 32-bit protected mode, jump into C.
# The BIOS loads this code from the first sector of the hard disk into
# memory at physical address 0x7c00 and starts executing in real mode
# with %cs=0 %ip=7c00.

.code16                       # Assemble for 16-bit mode
.globl start
start:
  cli                         # BIOS enabled interrupts; disable

  # Zero data segment registers DS, ES, and SS.
  xorw    %ax,%ax             # Set %ax to zero
  movw    %ax,%ds             # -> Data Segment
  movw    %ax,%es             # -> Extra Segment
  movw    %ax,%ss             # -> Stack Segment
```

Lets try to set the breakpoint at this address. First, exit both the debugger and QEMU. Then add a breakpoint at the `cli` instruction in `start:`. Restart the debugger. 

Now single step your execution (execute it one machine instruction at a time). Remember that the `0x7c00` address is defined in the assembly file, `bootasm.S` (the entry point of the boot loader). 

If you single-step a couple of times you eventually reach the C code of the `bootmain()` function that implements loading of the kernel from disk (it\'s implemented in the `bootmain.c` file). Note that GDB doesn\'t pick up the source file information since it\'s using the debugging info for the kernel, not for the bootblock which are two different programs. You can however follow what is going on by comparing the instructions that you execute with the `si` GDB command against the `bootblock.asm` file \-\-- a helper file generated by the compiler that interleaves assembly instructions with the source.

Note the first `call` instruction. Right before it, the first stack was set up (this will be useful in the question about the stack below).

## Part 1 (40%): Memtop system call

Now you\'re ready to start on the main part of the homework in which you will add a new system call to the xv6 kernel. The main point of the exercise is for you to see some of the different pieces of the system call machinery.

Your new system call will print the stats about available and used system memory.

Specifically, your new system call will have the following interface:

``` {style="position: relative;"}
 int memtop(); 
```

It takes no arguments and returns the amount of memory available in the system. When you invoke it from your test program `mtop` you should print the number in bytes on the console:

``` {style="position: relative;"}
$ mtop
available memory: 29712384
```

In order to test your system call you should create a user-level program `mtop` that calls your new system call. In order to make your new `mtop` program available to run from the xv6 shell, look at how other programs are implemented, e.g., `ls` and `wc` and make appropriate modifications to the Makefile such that the new application gets compiled, linked, and added to the xv6 filesystem.

When you\'re done, you should be able to invoke your `mtop` program from the shell. You can follow the following example template for `bt.c`, but feel free to extend it in any way you like:

``` {style="position: relative;"}
#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  /* Syscall invocation here */

  exit();
}
```

In order to make your new `mtop` program available to run from the xv6 shell, add `_mtop` to the `UPROGS` definition in `Makefile`.

Your strategy for making the `memtop` system call should be to clone all of the pieces of code that are specific to some existing system call, for example the \"uptime\" system call or \"read\". You should grep for uptime in all the source files, using `grep -n uptime *.[chS]`.

## Some hints

To count up the available system memory, you should walk the linked list used by the memory allocator and count how many pages are still available on that list.

## Submit your work

Submit your solution through as a compressed tar file of your xv6 source tree (after running make clean). You can use the following command to create a compressed tar file.

``` {style="position: relative;"}
CADE$ cd xv6-public
CADE$ make clean
CADE$ cd ..
CADE$ zip -r hw5.zip xv6-public
```
