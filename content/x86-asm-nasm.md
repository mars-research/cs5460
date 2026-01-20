---
title: "x86-64 Assembly Guide (AMD64 / Intel 64, Intel (NASM) syntax)"
description: "A 64-bit (x86-64) adaptation of a classic 32-bit x86 assembly guide, written in NASM-native Intel syntax."
---

**Contents:** [Registers](#registers) | [Memory and Addressing](#memory-and-addressing-modes) | [Instructions](#instructions) | [Calling Convention (System V AMD64)](#calling-convention-system-v-amd64)

This page is a **64-bit (x86-64)** adaptation of the classic **32-bit x86 Assembly Guide**, rewritten for **NASM** in **Intel syntax**.

We assume **x86-64 on UNIX-like systems** (Linux/BSD/macOS) and focus on the instruction subset used in typical systems/CS courses.

## NASM + Intel syntax basics

Compared to AT&T syntax:

- Operand order is **destination, source**.
- Registers are written **without** a `%` prefix (e.g., `rax`, not `%rax`).
- Immediate constants are written **without** a `$` prefix (e.g., `123`, `0xABC`).
- Memory operands use brackets: `[...]` (e.g., `[rbp-8]`).
- NASM comments use `;`.

### Sections and symbols in NASM

NASM uses *section* directives (not `.text/.data`):

- `section .text` for code
- `section .data` for initialized globals
- `section .bss` for uninitialized globals

Symbols are exported/imported with:

- `global name`
- `extern name`

### 64-bit mode and RIP-relative addresses

For x86-64 NASM source, it’s common to include:

- `bits 64` — assemble for 64-bit mode
- `default rel` — treat bare `[symbol]` memory references as **RIP-relative** where appropriate (very convenient for position-independent code)

Minimal skeleton:

```nasm
bits 64
default rel

section .text
global myFunc

myFunc:
    ret
```

---

## Registers

In 64-bit mode, x86-64 provides **sixteen** general purpose registers, each **64 bits** wide. Two registers are still used by convention for stack management: the stack pointer `rsp` and (optionally) the base/frame pointer `rbp`.

**General purpose registers (64-bit)**

|              | Meaning (by convention)                                 |
|--------------|----------------------------------------------------------|
| `rax`        | accumulator / return value                               |
| `rbx`        | callee-saved general register                            |
| `rcx`        | counter / shift count uses `cl`                          |
| `rdx`        | used with mul/div; also arg register                     |
| `rsi`        | arg register (often “source index” historically)         |
| `rdi`        | arg register (often “destination index” historically)    |
| `rbp`        | frame pointer (optional)                                 |
| `rsp`        | stack pointer                                            |
| `r8` … `r15` | additional general registers                             |

Most registers also have smaller “views” (sub-registers) used for 32-bit, 16-bit, or 8-bit operations.

|          |          |          |              |              |
|----------|----------|----------|--------------|--------------|
| **64**   | **32**   | **16**   | **8 low**    | **8 high**   |
| `rax`    | `eax`    | `ax`     | `al`         | `ah`         |
| `rbx`    | `ebx`    | `bx`     | `bl`         | `bh`         |
| `rcx`    | `ecx`    | `cx`     | `cl`         | `ch`         |
| `rdx`    | `edx`    | `dx`     | `dl`         | `dh`         |
| `rsi`    | `esi`    | `si`     | `sil`        | (none)       |
| `rdi`    | `edi`    | `di`     | `dil`        | (none)       |
| `rbp`    | `ebp`    | `bp`     | `bpl`        | (none)       |
| `rsp`    | `esp`    | `sp`     | `spl`        | (none)       |
| `r8`     | `r8d`    | `r8w`    | `r8b`        | (none)       |
| `r9`     | `r9d`    | `r9w`    | `r9b`        | (none)       |
| `r10`    | `r10d`   | `r10w`   | `r10b`       | (none)       |
| `r11`    | `r11d`   | `r11w`   | `r11b`       | (none)       |
| `r12`    | `r12d`   | `r12w`   | `r12b`       | (none)       |
| `r13`    | `r13d`   | `r13w`   | `r13b`       | (none)       |
| `r14`    | `r14d`   | `r14w`   | `r14b`       | (none)       |
| `r15`    | `r15d`   | `r15w`   | `r15b`       | (none)       |

**Important x86-64 rule:** writing a 32-bit sub-register (e.g., `eax`) **zero-extends** into the full 64-bit register (so writing `eax` clears the upper 32 bits of `rax`). This does *not* happen for 8-bit or 16-bit writes.

---

## Memory and Addressing Modes

### Declaring Static Data Regions

Static data regions (like global variables) typically live in `section .data` (initialized) or `section .bss` (uninitialized).

NASM uses:

- `db` (1 byte)
- `dw` (2 bytes)
- `dd` (4 bytes)
- `dq` (8 bytes)

Example declarations:

```nasm
section .data

var: db 64
     db 10

x:   dw 42

y:   dd 30000

z:   dq 0x1122334455667788
```

Arrays are contiguous memory cells. For 64-bit integer arrays, use `dq`. For byte arrays and strings, use `db`. For large areas of zeros you can use `times` (initialized) or `.bss` (uninitialized).

```nasm
section .data

arr32: dd 1, 2, 3            ; 3 x 4 bytes, so arr32 + 8 is 3
arr64: dq 1, 2, 3            ; 3 x 8 bytes, so arr64 + 16 is 3
barr:  times 10 db 0         ; 10 zero bytes
str:   db "hello", 0         ; bytes for hello followed by NUL

section .bss
buf:   resb 10                ; 10 uninitialized bytes
```

### Addressing Memory

In 64-bit mode, pointers and addresses are **64-bit** quantities. Labels are replaced by the assembler/linker with addresses.

Memory addresses are written in brackets using the form:

`[ base + index*scale + displacement ]`

where `scale ∈ {1,2,4,8}` and the `index*scale` part is optional.

#### RIP-relative globals in NASM

In x86-64, globals are commonly accessed using **RIP-relative** addressing.

With `default rel`, you can usually write:

- `mov rax, [var]`       (load)
- `mov [var], rbx`       (store)
- `lea rax, [var]`       (address of global)

Without `default rel`, you can write the explicit form:

- `mov rax, [rel var]`
- `lea rax, [rel var]`

Examples using `mov`:

|                            |                                                                 |
|----------------------------|-----------------------------------------------------------------|
| `mov rax, [rbx]`           | Load 8 bytes from address in RBX into RAX.                      |
| `mov [var], rbx`           | Store RBX into global variable `var` (RIP-relative with `default rel`). |
| `mov eax, dword [rsi-4]`   | Load 4 bytes from (RSI-4) into EAX (zero-extends into RAX).     |
| `mov [rsi+rax], cl`        | Store 1 byte (CL) to address RSI+RAX.                           |
| `mov rdx, [rsi+rbx*4]`     | Load 8 bytes from address RSI + 4*RBX into RDX.                 |

Some invalid address calculations (same restrictions as 32-bit):

|                                   |                                                                 |
|-----------------------------------|-----------------------------------------------------------------|
| `mov rax, [rbx + rcx*3]`          | Scale must be 1,2,4, or 8 (not 3).                              |
| `mov [rax + rsi + rdi], rbx`      | At most 2 registers in the address computation.                 |

### Operand size specifiers

NASM usually infers operand size from registers (e.g., `mov eax, [rsi]` implies a 32-bit load). But when a memory operand’s size is ambiguous, specify it explicitly:

- `byte`  (1 byte)
- `word`  (2 bytes)
- `dword` (4 bytes)
- `qword` (8 bytes)

For example, storing `2` to memory is ambiguous without a size:

```nasm
mov byte  [rbx], 2
mov word  [rbx], 2
mov dword [rbx], 2
mov qword [rbx], 2
```

---

## Instructions

Machine instructions fall into three broad categories: data movement, arithmetic/logic, and control-flow. This is not exhaustive; it is a useful subset.

Notation used below:

|           |                                                                             |
|-----------|-----------------------------------------------------------------------------|
| `<reg64>` | Any 64-bit register (`rax`, `rbx`, …, `r15`)                                |
| `<reg32>` | Any 32-bit register (`eax`, `ebx`, …)                                       |
| `<reg16>` | Any 16-bit register (`ax`, `bx`, …)                                         |
| `<reg8>`  | Any 8-bit register (`al`, `cl`, `r8b`, …)                                   |
| `<mem>`   | A memory operand (e.g. `[rax]`, `[rbp+8]`, `[var]`, `[rax+rbx*4]`)          |
| `<imm>`   | Any immediate constant (size depends on instruction/assembler)              |

Immediate constants are written without a prefix: `123`, `0xABC`, etc.

### Data Movement Instructions

`mov` — Move

Copies data from the source operand into the destination operand. Register-to-register is allowed; direct memory-to-memory is not (use a register as a temporary).

*Syntax*  
`mov <reg>, <reg>`  
`mov <reg>, <mem>`  
`mov <mem>, <reg>`  
`mov <reg>, <imm>`  
`mov <mem>, <imm>`  

*Examples*  
`mov rax, rbx` — copy RBX into RAX  
`mov byte [var], 5` — store 5 into the byte at `var` (RIP-relative with `default rel`)  
`mov eax, 0` — set EAX to 0 (also clears upper half of RAX)  

`push` — Push on stack

Pushes an 8-byte value onto the stack: decrements `rsp` by 8, then stores the value at `[rsp]`.

*Syntax*  
`push <reg64>`  
`push <mem>`  
`push <imm>`

*Examples*  
`push rax`  
`push qword [var]`  

`pop` — Pop from stack

Pops an 8-byte value from the stack: loads from `[rsp]`, then increments `rsp` by 8.

*Syntax*  
`pop <reg64>`  
`pop <mem>`

*Examples*  
`pop rdi`  
`pop qword [rbx]`  

`lea` — Load effective address

Computes an address and places it in a register (does not load memory contents). Often used for pointer arithmetic and for RIP-relative addresses.

*Syntax*  
`lea <reg64>, <mem>`

*Examples*  
`lea rdi, [rbx + rsi*8]` — RDI = RBX + 8*RSI  
`lea rax, [var]` — RAX = &var (RIP-relative with `default rel`)  

### Arithmetic and Logic Instructions

`add` — Integer addition

Adds the two operands, storing the result in the destination operand. At most one operand may be memory.

*Examples*  
`add rax, 10` — RAX = RAX + 10  
`add byte [rax], 10` — add 10 to the byte at address RAX  

`sub` — Integer subtraction

Subtracts the source operand from the destination operand, storing the result in the destination operand.

*Examples*  
`sub rax, 216`  
`sub al, ah` — still valid for 8-bit sub-registers  

`inc, dec` — Increment / Decrement

Increment or decrement by one.

*Examples*  
`dec rax`  
`inc dword [var]` — add one to a 32-bit integer at `var`  

`imul` — Integer multiplication

The two-operand form multiplies its operands and stores the result in the destination operand (a register). A three-operand form exists with an immediate multiplier.

*Examples*  
`imul rax, qword [rbx]` — RAX *= *(qword*)RBX  
`imul rsi, rdi, 25` — RSI = RDI * 25  

`idiv` — Signed integer division

Divides the signed 128-bit integer in `rdx:rax` (high:low) by the operand. Quotient is stored in `rax`, remainder in `rdx`.

Typically you prepare `rdx:rax` using `cqo` (sign-extend RAX into RDX).

*Example*  
```nasm
cqo
idiv rbx        ; (RDX:RAX) / RBX
```

`and, or, xor` — Bitwise logical operations

Perform the operation and store the result in the destination operand.

*Examples*  
`and rax, 0x0f` — clear all but the last 4 bits  
`xor rdx, rdx` — set RDX to zero  

`not` — Bitwise NOT

*Example*  
`not rax`

`neg` — Two's complement negation

*Example*  
`neg rax`

`shl, shr, sar` — Shifts

Shift count is an 8-bit immediate or `cl`. For 64-bit operands, shift counts are effectively taken modulo 64, and the operand can be shifted up to 63 places.

*Examples*  
`shl rax, 1` — RAX *= 2 (if no overflow concern)  
`shr rbx, cl` — RBX = floor(RBX / 2^CL) for unsigned values  
`sar rbx, cl` — arithmetic right shift (sign-propagating)  

### Control Flow Instructions

The processor maintains an instruction pointer `rip`, a 64-bit value pointing to the current instruction. It cannot be written directly, but is changed by control-flow instructions.

`jmp` — Jump

Unconditional jump to a label or indirect target.

*Examples*  
`jmp begin`  
`jmp rax` — indirect jump to address in RAX  

`j<condition>` — Conditional jump

Conditional branches based on flags set by a previous instruction (often `cmp`). Common conditions: `je`, `jne`, `jg`, `jge`, `jl`, `jle`.

```nasm
cmp rax, rbx
jle done
```

`cmp` — Compare

Like subtraction for flags, but discards the result.

*Example*  
```nasm
cmp byte [rbx], 10
je loop
```

`call, ret` — Call and return

`call` pushes an 8-byte return address (next `rip`) onto the stack and jumps to the target. `ret` pops that address and jumps back.

---

## Calling Convention (System V AMD64)

In 32-bit x86, a common “C calling convention” passes parameters on the stack. In **64-bit UNIX-like systems**, the standard is the **System V AMD64 ABI**, which passes the first arguments in registers. (Windows uses a different convention; see note at the end.)

### Argument passing

The first six integer/pointer arguments are passed in registers:

|        |        |
|--------|--------|
| `arg1` | `rdi` |
| `arg2` | `rsi` |
| `arg3` | `rdx` |
| `arg4` | `rcx` |
| `arg5` | `r8`  |
| `arg6` | `r9`  |

Additional arguments (7 and beyond) are passed on the stack. Integer/pointer return values are placed in `rax`.

### Caller-saved vs callee-saved

Registers are divided into those the caller must assume can be clobbered (caller-saved), and those a callee must preserve if it uses them (callee-saved). A common summary:

|                  |                                               |
|------------------|-----------------------------------------------|
| **Callee-saved** | `rbx rbp r12 r13 r14 r15`                     |
| **Caller-saved** | `rax rcx rdx rsi rdi r8 r9 r10 r11`           |

### Stack alignment

Before executing a `call`, the stack pointer `rsp` must be aligned to a **16-byte boundary**. Because `call` pushes an 8-byte return address, a typical callee entry sees `rsp` misaligned by 8 and fixes alignment in its prologue as needed.

### Example: making a call (caller side)

Call `myFunc(p1, 216, *p3)` where: `p1` is in `rax`, and `rbx` holds a pointer to the third argument value.

```nasm
mov rdi, rax        ; arg1 = p1
mov rsi, 216        ; arg2 = 216
mov rdx, [rbx]      ; arg3 = *p3
; ensure 16-byte stack alignment here if needed
call myFunc         ; return value in rax
```

### Example: function definition (callee side)

A simple function that returns `arg1 + (arg2 + arg3)`. This version uses a frame pointer (like the 32-bit guide) for clarity.

```nasm
bits 64
default rel

section .text
global myFunc

myFunc:
    ; Prologue
    push rbp
    mov  rbp, rsp
    sub  rsp, 16            ; space for locals, keeps stack aligned

    ; Body (args in rdi, rsi, rdx)
    mov  qword [rbp-8], rdx
    add  qword [rbp-8], rsi
    mov  rax, rdi
    add  rax, qword [rbp-8]

    ; Epilogue
    leave
    ret
```

### Windows note

If you are targeting **Windows x64**, the integer argument registers are `RCX, RDX, R8, R9`, and the caller must reserve 32 bytes of “shadow space” on the stack. The rest of this section assumes SysV AMD64.

---

**Credits:** Based on the structure of the classic x86 Assembly Guide (Ferrari/Batson/Lack/Jones/Evans), and later AT&T-syntax revisions. This page is a teaching-focused x86-64 adaptation.
