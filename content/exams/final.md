## Q1.1
Alice wants to make fun of Bob. On Bob's birthday, she replaces the `cat` program in Bob's xv6 system with a patched version of `cat` that always prints `Happy Birthday, Bob!` no matter the input. 

Here is the original version of `cat.c` from the xv6 source tree: 

```
  1 #include "types.h"
  2 #include "stat.h"
  3 #include "user.h"
  4
  5 char buf[512];
  6
  7 void
  8 cat(int fd)
  9 {
 10   int n;
 11
 12   while((n = read(fd, buf, sizeof(buf))) > 0) {
 13     if (write(1, buf, n) != n) {
 14       printf(1, "cat: write error\n");
 15       exit();
 16     }
 17   }
 18   if(n < 0){
 19     printf(1, "cat: read error\n");
 20     exit();
 21   }
 22 }
 23
 24 int
 25 main(int argc, char *argv[])
 26 {
 27   int fd, i;
 28
 29   if(argc <= 1){
 30     cat(0);
 31     exit();
 32   }
 33
 34   for(i = 1; i < argc; i++){
 35     if((fd = open(argv[i], 0)) < 0){
 36       printf(1, "cat: cannot open %s\n", argv[i]);
 37       exit();
 38     }
 39     cat(fd);
 40     close(fd);
 41   }
 42   exit();
 43 }
```

Can you help Alice to develop the patched version to make fun of Bob? Type the source code:

**Answer**
```
int
main(int argc, char *argv[])
{
  printf(1, "Happy Birthday, Bob!\n");
  exit();
}
```

## Q1.2

Bob (who is a good systems student) figures out Alice's joke, yet he still uses his xv6 system like nothing had happened. I.e., does need to cat files throughout the day. How does he do this? Be specific, explain how Bob managed to find a replacement for `cat`

**Answer**

Bob remembers that xv6 has `grep` and decides he can filter nothing using `grep` instead of `cat` (he looked at the source code to figure out the syntax):

