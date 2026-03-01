kernel := build/kernel.bin
iso := build/hello.iso

linker_script := linker.ld
grub_cfg := boot/grub.cfg
assembly_source_files := $(wildcard *.S)
assembly_object_files := $(patsubst %.S, build/%.o, $(assembly_source_files))
c_source_files := $(wildcard *.c)
c_object_files := $(patsubst %.c, build/%.o, $(c_source_files))

# Cross-compiling (e.g., on Mac OS X)
# TOOLPREFIX = x86_64-elf-

# Using native tools (e.g., on X86-64 Linux)
#TOOLPREFIX = 

# Try to infer the correct TOOLPREFIX if not set
ifndef TOOLPREFIX
TOOLPREFIX := $(shell if x86_64-elf-objdump -i 2>&1 | grep '^elf64-x86-64' >/dev/null 2>&1; \
	then echo 'x86_64-elf-'; \
	elif objdump -i 2>&1 | grep 'elf64-x86-64' >/dev/null 2>&1; \
	then echo ''; \
	else echo "***" 1>&2; \
	echo "*** Error: Couldn't find an x86_64-elf version of GCC/binutils." 1>&2; \
	echo "*** Is the directory with x86_64-elf-gcc in your PATH?" 1>&2; \
	echo "*** If your x86_64-elf toolchain is installed with a command" 1>&2; \
	echo "*** prefix other than 'x86_64-elf-', set your TOOLPREFIX" 1>&2; \
	echo "*** environment variable to that prefix and run 'make' again." 1>&2; \
	echo "*** To turn off this error, run 'gmake TOOLPREFIX= ...'." 1>&2; \
	echo "***" 1>&2; exit 1; fi)
endif

# If the makefile can't find QEMU, specify its path here
# QEMU = qemu-system-x86_64

# Try to infer the correct QEMU
ifndef QEMU
QEMU := $(shell if which qemu > /dev/null; \
	then echo qemu; \
	elif which qemu-system-x86_64 > /dev/null; \
	then echo qemu-system-x86_64; \
	else \
	echo "***" 1>&2; \
	echo "*** Error: Couldn't find a working QEMU executable." 1>&2; \
	echo "*** Is the directory containing the qemu binary in your PATH" 1>&2; \
	echo "*** or have you tried setting the QEMU variable in Makefile?" 1>&2; \
	echo "***" 1>&2; fi)
endif

CC = $(TOOLPREFIX)gcc
LD = $(TOOLPREFIX)ld

CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -Og -Wno-infinite-recursion -Wall -MD -ggdb -Werror -fno-omit-frame-pointer -mno-default -fno-pie -no-pie -m64 -mcmodel=large
LDFLAGS := --nmagic

UNAME_A := $(shell uname -a)

ifneq (,$(findstring eng.utah.edu,$(UNAME_A)))
    mkrescue := grub2-mkrescue
    MKRESCUEFLAGS = -d /home/cs5460/grub/lib/grub/i386-pc
else
    mkrescue := $(TOOLPREFIX)grub-mkrescue
    MKRESCUEFLAGS =
endif

.PHONY: all clean qemu qemu-nox qemu-gdb qemu-gdb-nox iso kernel

all: $(kernel)

clean:
	rm -r build

qemu: $(iso)
	$(QEMU) -cdrom $(iso) -display curses -vga std -serial file:serial.log

qemu-nox: $(iso)
	$(QEMU) -m 128 -cdrom $(iso) -vga std -no-reboot -nographic 

qemu-gdb: $(iso)
	$(QEMU) -S -m 128 -cdrom $(iso) -display curses -vga std -s -serial file:serial.log -no-reboot -no-shutdown -d int,cpu_reset 

qemu-gdb-nox: $(iso)
	$(QEMU) -S -m 128 -cdrom $(iso) -vga std -s -serial file:serial.log -no-reboot -no-shutdown -d int,cpu_reset -nographic

iso: $(iso)
	@echo "Done"

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	cp $(kernel) build/isofiles/boot/kernel.bin
	cp $(grub_cfg) build/isofiles/boot/grub
	$(mkrescue) $(MKRESCUEFLAGS) -o $(iso) build/isofiles #2> /dev/null
	@rm -r build/isofiles

kernel: $(kernel)
	@echo "Done"

$(kernel): $(c_object_files) $(assembly_object_files) $(linker_script)
	$(LD) $(LDFLAGS) -T $(linker_script) -o $(kernel) $(assembly_object_files) $(c_object_files)

# compile C files
build/%.o: %.c
	@mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS) -c $< -o $@

# compile assembly files
build/%.o: %.S
	@mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS) -c $< -o $@

