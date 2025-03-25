
# HW4: System Calls

This homework teaches you how to set up xv6, start debugging it and finally extend it with a simple system call.


You will program the xv6 operating system. We suggest you use a custom version of xv6 that includes small modifications that support VSCode Debugger integration. For this assignment (and for future xv6 assignments), follow the xv6 [setup instructions](https://mars-research.github.io/cs5460/xv6-setup/). After you\'re done with them, you\'ll be ready to start working on the assignment.

- Custom XV6 Repo: [here](https://github.com/mars-research/xv6-cs5460)
- Native Debug VSCode Extension (make sure you have this for better VSCode->GDB integration): [here](https://marketplace.visualstudio.com/items?itemName=webfreak.debug)

## Exercise 1: Debugging xv6

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

## Part 1: Breaking inside the kernel and explaining what is on the stack (20%)

Now, set a breakpoint at the `freerange()` funciton in `kalloc.c` by clicking on the red circle to the left of the function signature of `freerange()`. You may need to restart the debugging session for the breakpoint to come into effect. 

When breakpoint is triggered you can enter GDB commands in the Debug Console to inspect state of the registers and dump 24 bytes pointed by the `esp` register.

```
(gdb) info reg
...
(gdb) x/24x $esp
0x8010b590 <stack+4048>:	0x00010094	0x00010094	0x8010b5b8	0x80102fc1
0x8010b5a0 <stack+4064>:	0x801154a8	0x80400000	0x00000000	0x00000000
0x8010b5b0 <stack+4080>:	0x8010b5c4	0x00010094	0x00007bf8	0x00000000
0x8010b5c0 <bcache>:	0x00000000	0x00000000	0x00000000	0x00000000
0x8010b5d0 <bcache+16>:	0x00000000	0x00000000	0x00000000	0x00000000
0x8010b5e0 <bcache+32>:	0x00000000	0x00000000	0x00000000	0x00000000
(gdb)
```

Your assignment is to explain every value you see in this dump. Note, not all of it is stack (explain why). Also to better understand the meaning of the dump, you can use 
additional debugging features. For example, you can try to inspect the assembly code you are executing: 

```
(gdb) disas
```

You can try to set a breakpoint right when the stack is initialized in `entry.S`. Note VSCode might not let you to set a breakpoint on the assembly instruction, but you 
can use GDB directly. 

```
(gdb) disas entry
Dump of assembler code for function entry:
   0x8010000c <+0>:	mov    %cr4,%eax
   0x8010000f <+3>:	or     $0x10,%eax
   0x80100012 <+6>:	mov    %eax,%cr4
   0x80100015 <+9>:	mov    $0x109000,%eax
   0x8010001a <+14>:	mov    %eax,%cr3
   0x8010001d <+17>:	mov    %cr0,%eax
   0x80100020 <+20>:	or     $0x80010000,%eax
   0x80100025 <+25>:	mov    %eax,%cr0
   0x80100028 <+28>:	mov    $0x8010b5c0,%esp
   0x8010002d <+33>:	mov    $0x80102fa0,%eax
   0x80100032 <+38>:	jmp    *%eax
   0x80100034 <+40>:	xchg   %ax,%ax
   0x80100036 <+42>:	xchg   %ax,%ax
   0x80100038 <+44>:	xchg   %ax,%ax
   0x8010003a <+46>:	xchg   %ax,%ax
   0x8010003c <+48>:	xchg   %ax,%ax
   0x8010003e <+50>:	xchg   %ax,%ax
End of assembler dump.
```

Now you can set a breakpoint at the assembly instruction that initializes the stack (`0x80100028 <+28>:    mov    $0x8010b5c0,%esp`) and trace what is happening to the 
stack by single stepping execution with the step instruction (`si`) command.  

Eventually you will reach the C code of the `freerange()` function where you set your original breakpoint. 

## Part 2 (80%): process create system call

Now you\'re ready to start on the main part of the homework in which you will add a new `pcreate()` system call to the xv6 kernel. The main point of the exercise is for you to see some of the different pieces of the system call machinery as well as the internals of process creation in xv6.

Your new system call will serve as a replacement for `fork()` and `exec()` or more specifically, it will allow you to create new processes without forking. 

Specifically, your new system call will have the following interface:

``` {style="position: relative;"}
 int pcreate(char *path, char **argv, int fds[16]); 
```

Here similar to `exec()` the `pcreate()` takes the `path` to the binary of the new proces, it's arguments `**argv` and an aditional argument an array that specifies how the file decriptors of the parent are shared with the child. For example if `fds` contains `{3, 4, -1, -1, -1, ...}` it means that file descriptors 3 and 4 of the caller are copied into thefile descriptors 0 and 1 of the new process. -1 means that the file decriptor will be unallocated in the new process. At a high-level, it seems that `pcreate()` can replace the `fork()` and `exec()` combination in most cases.  

In order to test your system call you should create a user-level program `ptest` that uses your new system call to create new processes. In order to make your new `ptest` program available to run from the xv6 shell, look at how other programs are implemented, e.g., `ls` and `wc` and make appropriate modifications to the Makefile such that the new application gets compiled, linked, and added to the xv6 filesystem.

When you\'re done, you should be able to invoke your `ptest` program from the shell. You can follow the following example template for `bt.c`, but feel free to extend it in any way you like:

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

In order to make your new `ptest` program available to run from the xv6 shell, add `_ptest` to the `UPROGS` definition in `Makefile`.

Your strategy for making the `pcreate` system call should be to clone all of the pieces of code that are specific to some existing system call, for example the \"uptime\" system call or \"read\". You should grep for uptime in all the source files, using `grep -n uptime *.[chS]`. You can also copy the code from the `exec()` system call to create the new process. 


## Submit your work

Submit your solution through as a compressed tar file of your xv6 source tree (after running make clean). You can use the following command to create a compressed tar file.

``` {style="position: relative;"}
CADE$ cd xv6-public
CADE$ make clean
CADE$ cd ..
CADE$ zip -r hw5.zip xv6-public
```