```
grep . README
````


## Q2.1

Which lines in the xv6 source code are responsible for setting up execution in privilege level 3, i.e., in user mode? Explain your answer (feel free to use xv6 code printout).

**Answer**

xv6 initializes segments in `seginit()` function:

```
1714 void
1715 seginit(void)
1716 {
1717 struct cpu *c;
1718
1719 // Map "logical" addresses to virtual addresses using identity map.
1720 // Cannot share a CODE descriptor for both kernel and user
1721 // because it would have to have DPL_USR, but the CPU forbids
1722 // an interrupt from CPL=0 to DPL=3.
1723 c = &cpus[cpuid()];
1724 c?>gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0);
1725 c?>gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
1726 c?>gdt[SEG_UCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, DPL_USER);
1727 c?>gdt[SEG_UDATA] = SEG(STA_W, 0, 0xffffffff, DPL_USER);
1728 lgdt(c?>gdt, sizeof(c?>gdt));
1729 }
```

Specifically lines `1726` and `1727` set up two segments (code and data) with `DPL_USER`. 

For the first process xv6 sets up the `cs` segment to `DPL_USER` explicitly. Later on, for each new process (the only way to create a process in xv6 is to call `fork()`) the trap frame of the parent is copied to the child as 

```
*np->tf = *curproc->tf;
```

So `cs` stays as `DPL_USER`. Finally, the `iret` instruction in `trapret` loads the `tf->cs` into the `CS` register of the CPU changing the privilege level. 

**Note** this question assumes that you did HW4 (system calls) and thought of how to exit into user level. 

## Q2.2

What is the very first instruction that executes at privilege level 3? Explain your answer

**Answer**

The very first code that executes at PL3 is the code of the userinit process: 

```
  8 # exec(init, argv)
  9 .globl start
 10 start:
 11   pushl $argv
 12   pushl $init
 13   pushl $0  // where caller pc would be
 14   movl $SYS_exec, %eax
 15   int $T_SYSCALL
 ```
 
It happens to be `pushl $argv` which pushes the arguments for the `exec()` system call that execs into `init`.

## Q3

xv6 uses `entrypgdir` as the boot-time page table 

```
1300 // The boot page table used in entry.S and entryother.S.
1301 // Page directories (and page tables) must start on page boundaries,
1302 // hence the __aligned__ attribute.
1303 // PTE_PS in a page directory entry enables 4Mbyte pages.
1304
1305 __attribute__((__aligned__(PGSIZE)))
1306 pde_t entrypgdir[NPDENTRIES] = {
1307 // Map VA?s [0, 4MB) to PA?s [0, 4MB)
1308 [0] = (0) | PTE_P | PTE_W | PTE_PS,
1309 // Map VA?s [KERNBASE, KERNBASE+4MB) to PA?s [0, 4MB)
1310 [KERNBASE>>PDXSHIFT] = (0) | PTE_P | PTE_W | PTE_PS,
1311 };
```

## Q3.1

What happens if you set entry `0` in `entrypgdir` to 0? Explain your answer, be specific, which line in xv6 will trigger an exception?

Since xv6 is still executing at lower addresses (i.e., around 1MB) the moment we load `entrypgdir` into `CR3` the address from which we try to execute the next instruction will not be mapped and will trigger a page fault. I.e., the `0` entry maps addresses from 0 to 4MB. 

```
 43 # Entering xv6 on boot processor, with paging off.
 44 .globl entry
 45 entry:
 46   # Turn on page size extension for 4Mbyte pages
 47   movl    %cr4, %eax
 48   orl     $(CR4_PSE), %eax
 49   movl    %eax, %cr4
 50   # Set page directory
 51   movl    $(V2P_WO(entrypgdir)), %eax
 52   movl    %eax, %cr3
 53   # Turn on paging.
 54   movl    %cr0, %eax
 55   orl     $(CR0_PG|CR0_WP), %eax
 56   movl    %eax, %cr0
 57
 58   # Set up the stack pointer.
 59   movl $(stack + KSTACKSIZE), %esp
 60
 61   # Jump to main(), and switch to executing at
 62   # high addresses. The indirect call is needed because
 63   # the assembler produces a PC-relative instruction
 64   # for a direct jump.
 65   mov $main, %eax
 66   jmp *%eax
 67
 68 .comm stack, KSTACKSIZE
 ```
 
 Here line `56` will rigger a page fault as it's the line which enables paging. There might be a case when kernel is booted by a boot loader that leaves the paging on. In this case the fault will be triggered on line `54`. 
 
 ## Q3.2
 
 What happens if you set entry `512` (i.e., `KERNBASE>>PDXSHIFT`) in `entrypgdir` to 0? Explain your answer, be specific, which line in xv6 will trigger an exception?

The kernel will crash when it will try to jump to the high addresses 
at around 2GB + 1MB. Remember the kernel is linked and relocated to run 
at 2GB + 1MB, while a short assembly sequence starts executing at 1MB, it will eventually try to jump to `main`. Line `66` will trigger a fault.

## Q4

Alice starts the `cat` process on her xv6 system. When `cat` starts running, how many entries in the `cat`'s page table directory (i.e., root of the page table) are non-zero (i.e., configured by the kernel and have the present flag set)? Explain your answer.

We first identify the size of the `cat` binary. Use `readelf -a` to see that has one loadable section of size `0x00b4c`. Alternatively you can just guess that it's small and fits in one page, or use `objdump` to see the address of the last assembly instruction and guess that the data and BSS sections are small. 

Hence the user part of the page table will map 3 pages: text/data, guard and stack. The kernel part will follow the `kmap` and will map all the memory from 2GB to PHYSTOP (0xE000000/0x1000 = 57344 pages) and from DEVSPACE to 4GB ((0x1_0000_0000 - 0xFE000000)/0x1000 = 8192). The leaves of the page table will have 3 + 57344 + 8192 valid entries. You can also discuss how many root entries will be needed: 1 + 14 + 2 = 17. 

## Q5

Below is the printout of the xv6 `swtch` function :

```
3050 # Context switch
3051 #
3052 # void swtch(struct context **old, struct context *new);
3053 #
3054 # Save the current registers on the stack, creating
3055 # a struct context, and save its address in *old.
3056 # Switch stacks to new and pop previously?saved registers.
3057
3058 .globl swtch
3059 swtch:
3060 movl 4(%esp), %eax
3061 movl 8(%esp), %edx
3062
3063 # Save old callee?saved registers
3064 pushl %ebp
3065 pushl %ebx
3066 pushl %esi
3067 pushl %edi
3068
3069 # Switch stacks
3070 movl %esp, (%eax)
3071 movl %edx, %esp
3072
3073 # Load new callee?saved registers
3074 popl %edi
3075 popl %esi
3076 popl %ebx
3077 popl %ebp
3078 ret
```

### Q5.1

Alice (a student in cs5460) is wondering why register `eax` is not saved and restored inside `swtch`. Can you provide the explanation?

**Answer** eax is a caller-saved register so it's already pushed on the stack (if needed) by the caller of `swtch`. 

And of course, the user value of `eax` was saved inside `alltraps` by the `pushal` instruction. 

### Q5.2

Alice thinks that saving the context of the process on the stack is quite lame. Instead, she wants to change the `swtch` function to save and restore the context of the process into the context data structure that is passed into the `swtch` as a reference. Specifically, she changes the signature of the `swtch` function to take references to the "from" and "to" contexts like this: 

```
void swtch(struct context *from, struct context *to);
```

And then patches the `swtch` to work. Can you sketch the assembly code for the new `swtch` function which Alice wants to build?

**Answer**

```
 31 .globl swtch
 32 swtch:
 33   movl 4(%esp), %eax
 34   movl 8(%esp), %edx
 35
 36   # Save old callee-saved registers
 37   movl %ebp, 12(%eax)
 38   movl %ebx, 8(%eax)
 39   movl %esi, 4(%eax)
 40   movl %edi, (%eax)
 41
 42   # Switch stacks
 43   movl %esp, 16(%eax) # use eip field for saving stack (a bit ugly)
 44   movl 16(%edx), %esp
 45
 46   # Load new callee-saved registers
 47   movl %edx, %edi
 48   movl 4(%edx), %esi
 49   movl 8(%edx), %ebx
 50   movl 12(%edx), %ebp
 51   ret
 ```

Here Alice saves all callee-saved register into the `struct context` which is passed on the stack and is saved into `eax` and then restores the registers from the `to` context (in `edx`). She saves esp as part of the context but leaves the return address on the stack. 

## Q6

Below is the source code of the `loaduvm()` function that loads the text and data section of the ELF file into the user memory during `exec()` (remember you used this function in HW4 to implement process create): 

```
1900 // Load a program segment into pgdir. addr must be page?aligned
1901 // and the pages from addr to addr+sz must already be mapped.
1902 int
1903 loaduvm(pde_t *pgdir, char *addr, struct inode *ip, uint offset, uint sz)
1904 {
1905   uint i, pa, n;
1906   pte_t *pte;
1907
1908   if((uint) addr % PGSIZE != 0)
1909     panic("loaduvm: addr must be page aligned");
1910   for(i = 0; i < sz; i += PGSIZE){
1911     if((pte = walkpgdir(pgdir, addr+i, 0)) == 0)
1912       panic("loaduvm: address should exist");
1913     pa = PTE_ADDR(*pte);
1914     if(sz ? i < PGSIZE)
1915       n = sz ? i;
1916     else
1917       n = PGSIZE;
1918     if(readi(ip, P2V(pa), offset+i, n) != n)
1919       return ?1;
1920   }
1921   return 0;
1922 }
```

Alice is wondering what happens if she replaces `walkpgdir()` with `P2V(addr+i)` and then passes the result into `readi()`. Can you explain what can go wrong?

** Answer **

First, `addr + i` is a virtual address so it doesn't make sense to use the `P2V` macro on it as it will result in a rogue address (i.e., addr + i - 2GB). So a blind `readi(ip, P2V(addr+i), ...)` will likely result into a readi into the 0x0 - 2GB = 2GB address which is mapped with the write permissions in the kernel. The `readi()` succeeds by overwriting just a few pages around 2GBs (depending on the size of the text/data/bss sections of the process we're creating) however of course when the user process starts running it will have zeroes (clean pages) instead of its code and data section. It will eventually crash. 

But also the real intent of walking the page table for the `addr+i` is to get the physical address of the page that backs the `addr+i` address of the new address space we're creating with `exec()`. In xv6 the only way to get the physical address of the page that backs up a virtual address of the user part of the address space is to walk the page table. The physical address found in the page table can then be used in `P2V` as the kernel maps all available physical memory from 0 to PHYSTOP to [2GB: 2GB + PHYSTOP] range. 

## Q7

On her xv6 system, Alice types `echo hello` in the xv6 shell and hits enter. If she were to trace the execution of each system call performed by xv6, what would this trace look like? I.e., which system calls are executed until she sees the next command prompt? Explain your answer. If you don't know internals of the xv6 shell, imagine you're running the shell that you built in HW1. Echo is a very simple program with the following code:

```
  1 #include "types.h"
  2 #include "stat.h"
  3 #include "user.h"
  4
  5 int
  6 main(int argc, char *argv[])
  7 {
  8   int i;
  9
 10   for(i = 1; i < argc; i++)
 11     printf(1, "%s%s", argv[i], i+1 < argc ? " " : "\n");
 12   exit();
 13 }
