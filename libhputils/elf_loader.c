#include "elf_loader.h"

#include "elf.h"
#include "lib.h"

elf_err load_elf(void *elf_data, uint32_t *entry) {
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)elf_data;

	/* Check that the payload is an ELF file. */
	if ((ehdr->e_ident[EI_MAG0] != ELFMAG0) ||
	    (ehdr->e_ident[EI_MAG1] != ELFMAG1) ||
	    (ehdr->e_ident[EI_MAG2] != ELFMAG2) ||
	    (ehdr->e_ident[EI_MAG3] != ELFMAG3) ||
	    (ehdr->e_ident[EI_CLASS] != ELFCLASS32) ||
	    (ehdr->e_ident[EI_DATA] != ELFDATA2LSB) ||
	    (ehdr->e_type != ET_EXEC) ||
	    (ehdr->e_machine != EM_ARM) ||
	    (ehdr->e_phentsize != sizeof(Elf32_Phdr)) ||
	    (ehdr->e_phnum != 1))
		return ELF_ERR_INVALID_HEADER;

	Elf32_Phdr *phdr = (Elf32_Phdr *) ((char*)elf_data + ehdr->e_phoff);

	/* Check that the payload program header is sane. */
	if ((phdr->p_type != PT_LOAD) ||
	    (phdr->p_paddr < 0x30000000) ||
	    (phdr->p_paddr >= 0x32000000))
		return ELF_ERR_INVALID_PROGRAM_HEADER;

	/* Load the payload. */
	memset((char*)(phdr->p_paddr), 0, phdr->p_memsz);
	memcpy((char*)(phdr->p_paddr), (char*)elf_data + phdr->p_offset, phdr->p_filesz);

	*entry = ehdr->e_entry;

	return ELF_OK;
}
