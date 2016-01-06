#ifndef _ELF_LOADER_H
#define _ELF_LOADER_H

#include <stdint.h>

typedef enum {
	ELF_OK,
	ELF_ERR_INVALID_HEADER,
	ELF_ERR_INVALID_PROGRAM_HEADER
} elf_err;

elf_err load_elf(void *elf_data, uint32_t *entry);

void run_elf(uint32_t arg0, void *stack, uint32_t entry);

#endif
