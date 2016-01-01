#define ERR_LIBELF() do { \
	fprintf(stderr, "libelf: %s\n", elf_errmsg(elf_errno())); \
} while (0)

#define ERR_LIBELF_FILE(file) do { \
	fprintf(stderr, "%s: %s\n", file, elf_errmsg(elf_errno())); \
} while (0)

#define ERR_FILE(file, err) do { \
	fprintf(stderr, "%s: %s\n", file, err); \
} while (0)

#define POSIX_CHECK(file, a, go) FILE_CHECK(file, a, strerror(errno), go)

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
