/*
 * trace.h
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-03-14 14:22:09
 * @modified: 2025-03-16 13:05:48
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
#ifndef __INCLUDE_TRACE__
#define __INCLUDE_TRACE__

#include "common.h"
#include "cpu/decode.h"
#include <elf.h>

// itrace
#define ITRACE_POOL_SIZE 32
#define ITRACE_BUF_SIZE 32

typedef struct ITraceNode {
    vaddr_t pc;
    word_t inst;
    struct ITraceNode *next;
    char instdec[ITRACE_BUF_SIZE];
} ITraceNode;

void irb_trace(word_t inst, char *instdec, vaddr_t pc);
void irb_display();
void init_itrace();

// mtrace

typedef struct MTraceNode {
    paddr_t addr;
    bool is_read;
    word_t data;
} MTraceNode;

void mtrace_write(paddr_t addr, int len, word_t data);
void mtrace_read(paddr_t addr, int len, word_t data);


// ftrace
#ifdef CONFIG_ISA64
typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Sym Elf_Sym;
typedef Elf64_Off Elf_Off;
typedef Elf64_Addr Elf_Addr;
typedef Elf64_Half Elf_Half;
typedef Elf64_Word Elf_Word;
typedef Elf64_Sword Elf_Sword;

#define ELF_ST_TYPE ELF64_ST_TYPE
#else
typedef Elf32_Ehdr Elf_Ehdr;
typedef Elf32_Shdr Elf_Shdr;
typedef Elf32_Sym Elf_Sym;
typedef Elf32_Off Elf_Off;
typedef Elf32_Addr Elf_Addr;
typedef Elf32_Half Elf_Half;
typedef Elf32_Word Elf_Word;
typedef Elf32_Sword Elf_Sword;

#define ELF_ST_TYPE ELF32_ST_TYPE
#endif

typedef struct FuncSymbol {
  word_t addr;
  word_t size;
  struct FuncSymbol *next;
  char name[64];
} FuncSymbol;

/// load function symbols from ELF file
void fsym_load(FILE *fp);
void fsym_display();
void ftrace_call(Decode *s);
void ftrace_ret(Decode *s);
#endif // __INCLUDE_TRACE__
