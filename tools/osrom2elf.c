#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libgen.h>
#include <libelf.h>

typedef struct osrom_hdr {
	Elf32_Addr e_entry;
	unsigned char e_pad1[4];
	Elf32_Word e_size_copy;
	Elf32_Addr e_loadaddr;
	Elf32_Word e_size;
        unsigned char e_ident[8];
	unsigned char e_pad2[4];
} OSROM_Hdr;

const char osHdr_ident[8] = {'V', '5', 'J', 0x00, '2', '4', '1', '6' };

typedef enum {
        CONV_UNKNOWN,
        CONV_ELF2OSROM,
        CONV_OSROM2ELF
} conv_mode;

const int OSROM_SIZE = 1024*1024;
const int OSROM_DATA_OFFSET = sizeof(OSROM_Hdr);
const int OSROM_DATA_SIZE = OSROM_SIZE - OSROM_DATA_OFFSET;

#define ERR_LIBELF() do { \
	fprintf(stderr, "libelf: %s\n", elf_errmsg(elf_errno())); \
} while (0)

#define ERR_LIBELF_FILE(file) do { \
	fprintf(stderr, "%s: %s\n", file, elf_errmsg(elf_errno())); \
} while (0)

#define ERR_FILE(file, err) do { \
	fprintf(stderr, "%s: %s\n", file, err); \
} while (0)

#define FILE_CHECK(file, a, err, go) do { \
	if (!(a)) { \
		ERR_FILE(file, err); \
		goto go; \
	} \
} while(0)

#define LIBELF_CHECK(file, a, b) do { \
	if (!(a)) { \
		ERR_LIBELF_FILE(file); \
		goto b; \
	} \
} while(0)

/*
 * Load an OSROM file in memory.
 */
int load_osrom(char *inPath, char *data) {
	int status = -1;

	int inFd = open(inPath, O_RDONLY);
	if (inFd < 0) {
		perror(inPath);
		goto err0;
	}

	/* Read OSROM and make sure it's exactly 1 MiB in size */
	int numBytes = read(inFd, data, OSROM_SIZE);
	if (numBytes < 0) {
		perror(inPath);
		goto err1;
	}
	else if (numBytes != OSROM_SIZE) {
		fprintf(stderr, "%s: OSROM must be exactly 1 MiB in size\n", inPath);
		goto err1;
	}

	{
		char dummyByte;
		if (read(inFd, &dummyByte, 1) != 0) {
			fprintf(stderr, "%s: OSROM must be exactly 1 MiB in size\n", inPath);
			goto err1;
		}
	}

	status = 0;

err1:
	close(inFd);
err0:
	return status;
}

/*
 * Load an ELF file and convert it to OSROM format in memory.
 */
