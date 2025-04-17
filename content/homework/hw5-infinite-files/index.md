# Homework 5: Infinite files for xv6
To get started, take a fresh pull of the folder you cloned for homework 4 and download the necessary files attached with this instructions page.

In this assignment you'll increase the maximum size of an xv6 file. Currently xv6 files are limited to 140 sectors, or 71,680 bytes. This limit comes from the fact that an xv6 inode contains 12 "direct" block numbers and one "singly-indirect" block number, which refers to a block that holds up to 128 more block numbers, for a total of 12+128=140.

In this homework you will change the xv6 file system code to support a "linked-list" file addressing for files of infinite length (of course, in practice the file size will be limited by the size of your file system).

---

## Preliminaries

Modify your Makefile’s `CPUS` definition so that it reads:

```makefile
CPUS := 1
```

This speeds up QEMU when xv6 creates large files.

Since `mkfs` initializes the file system to have fewer than 1000 free data blocks, too few to show off the changes you'll make, modify `FSSIZE` inside `param.h` to:

```c
#define FSSIZE       128*128*16
```

Also, in `fs.h`, change `MAXFILE` to:

```c
#define MAXFILE 128*2048
```

While we will support files of infinite length, in practice xv6 too much depends on the `MAXFILE` constant. So to make the build system happy, lets change it to some large value.


Download [big.c](./big.c) into your xv6 directory, add it to the `UPROGS` list, start up xv6, and run `big`. It writes files of 1MB, 2MB, 4MB, and 8MB, then reads them --- this will help you to test your solution. Big writes 64KB at a time, which is significantly faster (and yet it takes quite a bit of time). Feel free to edit `big.c` to adjust write size, and enable additional debug info.

---

## Your Job

