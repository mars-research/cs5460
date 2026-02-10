// Homework 2 - ELF
//
// Useful materials:
// - https://refspecs.linuxfoundation.org/elf/elf.pdf
// - https://sourceware.org/git/?p=elfutils.git;a=blob;f=libelf/elf.h
//
// Further readings:
// - https://maskray.me/blog/2024-01-14-exploring-object-file-formats

#ifndef __x86_64__
#error This code requires x86-64 support
#endif

#include <stdbool.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>

// ## Definitions
//
// Simplified from <elf.h> so things are easier to grok:

#ifndef Elf64_Ehdr
void *load_base;
typedef struct {
    uint64_t r_offset;
    uint64_t r_info;
    int64_t  r_addend;
} Elf64_Rela;
// ### ELF Header
typedef struct {
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} Elf64_Ehdr;

// ### Program Header
typedef struct {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} Elf64_Phdr;

#define PT_LOAD 1

// ### Section Header
typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
} Elf64_Shdr;

#define SHT_SYMTAB  2
#define SHT_STRTAB  3
#define SHT_RELA    4
#define SHT_REL     9

// ### Relocation Entry
typedef struct {
    uint64_t r_offset;
    uint64_t r_info;
} Elf64_Rel;

#define ELF64_R_TYPE(val) ((val) & 0xffffffff)
#define ELF64_R_SYM(val)  ((val) >> 32)
#define R_X86_64_RELATIVE 8

// ### Symbol
typedef struct {
    uint32_t st_name;
    uint8_t  st_info;
    uint8_t  st_other;
    uint16_t st_shndx;
    uint64_t st_value;
    uint64_t st_size;
} Elf64_Sym;

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

