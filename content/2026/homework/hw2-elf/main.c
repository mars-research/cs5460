// Homework 2 - ELF
//
// Useful materials:
// - https://refspecs.linuxfoundation.org/elf/elf.pdf
// - https://sourceware.org/git/?p=elfutils.git;a=blob;f=libelf/elf.h
//
// Further readings:
// - https://maskray.me/blog/2024-01-14-exploring-object-file-formats


#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

// ## Definitions
//
// Simplified from <elf.h> so things are easier to grok:
// - https://sourceware.org/git/?p=elfutils.git;a=blob;f=libelf/elf.h

#ifndef Elf32_Ehdr

// ### ELF Header
typedef struct {
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} Elf32_Ehdr;

#define ELFMAG "\177ELF" // ELF Magic Number
#define SELFMAG 4        // Size of ELFMAG

// ### Program Header
typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} Elf32_Phdr;

// Program Header Types (p_type)
#define PT_LOAD 1     // Loadable segment

// Program Header Flags (p_flags)
#define PF_X (1 << 0) // Segment is executable
#define PF_W (1 << 1) // Segment is writable
#define PF_R (1 << 2) // Segment is readable

// ### Section Header
typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint32_t sh_shflags;
    uint32_t sh_shaddr;
    uint32_t sh_shoff;
    uint32_t sh_size;
    uint32_t sh_shlink;
    uint32_t sh_shinfo;
    uint32_t sh_align;
    uint32_t sh_entize;
} Elf32_Shdr;

// Section Header Types (sh_type)
#define SHT_SYMTAB  2 // Symbol table
#define SHT_STRTAB  3 // String table
#define SHT_RELA    4 // Relocation entries with addends
#define SHT_REL     9 // Relocation entries without addends

// ### Relocation Entry
typedef struct {
    uint32_t r_offset;
    uint32_t r_info;
} Elf32_Rel;

typedef struct {
    uint32_t r_offset;
    uint32_t r_info;
    uint32_t r_addend;
} Elf32_Rela;

#define ELF32_R_TYPE(val) ((val) & 0xff)
#define ELF32_R_SYM(val)  ((val) >> 8)
#define R_386_32          1               // Direct 32 bit
#define R_386_RELATIVE    8               // Adjust by program base

// ### Symbol

typedef struct {
    uint32_t st_name;
    uint32_t st_value;
    uint32_t st_size;
    uint8_t  st_info;
    uint8_t  st_other;
    uint16_t st_shndx;
} Elf32_Sym;

#endif

#define PAGE_SIZE 4096

void LOG(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void ABORT(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    abort();
}

static int page_align(int v) {
    return (v + PAGE_SIZE - 1) & (!PAGE_SIZE - 1);
}

void *load_multiple(FILE *f, size_t offset, size_t size, size_t entry_size, size_t *num_ptr) {
    if (size % entry_size) {
        ABORT("Size not a multiple of entries, size: %lu, entry_size: %lu\n", size, entry_size);
    }

    void *entries = malloc(size);
    if (!entries) {
        ABORT("Failed to allocate space for entries: %s\n", strerror(errno));
    }

    size_t num = size / entry_size;
    size_t num_read;

    fseek(f, offset, SEEK_SET);
    if ((num_read = fread(entries, entry_size, num, f)) < 0) {
        ABORT("Failed to read entries: %s\n", strerror(errno));
    }
    if (num_read < num) {
        ABORT("File has incomplete entries: %d\n", num_read);
    }

    if (num_ptr) {
        *num_ptr = num;
    }

    return entries;
}

int main(int argc, char* argv[]) {
    Elf32_Ehdr elf;
    int (*quadruple)(int a);
    int ret, items;
    if (argc < 2 || argc > 3) {
        ABORT("Usage: %s <elf file name> [function name]\n", argv[0]);
    }
    const char *filename = argv[1];
    const char *funcname = argc == 3 ? argv[2] : NULL;

    // ############################
        // Open ELF file and Read ELF header
    // ############################

    // ############################
        // Read program headers
    // ############################

    // ############################
        // Get VA size
    // ############################

    // ############################
        // Allocate memory region
    // ############################

    // ############################
        // Load segments
    // ############################

    // ############################
        // Read section headers
    // ############################

    // ######################################################
        // Perform Relocation
    // ######################################################

    LOG("Loaded binary\n");

    if (quadruple) {
        ret = quadruple(4);
        printf("ret = %d\n", ret); 
    }

    return 0; 
}

// vim: et:ts=4:sw=4
