/*
 * ftrace.c
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-03-15 11:27:50
 * @modified: 2025-03-16 15:10:08
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
#include "common.h"
#include "cpu/decode.h"
#include "debug.h"
#include "trace.h"
#include "utils.h"
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *fread_chunk(FILE *fp, size_t offset, size_t size) {
  void *buf = malloc(size);
  fseek(fp, offset, SEEK_SET);
  assert(fread(buf, size, 1, fp) != -1);
  return buf;
}

static FuncSymbol *fsym_head = NULL;
static int         fsym_size = 0;

static int ftrace_depth = 0;

void fsym_load(FILE *fp) {
  // elf header
  Elf_Ehdr *ehdr = (Elf_Ehdr *)fread_chunk(fp, 0, sizeof(Elf_Ehdr));
  Elf_Off   shoff = ehdr->e_shoff;         // section header table file offset
  Elf_Half  shnum = ehdr->e_shnum;         // number of section header table
  Elf_Half  shentsize = ehdr->e_shentsize; // size of section header table entry
  Elf_Half  shstrndx = ehdr->e_shstrndx;   // section header string table index

  // find symtab and strtab
  Elf_Half symtab_off = 0;
  int      nsym = 0;
  char    *shstrtab_str = NULL;
  char    *strtab_str = NULL;

  Elf_Shdr *shstrtab = (Elf_Shdr *)fread_chunk(fp, shoff + shstrndx * shentsize, sizeof(Elf_Shdr));
  shstrtab_str = (char *)fread_chunk(fp, shstrtab->sh_offset, shstrtab->sh_size + 1);
  free(shstrtab);

  // find symtab and strtab
  for (int i = 0; i < shnum; i++) {
    Elf_Shdr *shdr = (Elf_Shdr *)fread_chunk(fp, shoff + i * shentsize, sizeof(Elf_Shdr));

    if (shdr->sh_type == SHT_SYMTAB) {
      symtab_off = shdr->sh_offset;
      nsym = shdr->sh_size / sizeof(Elf_Sym);
    } else if (strncmp(shstrtab_str + shdr->sh_name, ".strtab", 7) == 0) {
      strtab_str = (char *)fread_chunk(fp, shdr->sh_offset, shdr->sh_size);
    }
    free(shdr);
  }

  for (int i = 0; i < nsym; i++) {
    Elf_Sym *sym = (Elf_Sym *)fread_chunk(fp, symtab_off + i * sizeof(Elf_Sym), sizeof(Elf_Sym));
    if (ELF_ST_TYPE(sym->st_info) == STT_FUNC) {
      FuncSymbol *fsym = (FuncSymbol *)malloc(sizeof(FuncSymbol));
      strncpy(fsym->name, strtab_str + sym->st_name, sizeof(fsym->name));
      fsym->addr = sym->st_value;
      fsym->size = sym->st_size;
      fsym->next = fsym_head;
      fsym_head = fsym;
      fsym_size++;
    }
    free(sym);
  }

  free(ehdr);
  free(shstrtab_str);
  free(strtab_str);
}

void fsym_display() {
  FuncSymbol *fsym = fsym_head;
  while (fsym != NULL) {
    printf(FMT_PADDR " - " FMT_PADDR ": %s\n", fsym->addr, fsym->addr + fsym->size, fsym->name);
    fsym = fsym->next;
  }
}

void ftrace_call(Decode *s) {
  vaddr_t     pc = s->pc;
  vaddr_t     dnpc = s->dnpc;
  char        buf[256];
  char       *p = buf;
  FuncSymbol *fsym = fsym_head;

  while (fsym != NULL) {
    if (dnpc >= fsym->addr && dnpc < fsym->addr + fsym->size) {
      p += snprintf(p, 16, FMT_PADDR ":", pc);
      p += snprintf(p, 200, "%*s call [%s@" FMT_PADDR "]", ftrace_depth * 2, "", fsym->name, s->dnpc);
      _Log("%% %s\n", buf);
      ftrace_depth++;
      break;
    } else {
      fsym = fsym->next;
    }
  }
}

void ftrace_ret(Decode *s) {
  vaddr_t     pc = s->pc;
  char        buf[256];
  char       *p = buf;
  FuncSymbol *fsym = fsym_head;

  while (fsym != NULL) {
    if (pc >= fsym->addr && pc < fsym->addr + fsym->size) {
      Assert(ftrace_depth > 0, "ftrace depth should be greater than 0");
      ftrace_depth--;
      p += snprintf(p, 16, FMT_PADDR ":", pc);
      p += snprintf(p, 200, "%*s ret [%s]", ftrace_depth * 2, "", fsym->name);
      _Log("%% %s\n", buf);
      break;
    } else {
      fsym = fsym->next;
    }
  }
}