Modify bmap() so that it implements a linked-list system. The last element of the ip->addrs[] (ip->addrs[12]) should point to the head of the list (an indirect block that contains 127 pointers to the data blocks, and one (the first 4 bytes of the 512 block) pointer pointing to the next indirect block on the list as in the figure below). The first element of the last list should have the "next" pointer set to 0 (we know that 0 block contains the boot loader, so it's ok, to use it as a NULL). An ascii example illustrates the layout of the linked list:

```
        struct inode
     _________________      
     |               |
     |     type      |
     -----------------
     |               |
     |     major     |
     -----------------
     |     ....      |
     -----------------
     |     size      |
     -----------------
     |     addrs[0]  |
     -----------------
     |     addrs[1]  |
     -----------------
     |     ....      |
     -----------------
     |    addrs[11]  |
     -----------------
     |    addrs[12]  |  ->     -----------------
     -----------------    / <- |     tbl1[0]   |  
                          |    -----------------
                          |    |     .....     | -> Physical
                          |    -----------------  
                          |    |     .....     | -> Physical 
                          |    -----------------    
                          |    |     .....     |  
                          |    -----------------
                          |    |    tbl1[127]  |  -> Physical
                          |     -----------------   
                          |  
                          |
                          \->  -----------------
                          ---  |     tbl1[0]   |   
                         |     -----------------
                         |     |     .....     | -> Physical
                         |     -----------------  
                         |     |     .....     | -> Physical 
                         |     -----------------    
                         |     |     .....     |  
                         |     -----------------
                         |     |    tbl1[127]  | -> Physical
                         |     -----------------    
                         | 

                     .... more list elements

                          \-> -----------------
                              |     tbl1[0]   | -> 0 (NULL) // last element has "next" pointer as null
                              -----------------
                              |     .....     | -> Physical
                              -----------------  
                              |     .....     | -> Physical
                              -----------------    
                              |     .....     |  
                              -----------------
                              |    tbl1[127]  | -> Physical
                              ----------------- 

```

Since you modified `bmap()` you need a new version of `mkfs.c` that creates the file system with the same layout. This c file gets run during the make process and builds the file system image which gets run with QEMU. We provide one for you. Download [mkfs.c](./mkfs.c) and add it to your xv6 source tree. You need to delete existing file system 

```
make clean
```

Inside `mkfs.c` all the changes are inside the `iappend()` function.  The code and logic will be extremely similar to the code you had written in bmap().
If you have a hard time understanding what iappend() is doing, here is the annotated code

```
void
iappend(uint inum, void *xp, int n)
{
  char *p = (char*)xp;
  uint fbn, off, n1;
  struct dinode din;
  char buf[BSIZE];
  uint indirect[NINDIRECT];
  uint x;
  uint sec, bn;

  rinode(inum, &din);
  off = xint(din.size);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  while(n > 0){
    fbn = off / BSIZE;
    assert(fbn < MAXFILE);
    if(fbn < NDIRECT){
      if(xint(din.addrs[fbn]) == 0){
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } else {
      if(xint(din.addrs[NDIRECT]) == 0){
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (char*)indirect);
      indirect[0] = 0;
      wsect(xint(din.addrs[NDIRECT]), (char*)indirect);
     
      sec = din.addrs[NDIRECT]; 

      // shift back by direct blocks, skip 0
      bn = fbn + 1 - NDIRECT;
      
      do{
        
        printf("fbn:%d, sec:%d\n", bn, xint(sec)); 
        
        if(bn < NINDIRECT) {
          // 1st iter: if it fits within the first indirect table, we're done
          // nth iter: if it fits within the nth indirect table, we're done
          
          if(indirect[bn] == 0){
            indirect[bn] = xint(freeblock++);
            wsect(xint(sec), (char*)indirect);
          }
          x = xint(indirect[bn]);
          break;     
          
        } else {
          // 1st iter: if it does not fit within the first indirect table
          // nth iter: if it does not fit within the nth indirect table
          if(indirect[0] == 0) {
            // allocate the next indirect table
            indirect[0] = xint(freeblock++);
            wsect(xint(sec), (char*)indirect);
          }
          sec = indirect[0];
          rsect(xint(sec), (char*)indirect);
          indirect[0] = 0;
          // set new indirect table pointer to 0
          wsect(xint(sec), (char*)indirect);
          
          bn -= 127; // shift back by 127 so we have 1th index into new indirect  table and then loop back
            
        }  
          
      } while (1);
    }
    n1 = min(n, (fbn + 1) * BSIZE - off);
    rsect(x, buf);
    bcopy(p, buf + off - (fbn * BSIZE), n1);
    wsect(x, buf);
    n -= n1;
    off += n1;
    p += n1;
  }
  din.size = xint(off);
  winode(inum, &din);
}

```
You don't have to modify xv6 to handle deletion of files with linked-list blocks.

## What to Look At

The format of an on-disk inode is defined by struct dinode in `fs.h`. `NDIRECT` is set to 12 to account for the 12 direct pointers in the inode. 

The code that finds a file's data on disk is in bmap() in `fs.c`. Have a look at it and make sure you understand what it's doing. `bmap()` is called both when reading and writing a file. When writing, `bmap()` allocates new blocks as needed to hold file content, as well as allocating an indirect block if needed to hold block addresses.

`bmap()` deals with two kinds of block numbers. The `bn` argument is a "logical block" -- a block number relative to the start of the file. The block numbers in `ip->addrs[]`, and the argument to `bread()`, are disk block numbers. You can view `bmap()` as mapping a file's logical block numbers into disk block numbers.



## Hints
Make sure you understand `bmap()`. Write out a diagram of the relationships between `ip->addrs[]`, the direct blocks, the list blocks. Make sure you understand why adding linked list addressing block allows you to have files of infinite size.

If your file system gets into a bad state, perhaps by crashing, you may need to delete `fs.img` (do this from Unix, not xv6).

Don't forget to `brelse()` each block that you `bread()`.

You should allocate linked list blocks as needed, like the original `bmap()`.

Our solution was done in around 50 lines of code.

## Extra Credit

#### Extra Credit 1 (10%) 

Writing a large file to disk in xv6 is incredibly slow. It gets slower the fuller the disk gets(hint). There are two ways to speed up writing a file to disk. In extra_credit_1.txt explain why when the disk gets fuller writing gets slower-- Explain how you can mitigate this problem. Second, explain the other way to speed up writes and why that works (hint `log.c`).

#### Extra Credit 2 (20%)  

Implement these changes, in xv6, resolving all panics and failed assertions that arise. Do a crappy timing experiment using a stop watch. In extra_credit_2.txt explain how much speed up occured.


## Submit

Submit your solution as a zipped folder on [Gradescope](https://www.gradescope.com/courses/947893/assignments/6078947). You can use the following command to create a compressed zip file.
```
CADE$ cd xv6-cs5460
CADE$ make clean
CADE$ zip -r ../hw5.zip .
```

Or, if you are using the original xv6 repository (not recommended):

```
CADE$ cd xv6-public
CADE$ make clean
CADE$ zip -r ../hw5.zip .
```

Following is a guideline on how the file structure should be:

```
/
  - mkfs.c
  - big.c
  - fs.h
  - fs.c
  - ...                             -- any other files required to start
  - /extra1                         -- optional
    - extra_credit_1.txt
  - /extra2                         -- optional
    - extra_credit_2.txt
    - mkfs.c
    - big.c
    - fs.h
    - fs.c
    - ...                             -- any other files required to start 
```