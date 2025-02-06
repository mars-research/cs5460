---
title: Home
---

# CS 5460/6460 - Operating Systems

---

- Instructor: [Anton Burtsev](https://users.cs.utah.edu/~aburtsev/) (anton.burtsev@utah.edu)
- Time and place: Tue/Thu, 9:10am - 10:30am, [WEB L101](https://map.utah.edu/index.html?code=WEB)
- Canvas: [https://utah.instructure.com/courses/1022264](https://utah.instructure.com/courses/1022264)
- Piazza (questions): [https://piazza.com/utah/spring2025/cs5460001spring2025/home](https://piazza.com/utah/spring2025/cs5460001spring2025/home)
- Gradescope (homework assignments and quizzes): [https://www.gradescope.com/courses/947893](https://www.gradescope.com/courses/947893)
- Office hours: 
  - Anton: Thursdays 11:00am-12:00pm [Anton's office, MEB 3424](https://map.utah.edu/index.html?code=MEB)
  - Soham: Tuesdays 2.00pm-3.00pm [MEB 3145](https://map.utah.edu/index.html?code=MEB)
  - Rutuja: Wednesdays 5.00pm-6.00pm [MEB 3145](https://map.utah.edu/index.html?code=MEB)
  - Navya: Mondays 12.00pm-1.00pm [MEB 3515](https://map.utah.edu/index.html?code=MEB) 
- Class repo (pull requests): [github](https://github.com/mars-research/cs5460)
- PollEverywhere link: [PollEv.com/antonburtsev](http://PollEv.com/antonburtsev)
---


# Class Overview

cs5460/6460 teaches the fundamentals of operating systems. You will study, in
detail, virtual memory, kernel and user mode, system calls, threads, context
switches, interrupts, interprocess communication, coordination of concurrent
activities, and the interface between software and hardware. Most importantly,
you will study the interactions between these concepts, and how to manage the
complexity introduced by the interactions.

To master the concepts, cs5460/6460 is organized in as a series of lectures,
and homeworks. The lectures (and the book readings) familiarize you with the
main concepts. The homrworks force you to understand the concepts at a deep
level.

The lectures are based on [xv6](https://pdos.csail.mit.edu/6.828/2016/xv6.html),
a modern re-implementation of one of the early UNIX operating systems, 
specifically, Unix Version 6 which was developed in the 1970s, on the modern hardware. 
xv6 is only 9,000 lines of C code, but it can run real processes, and perform many 
functions of a traditional operating system, e.g., Windows, Linux, and Mac OS. 
Due to its small size, it is possible to read the source code and understand the entire 
operating system. Moreover, xv6 is accompanied by a [book](https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf)
describing its architecture and a [printout of its source code](https://pdos.csail.mit.edu/6.828/2018/xv6/xv6-rev11.pdf).  
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

**Jan 7**  
- [Lecture 0 - Logistics](./lectures/lecture00-logistics/lecture00-logistics.pdf) ([video](https://youtube.com/live/lLwee3ElfyA))  
- [Lecture 1 - Introduction](./lectures/lecture01-intro/lecture01-intro.pdf) ([video](https://youtube.com/live/lLwee3ElfyA))  
- Reading: [OSTEP: 2 Introduction](http://pages.cs.wisc.edu/~remzi/OSTEP/intro.pdf)  
- Reading (optional, to help you with C): [The C Programming Language](https://archive.org/details/TheCProgrammingLanguageFirstEdition) (look at Chapter 5 Pointers and Arrays) by Brian Kernighan and Dennis Ritchie (first edition is freely available, second edition is in print and is available from all major bookstores)
		

**Jan 9**  
- [Lecture 02 - OS Interfaces (part 1)](./lectures/lecture02-os-interface/lecture02-os-interface.pdf) ([video](https://youtube.com/live/0cZx8caiIS8))
- Reading: [xv6: Chapter 0: Operating system interfaces](https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf)
- Video (optional): [AT&T Archives: The UNIX Operating System](https://www.youtube.com/watch?v=tc4ROCJYbm0)

**Jan 14**  
- [Lecture 02 - OS Interfaces (part 2)](./lectures/lecture02-os-interface/lecture02-os-interface.pdf) ([video](https://youtube.com/live/UzqupNygAZ4))
- Reading: [OSTEP: Chapter 5: Interlude: Process API](http://pages.cs.wisc.edu/~remzi/OSTEP/cpu-api.pdf)

**Jan 16**  
- [Lecture 02 - OS Interfaces (part 3)](./lectures/lecture02-os-interface/lecture02-os-interface.pdf)
- [Lecture 03 - x86 Assembly](./lectures/lecture03-x86-asm/lecture03-x86-asm.pdf) ([video](https://youtube.com/live/X7ivo7TT0D8))
- Reading: [Reading: xv6 Book: Appendix A: PC Hardware](https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf)
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
- [Lecture 06 - Segmentation and Paging (part 2)](./lectures/lecture06-address-translation/lecture06-address-translation.pdf) ([video]())
- Reading: [OSTEP: Chapter 18 Paging: introduction: only Sections 18.1 - 18.3](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-paging.pdf)
- Reading: [OSTEP: Chapter 20: only Section 20.3 Multi-level Page Tables](http://pages.cs.wisc.edu/~remzi/OSTEP/vm-smalltables.pdf)
- Reading: [Intel 64 and IA-32 architectures software developer's manual volume 3A: System programming guide, part 1 (Chapter 3: 3.1-3.4.5 and Chapter 4: 4.2-4.3)](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-3a-part-1-manual.pdf)



**Feb 4** 
- [**Homework 2 - ELF**](./homework/hw2-elf/)

**March 6**
- Midterm exam (regular time and place)


**March 11**
- No class (Spring Break)

**March 13**
- No class (Spring Break)

**April 29**
- Final exam 8:00am - 10:00am (same room) [official schedule](https://registrar.utah.edu/academic-calendars/final-exams-spring.php)