int load_elf(char *inPath, char *osrom) {
	int status = -1;
	OSROM_Hdr *osrom_Hdr = (OSROM_Hdr*)osrom;

	int inFd = open(inPath, O_RDONLY);
	if (inFd < 0) {
		perror(inPath);
		goto err0;
	}

	/* Read ELF and make sure it's correct enough */
	Elf *elf;
	Elf32_Ehdr *elf_Ehdr;
	Elf32_Phdr *elf_Phdr;
	Elf_Data *elf_Data;
	size_t numPhdr;

	LIBELF_CHECK(inPath, elf = elf_begin(inFd, ELF_C_READ, NULL), err1);

	FILE_CHECK(inPath, elf_kind(elf) == ELF_K_ELF, "not a ELF file", err2);
	LIBELF_CHECK(inPath, elf_Ehdr = elf32_getehdr(elf), err2);
	FILE_CHECK(inPath, elf_Ehdr->e_type == ET_EXEC, "not a ELF executable", err2);
	FILE_CHECK(inPath, elf_Ehdr->e_machine == EM_ARM, "not an ELF for ARM architecture", err2);

	LIBELF_CHECK(inPath, elf_getphdrnum(elf, &numPhdr) >= 0, err2);
	FILE_CHECK(inPath, numPhdr == 1, "invalid number of program headers (is the binary statically linked?)", err2);

	LIBELF_CHECK(inPath, elf_Phdr = elf32_getphdr(elf), err2);
	FILE_CHECK(inPath, elf_Phdr[0].p_type == PT_LOAD, "program header not of PT_LOAD type", err2);
	FILE_CHECK(inPath, elf_Phdr[0].p_filesz <= OSROM_DATA_SIZE, "program too big", err2);
	FILE_CHECK(inPath, elf_Phdr[0].p_memsz <= OSROM_DATA_SIZE, "program too big", err2);
	LIBELF_CHECK(inPath, elf_Data = elf_getdata_rawchunk(elf, elf_Phdr[0].p_offset, elf_Phdr[0].p_filesz, ELF_T_PHDR), err2);

	/* Build OSROM image in memory */
	memset(osrom_Hdr, 0, sizeof(OSROM_Hdr));
	osrom_Hdr->e_entry = elf_Ehdr->e_entry;
	osrom_Hdr->e_size = OSROM_SIZE;
	osrom_Hdr->e_size_copy = OSROM_SIZE;
	osrom_Hdr->e_loadaddr = elf_Phdr[0].p_paddr;
	memcpy(osrom_Hdr->e_ident, osHdr_ident, sizeof(osHdr_ident));

	memset(osrom + OSROM_DATA_OFFSET, 0x00, elf_Phdr[0].p_memsz);
	memset(osrom + OSROM_DATA_OFFSET + elf_Phdr[0].p_memsz, 0xFF, OSROM_DATA_SIZE - elf_Phdr[0].p_memsz);
	memcpy(osrom + OSROM_DATA_OFFSET, elf_Data->d_buf, elf_Phdr[0].p_filesz);

	status = 0;
err2:
	elf_end(elf);
err1:
	close(inFd);
err0:
	return status;
}

/*
 * Convert an ELF file to OSROM.
 */
