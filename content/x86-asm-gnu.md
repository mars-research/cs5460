---
title: "x86-64 Assembly Guide (GNU/AT&T syntax)"
description: "A 64-bit (x86-64) adaptation of a classic 32-bit x86 assembly guide."
---

**Contents:** [Registers](#registers) | [Memory and Addressing](#memory-and-addressing-modes) | [Instructions](#instructions) | [Calling Convention (System V AMD64)](#calling-convention-system-v-amd64)

This page is a **64-bit (x86-64)** adaptation of the classic **32-bit x86 Assembly Guide** (AT&T syntax / GNU `as`). It keeps the same teaching style and subset of instructions, but updates: register names and sizes, stack behavior, addressing, and the calling convention.

We assume the **GNU assembler (gas)** using the standard **AT&T syntax** on UNIX-like systems. Operand order is **source, destination**. Registers are prefixed with `%` and immediates with `$`.

---

## Registers

In 64-bit mode, x86-64 provides **sixteen** general purpose registers, each **64 bits** wide. Many have historical names (accumulator, counter, etc.), but today they are largely general-purpose. Two registers are still used by convention for the stack and stack frames: the stack pointer `%rsp` and (optionally) the base/frame pointer `%rbp`.

**General purpose registers (64-bit)**

|                |                                                       |
|----------------|-------------------------------------------------------|
| `%rax`         | accumulator / return value                            |
| `%rbx`         | callee-saved general register                         |
| `%rcx`         | counter / shift count uses `%cl`                      |
| `%rdx`         | used with mul/div; also arg register                  |
| `%rsi`         | arg register (often “source index” historically)      |
| `%rdi`         | arg register (often “destination index” historically) |
| `%rbp`         | frame pointer (optional)                              |
| `%rsp`         | stack pointer                                         |
| `%r8` … `%r15` | additional general registers                          |

Most registers also have smaller “views” (sub-registers) used for 32-bit, 16-bit, or 8-bit operations.

|            |            |            |               |                |
|------------|------------|------------|---------------|----------------|
| **64-bit** | **32-bit** | **16-bit** | **8-bit low** | **8-bit high** |
| `%rax`     | `%eax`     | `%ax`      | `%al`         | `%ah`          |
| `%rbx`     | `%ebx`     | `%bx`      | `%bl`         | `%bh`          |
| `%rcx`     | `%ecx`     | `%cx`      | `%cl`         | `%ch`          |
| `%rdx`     | `%edx`     | `%dx`      | `%dl`         | `%dh`          |
| `%rsi`     | `%esi`     | `%si`      | `%sil`        | (none)         |
| `%rdi`     | `%edi`     | `%di`      | `%dil`        | (none)         |
| `%rbp`     | `%ebp`     | `%bp`      | `%bpl`        | (none)         |
| `%rsp`     | `%esp`     | `%sp`      | `%spl`        | (none)         |
| `%r8`      | `%r8d`     | `%r8w`     | `%r8b`        | (none)         |
| `%r9`      | `%r9d`     | `%r9w`     | `%r9b`        | (none)         |
| `%r10`     | `%r10d`    | `%r10w`    | `%r10b`       | (none)         |
| `%r11`     | `%r11d`    | `%r11w`    | `%r11b`       | (none)         |
| `%r12`     | `%r12d`    | `%r12w`    | `%r12b`       | (none)         |
| `%r13`     | `%r13d`    | `%r13w`    | `%r13b`       | (none)         |
| `%r14`     | `%r14d`    | `%r14w`    | `%r14b`       | (none)         |
| `%r15`     | `%r15d`    | `%r15w`    | `%r15b`       | (none)         |

**Important x86-64 rule:** writing a 32-bit sub-register (e.g. `%eax`) **zero-extends** into the full 64-bit register (so writing `%eax` clears the upper 32 bits of `%rax`). This does *not* happen for 8-bit or 16-bit writes.

---

## Memory and Addressing Modes

### Declaring Static Data Regions

Static data regions (like global variables) are declared after the `.data` directive. In addition to `.byte` (1 byte), `.short` (2 bytes), and `.long` (4 bytes), x86-64 code often uses `.quad` (8 bytes).

Example declarations:

```gas
.data
var:
    .byte 64                 # 1 byte at var, value 64
    .byte 10                 # 1 byte at var + 1, value 10

x:
    .short 42                # 2 bytes at x, value 42

y:
    .long 30000              # 4 bytes at y, value 30000

z:
    .quad 0x1122334455667788 # 8 bytes at z
```

Arrays are contiguous memory cells. For 64-bit integer arrays, prefer `.quad`. For byte arrays, strings and `.zero` are still useful.

```gas
arr32:
    .long 1, 2, 3        # 3 x 4 bytes, so arr32 + 8 is 3
arr64:
    .quad 1, 2, 3        # 3 x 8 bytes, so arr64 + 16 is 3
barr:
    .zero 10             # 10 zero bytes at barr
str:
    .string "hello"      # bytes for hello followed by NUL
```

### Addressing Memory

In 64-bit mode, pointers and addresses are **64-bit** quantities (though current CPUs/OSes often use fewer than 64 bits of virtual address space). Labels are replaced by the assembler/linker with an address.

As in 32-bit x86, memory addresses can be computed using: **a base register** + **an index register** × **scale** + **displacement**. In AT&T syntax, this is written: `disp(base, index, scale)` where scale ∈ {1,2,4,8}.

**New in x86-64:** code commonly uses **RIP-relative** addressing for globals, written `symbol(%rip)`. This is the standard position-independent form.

Examples using `mov`:

```gas
movq (%rbx), %rax        # load 8 bytes from address in RBX into RAX
movq %rbx, var(%rip)     # store RBX into global variable var
movl -4(%rsi), %eax      # load 4 bytes from (RSI-4) into EAX (zero-extends into RAX)
movb %cl, (%rsi,%rax,1)  # store 1 byte (CL) to address RSI+RAX
movq (%rsi,%rbx,4), %rdx # load 8 bytes from address RSI + 4*RBX into RDX
```

Some invalid address calculations (same restrictions as 32-bit):

```gas
movq (%rbx,%rcx,-1), %rax      # invalid: scale must be 1,2,4, or 8 (not -1)
movq %rbx, (%rax,%rsi,%rdi,1)  # invalid: at most 2 registers in the address computation
```

### Operation Suffixes

The instruction suffix indicates operand size: `b`=1 byte, `w`=2 bytes, `l`=4 bytes, `q`=8 bytes.

Sometimes the operand size is ambiguous, e.g. `mov $2, (%rbx)`. In such cases, use an explicit suffix:

```gas
movb $2, (%rbx)  # store 1 byte
movw $2, (%rbx)  # store 2 bytes
movl $2, (%rbx)  # store 4 bytes
movq $2, (%rbx)  # store 8 bytes
```

---

## Instructions

Machine instructions fall into three broad categories: data movement, arithmetic/logic, and control-flow. This is not exhaustive; it is a useful subset.

Notation used below:

|           |                                                                           |
|-----------|---------------------------------------------------------------------------|
| `<reg64>` | Any 64-bit register (`%rax`, `%rbx`, …, `%r15`)                           |
| `<reg32>` | Any 32-bit register (`%eax`, `%ebx`, …)                                   |
| `<reg16>` | Any 16-bit register (`%ax`, `%bx`, …)                                     |
| `<reg8>`  | Any 8-bit register (`%al`, `%cl`, `%r8b`, …)                              |
| `<mem>`   | A memory operand (e.g. `(%rax)`, `8(%rbp)`, `var(%rip)`, `(%rax,%rbx,4)`) |
| `<con64>` | Any 64-bit immediate constant                                             |
| `<con>`   | Any immediate constant (size depends on instruction)                      |

Immediate operands use a dollar sign prefix: `$123`, `$0xABC`, etc.

### Data Movement Instructions

`mov` — Move

Copies data from the first operand to the second. Register-to-register is allowed; memory-to-memory is not (use a register as a temporary).

*Syntax*

```gas
mov{b,w,l,q} <reg>, <reg>
mov{b,w,l,q} <reg>, <mem>
mov{b,w,l,q} <mem>, <reg>
mov{b,w,l,q} <con>, <reg>
mov{b,w,l,q} <con>, <mem>
```

*Examples*

```gas
movq %rbx, %rax        # copy RBX into RAX
movb $5, var(%rip)     # store 5 into the byte at var
movl $0, %eax          # set EAX to 0 (also clears upper half of RAX)
```

`push` — Push on stack

Pushes an 8-byte value onto the stack: decrements `%rsp` by 8, then stores the value at `(%rsp)`.

*Syntax*

```gas
pushq <reg64>
pushq <mem>
pushq <con>
```

*Examples*

```gas
pushq %rax
pushq var(%rip)
```

`pop` — Pop from stack

Pops an 8-byte value from the stack: loads from `(%rsp)`, then increments `%rsp` by 8.

*Syntax*

```gas
popq <reg64>
popq <mem>
```

*Examples*

```gas
popq %rdi
popq (%rbx)
```

`lea` — Load effective address

Computes an address and places it in a register (does not load memory contents). Often used for pointer arithmetic and for RIP-relative addresses.

*Syntax*

```gas
leaq <mem>, <reg64>
```

*Examples*

```gas
leaq (%rbx,%rsi,8), %rdi   # RDI = RBX + 8*RSI
leaq var(%rip), %rax       # RAX = &var
```

### Arithmetic and Logic Instructions

`add` — Integer addition

Adds the two operands, storing the result in the second operand. At most one operand may be memory.

*Examples*

```gas
addq $10, %rax       # RAX = RAX + 10
addb $10, (%rax)     # add 10 to the byte at address RAX
```

`sub` — Integer subtraction

Subtracts first operand from the second operand, storing the result in the second operand.

*Examples*

```gas
subq $216, %rax
sub  %ah, %al        # still valid for 8-bit sub-registers
```

`inc, dec` — Increment / Decrement

Increment or decrement by one.

*Examples*

```gas
decq %rax
incl var(%rip)       # add one to a 32-bit integer at var
```

`imul` — Integer multiplication

The two-operand form multiplies its operands and stores the result in the second operand (a register). A three-operand form exists with an immediate multiplier.

*Syntax*

```gas
imulq <reg64>, <reg64>
imulq <mem>, <reg64>
imulq <con>, <reg64>, <reg64>
```

*Examples*

```gas
imulq (%rbx), %rax       # RAX *= *(qword*)RBX
imulq $25, %rdi, %rsi    # RSI = RDI * 25
```

`idiv` — Signed integer division

Divides the signed 128-bit integer in `%rdx:%rax` (high:low) by the operand. Quotient is stored in `%rax`, remainder in `%rdx`.

Typically you prepare `%rdx:%rax` using `cqto` (sign-extend RAX into RDX).

*Example*

```gas
cqto
idivq %rbx      # (RDX:RAX) / RBX  -> quotient in RAX, remainder in RDX
```

`and, or, xor` — Bitwise logical operations

Perform the operation and store the result in the second operand (AT&T order still source, destination).

*Examples*

```gas
andq $0x0f, %rax     # clear all but the last 4 bits
xorq %rdx, %rdx      # set RDX to zero
```

`not` — Bitwise NOT

*Example*

```gas
notq %rax
```

`neg` — Two's complement negation

*Example*

```gas
negq %rax
```

`shl, shr, sar` — Shifts

Shift count is an 8-bit immediate or `%cl`. For 64-bit operands, shift counts are effectively taken modulo 64, and the operand can be shifted up to 63 places.

*Examples*

```gas
shlq $1, %rax     # RAX *= 2 (if no overflow concern)
shrq %cl, %rbx    # RBX = floor(RBX / 2^CL) for unsigned values
sarq %cl, %rbx    # arithmetic right shift (sign-propagating)
```

### Control Flow Instructions

The processor maintains an instruction pointer `%rip`, a 64-bit value pointing to the current instruction. It cannot be written directly, but is changed by control-flow instructions.

`jmp` — Jump

Unconditional jump to a label or indirect target.

*Examples*

```gas
jmp begin
jmp *%rax      # indirect jump to address in RAX
```

`j<condition>` — Conditional jump

Conditional branches based on flags set by a previous instruction (often `cmp`). Common conditions: `je`, `jne`, `jg`, `jge`, `jl`, `jle`.

```gas
cmpq %rbx, %rax
jle done
```

`cmp` — Compare

Like subtraction for flags, but discards the result.

*Example*

```gas
cmpb $10, (%rbx)
je loop
```

`call, ret` — Call and return

`call` pushes an 8-byte return address (next `%rip`) onto the stack and jumps to the target. `ret` pops that address and jumps back.

---

## Calling Convention (System V AMD64)

In 32-bit x86, a common “C calling convention” passes parameters on the stack. In **64-bit UNIX-like systems**, the standard is the **System V AMD64 ABI**, which passes the first arguments in registers. (Windows uses a different convention; see note at the end.)

### Argument passing

The first six integer/pointer arguments are passed in registers:

|        |        |
|--------|--------|
| `arg1` | `%rdi` |
| `arg2` | `%rsi` |
| `arg3` | `%rdx` |
| `arg4` | `%rcx` |
| `arg5` | `%r8`  |
| `arg6` | `%r9`  |

Additional arguments (7 and beyond) are passed on the stack. Integer/pointer return values are placed in `%rax`.

### Caller-saved vs callee-saved

Registers are divided into those the caller must assume can be clobbered (caller-saved), and those a callee must preserve if it uses them (callee-saved). A common summary:

|                  |                                              |
|------------------|----------------------------------------------|
| **Callee-saved** | `%rbx %rbp %r12 %r13 %r14 %r15`              |
| **Caller-saved** | `%rax %rcx %rdx %rsi %rdi %r8 %r9 %r10 %r11` |

### Stack alignment

Before executing a `call`, the stack pointer `%rsp` must be aligned to a **16-byte boundary**. Because `call` pushes an 8-byte return address, a typical callee entry sees `%rsp` misaligned by 8 and fixes alignment in its prologue as needed.

### Example: making a call (caller side)

Call `myFunc(p1, 216, *p3)` where: `p1` is in `%rax`, and `%rbx` holds a pointer to the third argument value.

```gas
movq %rax, %rdi      # arg1 = p1
movq $216, %rsi      # arg2 = 216
movq (%rbx), %rdx    # arg3 = *p3
# ensure 16-byte stack alignment here if needed
call myFunc          # return value in %rax
```

### Example: function definition (callee side)

A simple function that returns `arg1 + (arg2 + arg3)`. This version uses a frame pointer (like the 32-bit guide) for clarity.

```gas
.text
.globl myFunc
.type myFunc, @function
myFunc:
    # Prologue
    pushq %rbp
    movq  %rsp, %rbp
    subq  $16, %rsp        # space for locals, keeps stack aligned

    # Body (args in %rdi, %rsi, %rdx)
    movq  %rdx, -8(%rbp)   # local = arg3
    addq  %rsi, -8(%rbp)   # local += arg2
    movq  %rdi, %rax       # rax = arg1
    addq  -8(%rbp), %rax   # rax += local

    # Epilogue
    leave
    ret
```

### Windows note

If you are targeting **Windows x64**, the integer argument registers are `RCX, RDX, R8, R9`, and the caller must reserve 32 bytes of “shadow space” on the stack. The rest of this section assumes SysV AMD64.

---

**Credits:** Based on the structure of the classic x86 Assembly Guide (Ferrari/Batson/Lack/Jones/Evans), and later AT&T-syntax revisions. This page is a teaching-focused x86-64 adaptation.