static size_t page_align(size_t v) {
    return (v + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
}

void *load_multiple(FILE *f, size_t offset, size_t size,
                    size_t entry_size, size_t *num_ptr) {
    if (size % entry_size) {
        ABORT("Size not a multiple of entries\n");
    }

    void *entries = malloc(size);
    if (!entries) {
        ABORT("malloc failed\n");
    }

    size_t num = size / entry_size;
    fseek(f, offset, SEEK_SET);
    if (fread(entries, entry_size, num, f) != num) {
        ABORT("short read\n");
    }

    if (num_ptr) {
        *num_ptr = num;
    }
    return entries;
}

void *get_sm(Elf64_Sym *syms, char *strtab,
             int num_syms, const char *name,
             uint64_t min_vaddr) {
    for (int i = 0; i < num_syms; ++i) {
        if (strcmp(&strtab[syms[i].st_name], name) == 0) {
            return (uint8_t *)load_base +
                   (syms[i].st_value - min_vaddr);
        }
    }
    return NULL;
}

static const char *default_func_for_file(const char *filename) {
    if (strcmp(filename, "elf") == 0) {
        return "add";
    }
    if (strcmp(filename, "elf1") == 0) {
        return "linear_transform";
    }
    if (strcmp(filename, "elf_extra_credit") == 0) {
        return "linear_transform";
    }
    if (strcmp(filename, "ml_main") == 0) {
        return "ml_func";
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    // Argument handling:
    //   ./main elf                 -> calls add
    //   ./main elf1                -> calls linear_transform
    //   ./main elf_extra_credit    -> calls linear_transform
    //   ./main <elf> <funcname>    -> calls funcname
    const char *filename = NULL;
    const char *funcname = NULL;

    if (argc < 2 || argc > 3) {
        ABORT("Usage: %s <elf file name> [function name]\n", argv[0]);
    }

    filename = argv[1];
    funcname = (argc == 3) ? argv[2] : default_func_for_file(filename);
    if (!funcname) {
        ABORT("No default function for '%s'. Pass a function name explicitly.\n", filename);
    }

//     Elf64_Ehdr elf;
//     int (*add)(int a, int b);  // elf.c
//     int (*linear_transform)(int a); //elf1.c
//     int ret, items;

//     // filename + funcname are set above for flexible invocation.

//     size_t max_vaddr = 0;
//     size_t min_vaddr = UINT64_MAX;

//     FILE* f = fopen(filename, "r");
//     if (!f) {
//         ABORT("open failed\n");
//     }

//     fread(&elf, sizeof(Elf64_Ehdr), 1, f);

//     if (elf.e_phentsize != sizeof(Elf64_Phdr)) {
//         ABORT("Unexpected PHDR size\n");
//     }

//     Elf64_Phdr *phs =
//         load_multiple(f, elf.e_phoff,
//                       elf.e_phnum * sizeof(Elf64_Phdr),
//                       sizeof(Elf64_Phdr), NULL);

//     for (int i = 0; i < elf.e_phnum; ++i) {
//         if (phs[i].p_type != PT_LOAD) continue;
//         if (phs[i].p_vaddr < min_vaddr){
//             // to do
//         }
//         if (phs[i].p_vaddr + phs[i].p_memsz > max_vaddr){
//             // to do
//         }
//     }

//     load_base = mmap(NULL, page_align(max_vaddr - min_vaddr),
//                      PROT_READ | PROT_WRITE | PROT_EXEC,
//                      MAP_ANONYMOUS | MAP_PRIVATE, 0, 0); // Allocate Memory

    

//     for (int i = 0; i < elf.e_phnum; i++) {
//         if (phs[i].p_type != PT_LOAD) continue;
//         void *seg = (uint8_t *)load_base + (phs[i].p_vaddr - min_vaddr);

//         fseek(f, phs[i].p_offset, SEEK_SET);
//         fread(seg, 1, phs[i].p_filesz, f);

//         if (phs[i].p_memsz > phs[i].p_filesz) {
//             // to do
//         }
//     }
//     free(phs);

//     // if (entry_point) {
//             // add = entry_point;
//             // ret = add(1, 2);
//             // printf("add:%d\n", ret);
//     // }
//     // return 0; 
//     // ########################################################### 
//         // PERFORM RELOCATION BELOW (NEEDED FOR elf1.c)
//         // Below sections can be commented out for Part 1, as no relocations are applied 
//     // ###########################################################

//     if (elf.e_shentsize != sizeof(Elf64_Shdr)) {
//         ABORT("Unexpected SHDR size\n");
//     }

//     Elf64_Shdr *shs =
//         load_multiple(f, elf.e_shoff, elf.e_shnum * sizeof(Elf64_Shdr), sizeof(Elf64_Shdr), NULL);

//     size_t relnum = 0, num_syms = 0, relanum = 0;
//     Elf64_Rel *rels = NULL;
//     Elf64_Sym *syms = NULL;
//     Elf64_Rela *relas = NULL;
//     char *strtab = NULL;
//     bool string_table = false;

//     for (int i = 0; i < elf.e_shnum; i++) {
//         // Loads the ELF section headers and extracts relocation, symbol table, and string table sections
//         Elf64_Shdr *sh = shs + i;
//         switch (sh->sh_type) {
//             case SHT_RELA:
//                 // to do
//             case SHT_REL:
//                 // to do
//             case SHT_SYMTAB:
//                 // to do
//             case SHT_STRTAB:
//                 // to do
//   }
//     }
//     free(shs);


//     uint64_t delta = (uint64_t)load_base - min_vaddr;

//     if (relas) {
//         for (size_t j = 0; j < relanum; ++j) {
//             if (type == R_X86_64_RELATIVE) {
//                 // Apply relocations
//             }
//         }
//         free(relas);
//     }

//     fclose(f);

//     LOG("Loaded binary\n");

//     // Call Funtion, add or linear_transform
//     // Example:
//     // if (strcmp(funcname, "add") == 0) {
//     //     add = get_sm(syms, strtab, num_syms, funcname, min_vaddr);
//     //     ret = add(1, 2);
//     // } else if (strcmp(funcname, "linear_transform") == 0) {
//     //     linear_transform = get_sm(syms, strtab, num_syms, funcname, min_vaddr);
//     //     ret = linear_transform(4);
//     // }

//     return 0; 
}

// vim: et:ts=4:sw=4
