---
title: Home
---

# CS 5460/6460 - Operating Systems

---

- Instructor: [Anton Burtsev](https://users.cs.utah.edu/~aburtsev/) (anton.burtsev@utah.edu)
- Time and place: Tue/Thu, 9:10am - 10:30am, [WEB L101](https://map.utah.edu/index.html?code=WEB)
- Canvas: <https://utah.instructure.com/courses/1221030>
- Piazza (questions): <https://piazza.com/utah/spring2026/cs6460001spring2026/home>
- Piazza sign up link and code are on Canvas
- Gradescope (homework assignments and quizzes): <https://www.gradescope.com/courses/1213502>
- Office hours: 
  - Anton: Tuesdays 11:00am-12:00pm in [Anton's office, MEB 3424](https://map.utah.edu/index.html?code=MEB)
  - Thalia: Thursdays 11:00am-12:00pm or by appointment in MEB 2180
  - Hanwen - TBD
  - Manvik - Thursdays 4:00pm-5:00pm or by appointment in [MEB 3375](https://map.utah.edu/index.html?code=MEB)
- Class repo (pull requests): [github](https://github.com/mars-research/cs5460)
- Poll Everywhere: [PollEv.com/antonburtsev](https://pollev.com/antonburtsev)
- Previous years: [2025](./2025)

---


# Class Overview

cs5460/6460 teaches the fundamentals of operating systems. You will study, in
detail, virtual memory, kernel and user mode, system calls, threads, context
switches, interrupts, interprocess communication, coordination of concurrent
activities, and the interface between software and hardware. Most importantly,
you will study the interactions between these concepts, and how to manage the
complexity introduced by the interactions.

To master the concepts, cs5460/6460 is organized as a series of lectures,
and homeworks. The lectures (and the book readings) familiarize you with the
main concepts. The homeworks force you to understand the concepts at a deep
level.

The lectures are based on [xv6](https://xv6-64.gitlab.io/xv6-64/),
a modern re-implementation of one of the early UNIX operating systems, 
specifically, Unix Version 6 which was developed in the 1970s, on the modern hardware. 
xv6 is only 9,000 lines of C code, but it can run real processes, and perform many 
functions of a traditional operating system, e.g., Windows, Linux, and Mac OS. 
Due to its small size, it is possible to read the source code and understand the entire 
operating system. Moreover, xv6 is accompanied by a [book](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf)
describing its architecture and a [printout of its source code](https://xv6-64.gitlab.io/xv6-64/xv6-64.pdf).
**Note, we are using x86 64 bit version of xv6 in this class, not x86 32bit version and not RISC-V**.  
Homework assignments will help you to deepen the
understanding of the principles and internal organization of a simple, but real
operating system.

You may wonder why we are studying an operating system that resembles Unix
v6 instead of the latest and greatest version of Linux, Windows, or BSD Unix.
xv6 is big enough to illustrate the basic design and implementation ideas in
operating systems. On the other hand, xv6 is far smaller than any modern
production operating systems, and correspondingly easier to understand. xv6 has
a structure similar to many modern operating systems; once you've explored xv6
you will find that much is familiar inside kernels such as Linux.

### Grading policy

Homework: 40%, in-class activities: 10%, quizzes 15%, midterm exam: 15%, final exam: 20% of your grade (all grades curved). 

### Late homework policy
You can submit late homework assignments (not quizzes or in-class activities) 3 days after the deadline for 60% of your grade.

# Schedule

**Jan 6**  
- [Lecture 0 - Logistics](./lectures/lecture00-logistics/lecture00-logistics.pdf) ([video](https://youtube.com/live/NIewEe9dBgs))  
- [Lecture 1 - Introduction](./lectures/lecture01-intro/lecture01-intro.pdf) ([video](https://youtube.com/live/NIewEe9dBgs))  
- Reading: [OSTEP: 2 Introduction](http://pages.cs.wisc.edu/~remzi/OSTEP/intro.pdf)  
- Reading (optional, to help you with C): [The C Programming Language](https://ia903407.us.archive.org/35/items/the-ansi-c-programming-language-by-brian-w.-kernighan-dennis-m.-ritchie.org/The%20ANSI%20C%20Programming%20Language%20by%20Brian%20W.%20Kernighan%2C%20Dennis%20M.%20Ritchie.pdf) (look at Chapter 5 Pointers and Arrays) by Brian Kernighan and Dennis Ritchie
		

**Jan 8**  
- [Lecture 02 - OS Interfaces (part 1)](./lectures/lecture02-os-interface/lecture02-os-interface.pdf) ([video](https://youtube.com/live/5F-f0opqzQ0))
- Reading: [xv6: Chapter 0: Operating system interfaces](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf)
- Video (optional): [AT&T Archives: The UNIX Operating System](https://www.youtube.com/watch?v=tc4ROCJYbm0)

<!--
**Jan 14**  
- [Lecture 02 - OS Interfaces (part 2)](./lectures/lecture02-os-interface/lecture02-os-interface.pdf) ([video](https://youtube.com/live/UzqupNygAZ4))
- Reading: [OSTEP: Chapter 5: Interlude: Process API](http://pages.cs.wisc.edu/~remzi/OSTEP/cpu-api.pdf)

**Jan 16**  
- [Lecture 02 - OS Interfaces (part 3)](./lectures/lecture02-os-interface/lecture02-os-interface.pdf)
- [Lecture 03 - x86 Assembly](./lectures/lecture03-x86-asm/lecture03-x86-asm.pdf) ([video](https://youtube.com/live/X7ivo7TT0D8))
- Reading: [Reading: xv6 Book: Appendix A: PC Hardware](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf)
- Reading: [Reading: x86 Assembly Guide](http://www.cs.virginia.edu/~evans/cs216/guides/x86.html)

**Jan 21**  
- [Lecture 03 - x86 Assembly (part 2)](./lectures/lecture03-x86-asm/lecture03-x86-asm.pdf) ([video](https://youtube.com/live/cIioOwbGERo))
- [Lecture 04 - Calling Conventions](./lectures/lecture04-calling-conventions/lecture04-calling-conventions.pdf) ([video](https://youtube.com/live/cIioOwbGERo))
- Reading: [PC Assembly Language. Paul A. Carter: Section 4 Subprograms (4.1 - 4.5)](https://pdos.csail.mit.edu/6.828/2014/readings/pcasm-book.pdf)
- Reading: [Wikipedia: x86 calling conventions](https://en.wikipedia.org/wiki/X86_calling_conventions)
- [**Homework 1 - Shell**](./homework/hw1-shell/)

**Jan 23**  
- [Lecture 04 - Calling Conventions](./lectures/lecture04-calling-conventions/lecture04-calling-conventions.pdf) ([video](https://youtube.com/live/fqmWbjVrcAg))
- Source code examples: [github](https://github.com/mars-research/cs5460/tree/main/static/lectures/lecture03-x86-asm/src)

**Jan 28**  
- [Lecture 05 - Linking and Loading](./lectures/lecture05-linking-and-loading/lecture05-linking-and-loading.pdf) ([video](https://youtu.be/SkyJH1wTPjc)
- Reading: [Operating Systems from 0 to 1. Chapter 5. The Anatomy of a Program](https://github.com/tuhdo/os01/blob/master/Operating_Systems_From_0_to_1.pdf)
- Optional reading: This lecture is mostly based on the material of this book: [Linkers and Loaders by John R. Levine](https://books.google.com/books/about/Linkers_and_Loaders.html?id=Id9cYsIdjIwC)

**Jan 30** 
- [Lecture 05 - Linking and Loading (part 2)](./lectures/lecture05-linking-and-loading/lecture05-linking-and-loading.pdf) ([video](https://youtube.com/live/uCQcn2VlGgs))

**Feb 4** 
- [Lecture 06 - Segmentation and Paging](./lectures/lecture06-address-translation/lecture06-address-translation.pdf) ([video](https://youtube.com/live/hwulcVvWuMw))
- Reading: [Intel 64 and IA-32 architectures software developer's manual volume 3A: System programming guide, part 1 (Chapter 3: 3.1-3.4.5 and Chapter 4: 4.2-4.3)](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-3a-part-1-manual.pdf)


**Feb 6** 
- [Lecture 06 - Segmentation and Paging (part 2)](./lectures/lecture06-address-translation/lecture06-address-translation.pdf) ([video](https://youtube.com/live/QADhnUCLGTI))
- Reading: [OSTEP: Chapter 18 Paging: introduction: only Sections 18.1 - 18.3](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-paging.pdf)
- Reading: [OSTEP: Chapter 20: only Section 20.3 Multi-level Page Tables](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-smalltables.pdf)
- Reading: [Intel 64 and IA-32 architectures software developer's manual volume 3A: System programming guide, part 1 (Chapter 3: 3.1-3.4.5 and Chapter 4: 4.2-4.3)](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-3a-part-1-manual.pdf)
- [**Homework 2 - ELF**](./homework/hw2-elf/)

**Feb 11** 
- [Lecture 06 - Segmentation and Paging (part 3)](./lectures/lecture06-address-translation/lecture06-address-translation.pdf) ([video](https://youtube.com/live/q6-70qitj5I))

**Feb 13**
- [Lecture 07 - System Boot](./lectures/lecture07-system-boot/lecture07-system-boot.pdf) ([video](https://youtube.com/live/tOOFmZLfKTw))
- Reading: [Appendix B: The boot loader](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf) and [Chapter 1: Code: the first address space](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf).
- Reading (optional): [How Does an Intel Processor Boot](https://binarydebt.wordpress.com/2018/10/06/how-does-an-x86-processor-boot/)?
- Reading (optional): [Intel SGX explained](https://eprint.iacr.org/2016/086.pdf) (sections 2.9.1 The Motherboard, 2.9.2 The Intel Management Engine (ME), and 2.13 Platform Initialization (Booting))

**Feb 18**
- [Lecture 07 - System Boot](./lectures/lecture07-system-boot/lecture07-system-boot.pdf) ([video](https://youtube.com/live/IY4ij-9B_7c))

**Feb 20**
-[Lecture 08 - System Init](./lectures/lecture08-kernel-page-table/lecture08-kernel-page-table.pdf) ([video](https://youtube.com/live/t7UnnRWz4k4))
- Reading: [Chapter 1: Operating system organization](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf)
- Reading: [Chapter 2: Page tables](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf)

**Feb 25**
- [Lecture 08 - System Init (part2)](./lectures/lecture08-kernel-page-table/lecture08-kernel-page-table.pdf) ([video](https://youtube.com/live/ZIkLnn0QcbU))
- [**Homework 3 - Boot Into C**](./homework/hw3-boot-into-c/)

**Feb 27**
- [Lecture 09 - Interrupts and Exceptions](./lectures/lecture09-interrupts/lecture09-interrupts.pdf) ([video](https://youtube.com/live/89fLiN1xc8o))
- Reading: [Chapter 3: Traps, interrupts and drivers](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf)


**March 4**
- [Midterm review](./lectures/lecture-midterm-recap/lecture-midterm-recap.pdf) ([video](https://youtube.com/live/aj5OOUeraTc)
- Reading: [Exams from previous years](https://users.cs.utah.edu/~aburtsev/5460/exams.html)

-->

**March 5**
- **Midterm exam** (regular time and place)
- Exam rules: _paper books and printouts are allowed, no devices_. 

<!--
**March 11**
- No class (Spring Break)

**March 13**
- No class (Spring Break)

**March 18**
- [Lecture 09 - Interrupts and Exceptions (part 2)](./lectures/lecture09-interrupts/lecture09-interrupts.pdf) ([video](https://youtube.com/live/Gf84CFB3uWw))
- Reading: [Chapter 3: Traps, interrupts and drivers](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf)

**March 20**
- [Lecture 09 - Interrupts and Exceptions (part 3)](./lectures/lecture09-interrupts/lecture09-interrupts.pdf) ([video](https://youtube.com/live/5_gOW3waRWA))
- Reading: [Chapter 3: Traps, interrupts and drivers](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf)

**March 25**
- [Lecture 10 - Creating Processes](./lectures/lecture10-creating-processes/lecture10-creating-processes.pptx) ([video](https://youtube.com/live/V9jk3Shm838))
- Reading: [Chapter 2: Page tables](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf)

**March 27**
- [Lecture 10 - Creatting Processes (part 2)](./lectures/lecture10-creating-processes/lecture10-creating-processes.pptx) ([video](https://youtube.com/live/P4ZZASCxA00))
- Reading: [Chapter 2: Page tables](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf)

**March 30**
- [**Homework 4 - System Calls**](./homework/hw4-system-call)

**April 1**
- [Lecture 11 - Context Switch](./lectures/lecture11-context-switch/lecture11-context-switch.pdf) ([video](https://youtube.com/live/dd0dgK28Wno))
- Reading: [Chapter 1. Subsection: Running the first process, Chapter 5 - Scheduling (up until Drivers)](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf)

**April 3**
- [Lecture 11 - Context Switch (part 2)](./lectures/lecture11-context-switch/lecture11-context-switch.pdf) ([video](https://youtube.com/live/jBUCYKkcoWI))
- Reading: [Chapter 1. Subsection: Running the first process, Chapter 5 - Scheduling (up until Drivers)](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf)

**April 8**
- [Lecture 12 - File Systems](./lectures/lecture12-file-system/lecture12-file-system.pdf) ([video](https://youtube.com/live/rrMSQk0PFis))
- Reading: [xv6: Chapter 6: File System](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf)
- Reading: [OSTEP: Chapter 40: File System Implementation](https://pages.cs.wisc.edu/~remzi/OSTEP/file-implementation.pdf)

**April 10**
- [Lecture 12 - File Systems (part 2)](./lectures/lecture12-file-system/lecture12-file-system.pdf) ([video](https://youtube.com/live/byOHgi16xMI))
- Reading: [xv6: Chapter 6: File System](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf)
- Reading: [OSTEP: Chapter 40: File System Implementation](https://pages.cs.wisc.edu/~remzi/OSTEP/file-implementation.pdf)


**April 15**
- [Lecture 13 - Synchronization](./lectures/lecture13-synchronization/lecture13-synchronization.pdf) ([video](https://youtube.com/live/YcgkfdFWIGQ))
- Reading: [xv6: Chapter 4: Locking and synchronization](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf)

**April 17**
- [Lecture 13 - Synchronization (part 2)](./lectures/lecture13-synchronization/lecture13-synchronization.pdf) ([video](https://youtube.com/live/-cabHMty2OU))
- Reading: [xv6: Chapter 4: Locking and synchronization](https://xv6-64.gitlab.io/xv6-64-book/xv6-64-book.pdf)
- [**Homework 5 (extra credit) - Infinite files for xv6**](./homework/hw5-infinite-files)
  
**April 22**
- [Final review](./lectures/lecture-final-recap/lecture-final-recap.pdf) ([video](https://youtube.com/live/guIvwQlmknw)
- Reading: [Exams from previous years](https://users.cs.utah.edu/~aburtsev/5460/exams.html)

-->

**April 29**
- **Final exam** 8:00am - 10:00am (same room) [official schedule](https://registrar.utah.edu/academic-calendars/final-exams-spring.php)
<!--- [Exam and solutions](./exams/final)-->
- Reading: [Exams from previous years](https://users.cs.utah.edu/~aburtsev/5460/exams.html)


[**Homework 1 - Shell**](./homework/hw1-shell/)