```

At a high level the shell will get a command from the command prompt, fork itself (`fork()` system call), exec the child into `echo` passing command line arguments (`exec()` system call) and will wait for the child to complete (`wait()` system call). The child will do a series of `write()` system calls to implement `printf` and then calls `exit()` system call. The trace will look something like: 

```
fork
exec
wait
write // one for each letter
write
write
...
exit
```

**Bonus points:** If you want to be more specific you can mention that reading the command will require multiple `read` system calls. Also, the shell might need to allocate memory with `malloc()` which might trigger an `sbrk()` system call. 

## Q8

Xv6 has the following file system layout:

![File system layout](../fs.png)

Block 1 contains the super block. Blocks 2 through 31 contain the log header and the log.

Blocks 32 through 57 contain inodes. Block 58 contains the bitmap of free blocks. Blocks 59 through the end of the disk contain data blocks.
Ben boots xv6 with a fresh fs.img and starts a program that opens a file which is represented by the inode 0 and writes one byte into it. The inode?s dentry[0] contains 76.

Which blocks will be written as the result of the write system call? Be specific, provide the trace of block writes and explain each write.

**Answer**

When writing to a file xv6 will create a logging (journalling transaction) which will first accumulate all written blocks in the logging area, will update the log header with the size of the transaction and details of the blocks, install the transaction into the data blocks and update the log header with 0 committing the transaction. 

Since the file already has block 0 alocated (`dentry[0]` is 76) and we're assuming that we're writing one byte into that allocated block no additional block allocations or inode modifications will be needed. Hence no updates to the inode or bitmap area. 

The disk access will look like this: 

```
write 3 // write data block into the logging area
write 2 // update log header
write 76 // write data block into its destination
write 2 // update log header with 0
```