int elf2osrom(char *inPath, char *outPath) {
	int status = -1;

	char osrom[OSROM_SIZE];

	if (load_elf(inPath, osrom) < 0) {
		goto err0;
	}

	/* Open OSROM for writing */
	int outFd = open(outPath, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if (outFd < 0) {
		perror(outPath);
		goto err0;
	}

	if (write(outFd, osrom, OSROM_SIZE) != OSROM_SIZE) {
		perror(outPath);
		goto err1;
	}

	status = 0;

err1:
	close(outFd);
err0:
	return status;
}

/*
 * Convert an OSROM file to ELF.
 */
int osrom2elf(char *inPath, char *outPath) {
	int status = -1;

	/* Open OSROM for reading */
	char osrom[OSROM_SIZE];
	OSROM_Hdr *osrom_Hdr = (OSROM_Hdr*)osrom;

	if (load_osrom(inPath, osrom) < 0)
		goto err0;

	/* Open ELF file for writing */
	int outFd = open(outPath, O_WRONLY | O_CREAT, 0777);
	if (outFd < 0) {
		perror(outPath);
		goto err0;
	}

	Elf *elf;
	Elf32_Ehdr *elf_Ehdr;
	Elf32_Phdr *elf_Phdr;
	Elf_Scn *elf_Scn, *elf_ScnStr;
	Elf_Data *elf_Data, *elf_DataStr;
	Elf32_Shdr *elf_ShdrData, *elf_ShdrDataStr;

	/* Create ELF in-memory file */
	LIBELF_CHECK(inPath, elf = elf_begin(outFd, ELF_C_WRITE, NULL), err1);

	/* Create ELF header */
	LIBELF_CHECK(inPath, elf_Ehdr = elf32_newehdr(elf), err2);

	elf_Ehdr->e_ident[EI_DATA] = ELFDATA2LSB;
	elf_Ehdr->e_machine = EM_ARM;
	elf_Ehdr->e_type = ET_EXEC;
	elf_Ehdr->e_version = 1;
	elf_Ehdr->e_entry = osrom_Hdr->e_entry;

	/*
	 * Payload section
	 */
	LIBELF_CHECK(inPath, elf_Phdr = elf32_newphdr(elf, 1), err2);
	LIBELF_CHECK(inPath, elf_Scn = elf_newscn(elf), err2);
	LIBELF_CHECK(inPath, elf_Data = elf_newdata(elf_Scn), err2);

	elf_Data->d_align = 0;
	elf_Data->d_off = 0;
	elf_Data->d_buf = osrom + OSROM_DATA_OFFSET;
	elf_Data->d_type = ELF_T_BYTE;
	elf_Data->d_size = osrom_Hdr->e_size - OSROM_DATA_OFFSET;
	elf_Data->d_version = 1;

	LIBELF_CHECK(inPath, elf_ShdrData = elf32_getshdr(elf_Scn), err2);
	elf_ShdrData->sh_name = 1;
	elf_ShdrData->sh_type = SHT_PROGBITS;
	elf_ShdrData->sh_flags = SHF_WRITE | SHF_ALLOC | SHF_EXECINSTR;
	elf_ShdrData->sh_addr = osrom_Hdr->e_loadaddr;

	/*
	 * Strings table
	 */
	LIBELF_CHECK(inPath, elf_ScnStr = elf_newscn(elf), err2);
	LIBELF_CHECK(inPath, elf_DataStr = elf_newdata(elf_ScnStr), err2);

	const char shstrtab[19] = "\x00PAYLOAD\x00.shstrtab";
	elf_DataStr->d_align = 0;
	elf_DataStr->d_off = 0;
	elf_DataStr->d_buf = (void*)shstrtab;
	elf_DataStr->d_type = ELF_T_BYTE;
	elf_DataStr->d_size = sizeof(shstrtab);
	elf_DataStr->d_version = 1;

	LIBELF_CHECK(inPath, elf_ShdrDataStr = elf32_getshdr(elf_ScnStr), err2);
	elf_ShdrDataStr->sh_name = 9;
	elf_ShdrDataStr->sh_type = SHT_STRTAB;
	elf_ShdrDataStr->sh_flags = 0;
	elf_ShdrDataStr->sh_addr = 0x0;

	elf_Ehdr->e_shstrndx = elf_ndxscn(elf_ScnStr);

	LIBELF_CHECK(inPath, elf_update(elf, ELF_C_NULL) >= 0, err2);

	/* Write LOAD program header */
	elf_Phdr->p_type = PT_LOAD;
	elf_Phdr->p_flags = PF_R | PF_W | PF_X;
	elf_Phdr->p_offset = elf_ShdrData->sh_offset;
	elf_Phdr->p_vaddr = osrom_Hdr->e_loadaddr;
	elf_Phdr->p_paddr = osrom_Hdr->e_loadaddr;
	elf_Phdr->p_filesz = elf_Data->d_size;
	elf_Phdr->p_memsz = elf_Data->d_size;

	/* Write to file */
	elf_flagphdr(elf, ELF_C_SET, ELF_F_DIRTY);

	LIBELF_CHECK(inPath, elf_update(elf, ELF_C_WRITE) > 0, err2);

	status = 0;

err2:
	elf_end(elf);
err1:
	close(outFd);
err0:
	return status;
}

/*
 * Get conversion mode from utility name.
 */
conv_mode get_conv_mode(char *argv0, char **base_name) {
        char *dup = strdup(argv0);
        char *base = basename(dup);

        conv_mode mode = CONV_UNKNOWN;
	*base_name = NULL;

        if (strcmp(base, "elf2osrom") == 0) {
		mode = CONV_ELF2OSROM;
		*base_name = "elf2osrom";
	}
        else if (strcmp(base, "osrom2elf") == 0) {
		mode = CONV_OSROM2ELF;
		*base_name = "osrom2elf";
	}

        free(dup);
        return mode;
}

int main(int argc, char *argv[]) {
	int status = 1;
	char *basename;

	conv_mode mode = get_conv_mode(argv[0], &basename);
	if (mode == CONV_UNKNOWN) {
		fprintf(stderr, "utility must be named elf2osrom or osrom2elf\n");
		goto err0;
	}

	if (argc != 3) {
		fprintf(stderr, "usage: %s in out\n", basename);
		goto err0;
	}

	if (elf_version(1) == EV_NONE) {
		ERR_LIBELF();
		goto err0;
	}

        switch (mode) {
	case CONV_ELF2OSROM:
		status = elf2osrom(argv[1], argv[2]) < 0 ? 1 : 0;
		break;
	case CONV_OSROM2ELF:
		status = osrom2elf(argv[1], argv[2]) < 0 ? 1 : 0;
		break;
	default:
		break;
	}

err0:
	return status;
}
