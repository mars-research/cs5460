
# HW4: System Calls

This homework teaches you how to set up xv6, start debugging it and finally extend it with a simple system call.


You will program the xv6 operating system. We suggest you use a custom version of xv6 that includes small modifications that support VSCode Debugger integration. For this assignment (and for future xv6 assignments), follow the xv6 [setup instructions](https://mars-research.github.io/cs5460/xv6-setup/). After you\'re done with them, you\'ll be ready to start working on the assignment.

- Custom XV6 Repo: [here](https://github.com/mars-research/xv6-cs5460)
- Native Debug VSCode Extension (make sure you have this for better VSCode->GDB integration): [here](https://marketplace.visualstudio.com/items?itemName=webfreak.debug)

## Exercise 1: Debugging xv6

Use the shortcut `Ctrl/Cmd + K Ctrl/Cmd + O` to open a directory after launching VSCode and connecting to your remote server (CADE presumably). The shortcut will open a panel for you to navigate into the correct directory. If your directory does not exist yet, you can use the Terminal / Command-Line, and follow the xv6 [setup instructions](https://mars-research.github.io/cs5460/xv6-setup/). 

This first part of the assignment teaches you to debug the xv6 kernel with VSCode and GDB. First, let's start the debugger and set a breakpoint on the `main` function.

From inside your `xv6-cs5460` folder, launch QEMU (our version of the repo will automatically generate a json template in `.vscode/launch.json` which will allow your vscode to attach gdb to this qemu process:

``` {style="position: relative;"}
CADE$ make qemu-nox-gdb
...
```

Now open the **Run and Debug** tab on VSCode (`Ctrl/Cmd + Shift + D`), and hit the **Attach to QEMU** button.  

Congratulations! The VSCode debugger is connected to xv6. If you shift to the **Debug Console** tab on the VSCode Terminal, you might see the following: 


``` {style="position: relative;"}
Reading symbols from /home/u0478645/projects/cs5460/xv6-cs5460/kernel...
Warning: 'set target-async', an alias for the command 'set mi-async', is deprecated.
Use 'set mi-async'.

0x0000fff0 in ?? ()
+ symbol-file kernel
add symbol table from file "kernel"
Continue with 'c' or 'continue'
```

The GDB is ready to start running the assembly code of the BIOS that QEMU executes as part of the platform initialization. The BIOS starts at address `0xfff0` (you can read more about it in the [How Does an Intel Processor Boot?](https://binarydebt.wordpress.com/2018/10/06/how-does-an-x86-processor-boot/) blog post. You can single step through the BIOS machine code with the `si` (single instruction) GDB command if you like, but it\'s hard to make sense of what is going on so lets skip it for now and get to the point when QEMU starts executing the xv6 kernel.

At the bottom of your Debug Console you can enter GDB commands, like `c` (short for `continue`) to continue booting (you can also click the continue button at the top).

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

You can also set a breakpoint at the entry point of the kernel (before the
instruction that initializes the stack, i.e.,`0x80100028 <+28>:    mov
$0x8010b5c0,%esp`and trace what is happening to the stack by single stepping
execution with the step instruction (`si`) command. 

**Note** There is a little trick here. The kernel is linked to run at 2GB + 1MB, but since the boot loader loads it at the 1MB address, the entry point of the kernel ELF file is still set to to the 1MB range. E.g., you can verify this by running `readelf` like this: 

```
\>readelf kernel -a
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
  Entry point address:               0x10000c
```

While the actual linked address of the `entry` symbol is `0x8010000c` 

```
\>readelf kernel -a |grep entry
    83: 8010000c     0 NOTYPE  GLOBAL DEFAULT    1 entry
```

Ok, so to set a breakpoint we will subtract these 2GB (`0x80000000` in hex) like this: on start in the Debug Console use the `b` (breakpoint) command: 

```
>b *0x10000c
```

If you restart the GDB session, set the breakpoint, and hit continue you will
break on the entry point of the kernel (you can re-watch the boot section to
recall what is going on). 

**Note** since the `entry` is linked at 2MB + 1GB but the instruction pointer
is at 1MB mark GDB cannot match the addresses and show you a disassembly. To
force the disassembly of the current code use this command (disassemble
instructions pointed by the `eip` + 16 bytes): 

```
disas $eip,+16
Dump of assembler code from 0x10000c to 0x10001c:
=> 0x0010000c:	mov    %cr4,%eax
   0x0010000f:	or     $0x10,%eax
   0x00100012:	mov    %eax,%cr4
   0x00100015:	mov    $0x109000,%eax
   0x0010001a:	mov    %eax,%cr3
End of assembler dump.
```

This is the power of GDB... such tricks are hard or impossible to do with VSCode. 

Now use `si` (step instruction) or `ni` (next instruction) to single step until you reach `freerange()`. You can monitor what is going on on the stack after each instruction

Eventually you will reach the C code of the `freerange()` function where you set your original breakpoint.

Note, it's a bit annoying to invoke one by one, so you can combine them in user-defined functions, like for example a command that single steps and prints the disassembly in a single shot. Add this to the `.gdbinit.tmpl` file: 

```
define mystep
  si
  disas $eip,+16
end
```

Now you can use `mystep` as a GDB command. You can also add stack dumping there so you can monitor the stack changes.

 

**Remember** your goal is to explain every value you see when you dump the stack. 


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

### Hints

You will want to allocate a new process similar to how it's done in `fork()` or `userinit()`.

Since it's the new process you will have to create a correct trapframe, look at how it's done in `userinit()`

## Extra credit (10%)

Use your new `pcreate()` implementation to create the first process in the system, i.e., instead of using the assembly sequence use internals of `pcreate()` to load the ELF binary of the `shell` process from disk. 


## Submit your work

Submit your solution through as a compressed zip file of your xv6 source tree (after running make clean). You can use the following command to create a compressed zip file.

``` {style="position: relative;"}
CADE$ cd xv6-public
CADE$ make clean
CADE$ cd ..
CADE$ zip -r hw4.zip xv6-public
```

``` {style="position: relative;"}
CADE$ cd xv6-cs5460
CADE$ make clean
CADE$ cd ..
CADE$ zip -r hw4.zip xv6-cs5460
```
