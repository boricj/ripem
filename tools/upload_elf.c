#include <errno.h>
#include <fcntl.h>
#include <libelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "libelf_check_macros.h"

#define GDB_PACKET_SIZE 256

#define TRY_AGAIN do { \
	usleep(1000000 / baud_rate * 16); \
	tcflush(fd_tty, TCIFLUSH); \
	goto retry; \
} while(0)

static int baud_rate, verbose;

typedef struct {
	size_t opt_comspeed;
	int opt_run;
	Elf32_Addr opt_entry;

	const char *elf_filename;
	const char *tty_filename;
} upload_elf_args;

void usage(const char *argv0)
{
	printf("Usage: %s [-e entry] [-f executable.elf] [-r] [-s bauds] [-v] serial_port\n", argv0);
}

speed_t bauds_to_speed(int baud)
{
	int bauds[] = { 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400,
	                4800,  9600, 19200, 38400, 57600, 115200, 230400 } ;
	speed_t speeds[] = { B50, B75, B110, B134, B150, B200, B300, B600,
		             B1200, B1800, B2400, B4800, B9600, B19200, B38400,
			     B57600, B115200, B230400 } ;

	for (int i = 0; i < (sizeof(bauds) / sizeof(int)); i++) {
		if (bauds[i] == baud)
			return speeds[i];
	}

	return B0;
}

int parse_args(int argc, char *argv[], upload_elf_args *args)
{
	int opt;

	baud_rate = 115200;
	verbose = 0;

	args->opt_comspeed = B115200;
	args->opt_run = 0;
	args->opt_entry = 0xFFFFFFFF;
	args->elf_filename = NULL;
	args->tty_filename = NULL;

	/* Parse arguments. */
	while ((opt = getopt(argc, argv, "e:f:rs:v")) != -1) {
		switch (opt) {
		case 'e':
			/* Entry point. */
			if (sscanf(optarg, "%i", &args->opt_entry) != 1) {
				usage(argv[0]);
				return -1;
			}

			break;

		case 'f':
			/* ELF file to upload. */
			args->elf_filename = optarg;
			break;

		case 'r':
			/* Execute program after upload. */
			args->opt_run = 1;
			break;

		case 's':
			/* Set speed in bauds. */
			if (sscanf(optarg, "%d", &baud_rate) != 1) {
				usage(argv[0]);
				return -1;
			}

			args->opt_comspeed = bauds_to_speed(baud_rate);
			if (args->opt_comspeed == B0) {
				fprintf(stderr, "Unknown baud rate");
				return -1;
			}

			break;

		case 'v':
			/* Verbose. */
			verbose = 1;
			break;

		default:
			usage(argv[0]);
			return -1;
		}
	}

	if (optind + 1 != argc) {
		usage(argv[0]);
		return -1;
		args->tty_filename = argv[optind+1];
	}

	args->tty_filename = argv[optind];

	return 0;
}

uint8_t compute_chksum(uint8_t chksum, void *data)
{
	uint8_t *ptr = data;

	while (*ptr)
		chksum += *ptr++;

	return chksum;
}

void print_progress(int bytes_out, int bytes_total)
{
	if (verbose == 1)
		return;

	printf("\r[");

	for (int i = 0; i < 50; i++) {
		if (i <= (bytes_out * 50 / bytes_total))
			putc('=', stdout);
		else
			putc(' ', stdout);
	}

	printf("] %8d KiB/%d KiB", bytes_out/1024, bytes_total/1024);
}

void gdb_send_packet(int fd_tty, char *buffer)
{
	char chkdat[16];

	/* Send payload. */
	write(fd_tty, "$", 1);
	write(fd_tty, buffer, strlen(buffer));

	/* Send checksum. */
	sprintf(chkdat, "#%02x", compute_chksum(0, buffer));

	if (verbose)
		printf(" > : $%s%s\n", buffer, chkdat);

	write(fd_tty, chkdat, strlen(chkdat));
}

int gdb_check_ack(int fd_tty)
{
	char c;

	read(fd_tty, &c, 1);

	if (verbose)
		printf("ACK: %c\n", c);

	if (c == '+')
		return 0;

	return -1;
}

int gdb_receive_packet(int fd_tty, char *buffer)
{
	char c, *ptr = buffer;

	char chkdat[3];
	int chksum;

	read(fd_tty, &c, 1);

	if (c != '$')
		return -1;

	do {
		read(fd_tty, &c, 1);

		if (c == '#') {
			*ptr++ = 0;
			break;
		}

		*ptr++ = c;
	} while(1);

	read(fd_tty, chkdat, 2);
	chkdat[2] = 0;

	if (verbose)
		printf(" < : $%s#%s\n", buffer, chkdat);

	if (sscanf(chkdat, "%x", &chksum) != 1)
		return -1;

 	if (chksum != compute_chksum(0, buffer))
 		return -1;

	return 0;
}

int gdb_write_mem(int fd_tty, Elf32_Addr addr, void *data, int len)
{
	if (len <= 0)
		return 0;

	char buffer[32+GDB_PACKET_SIZE*2];
	uint8_t *ptr = data;
	int len_header;

	/* Write packet metadata. */
	sprintf(buffer, "M%08x,%x:", addr, len);
	len_header = strlen(buffer);

	/* Write packet data. */
	for (int i = 0; i < len; i++)
		sprintf(buffer + len_header + 2*i, "%02x", *ptr++);

retry:
	gdb_send_packet(fd_tty, buffer);

	/* Check ACK and reply. */
	if (gdb_check_ack(fd_tty) < 0) {
		TRY_AGAIN;
	}

	if (gdb_receive_packet(fd_tty, buffer) < 0) {
		TRY_AGAIN;
	}

	if (buffer[0] != 'O' && buffer[1] != 'K') {
		TRY_AGAIN;
	}

	return 0;
}

int gdb_send_program_header(int fd_tty, Elf *elf, Elf32_Phdr *phdr)
{
	if (phdr->p_type != PT_LOAD) {
		fprintf(stderr, "Error: unknown program header type.\n");
		return -1;
	}

	Elf_Data *data = elf_getdata_rawchunk(elf, phdr->p_offset, phdr->p_filesz, ELF_T_BYTE);
	if (data == NULL)  {
		fprintf(stderr, "Error: couldn't retrieve program header data.\n");
		return -1;
	}

	/* Send data. */
	for (int cpt = 0; cpt <= phdr->p_filesz; cpt += GDB_PACKET_SIZE) {
		int size_send;

		if (phdr->p_filesz - cpt >= GDB_PACKET_SIZE)
			size_send = GDB_PACKET_SIZE;
		else
			size_send = phdr->p_filesz - cpt;

		if (gdb_write_mem(fd_tty, phdr->p_paddr+cpt, ((char*)data->d_buf)+cpt, size_send) < 0) {
			fprintf(stderr, "\nError while sending data.\n");
			return -1;
		}

		print_progress(cpt, phdr->p_memsz);
	}

	char zeroes[GDB_PACKET_SIZE*2];
	memset(zeroes, 0, GDB_PACKET_SIZE*2);

	/* Write zeroes. */
	for (int cpt = phdr->p_filesz; cpt <= phdr->p_memsz; cpt += GDB_PACKET_SIZE) {
		int size_send;

		if (phdr->p_filesz - cpt >= GDB_PACKET_SIZE)
			size_send = GDB_PACKET_SIZE;
		else
			size_send = phdr->p_filesz - cpt;

		if (gdb_write_mem(fd_tty, phdr->p_paddr+cpt, zeroes, size_send) < 0)
			return -1;

		print_progress(cpt, phdr->p_memsz);
	}

	return 0;
}

int gdb_send_entry(int fd_tty, Elf32_Addr addr)
{
	char buffer[512];
	char pc[9];
	int len_buffer;

retry:
	gdb_send_packet(fd_tty, "g");
	if (gdb_check_ack(fd_tty) < 0) {
		TRY_AGAIN;
	}

	if (gdb_receive_packet(fd_tty, buffer+1) < 0) {
		TRY_AGAIN;
	}

	buffer[0] = 'G';

	/* Set PC. */
	for (int j = 0; j < 4; j++)
		sprintf(pc + 2*j, "%02x", (addr >> (j*8)) & 0xFF);
	memcpy(buffer + 1 + 8*15, pc, 8);

	/* Clear checksum. */
	len_buffer = strlen(buffer);
	buffer[len_buffer-2] = 0;

	gdb_send_packet(fd_tty, buffer);

	/* Check ACK and reply. */
	if (gdb_check_ack(fd_tty) < 0) {
		TRY_AGAIN;
	}

	if (gdb_receive_packet(fd_tty, buffer) < 0) {
		TRY_AGAIN;
	}

	if (buffer[0] != 'O' && buffer[1] != 'K') {
		TRY_AGAIN;
	}

	return 0;
}

int gdb_send_continue(int fd_tty)
{
	/* Build packet metadata. */
	gdb_send_packet(fd_tty, "c");

	return 0;
}

int load_elf(const char *filename, int *fd_elf, Elf **elf, Elf32_Ehdr **ehdr, Elf32_Phdr **phdr)
{
	*fd_elf = open(filename, O_RDONLY);
	POSIX_CHECK(filename, *fd_elf >= 0, err);

	LIBELF_CHECK(filename, *elf = elf_begin(*fd_elf, ELF_C_READ, NULL), err);

	/* Check header. */
	FILE_CHECK(filename, elf_kind(*elf) == ELF_K_ELF, "not a ELF file", err);
	LIBELF_CHECK(filename, *ehdr = elf32_getehdr(*elf), err);
	FILE_CHECK(filename, (*ehdr)->e_type == ET_EXEC, "not a ELF executable", err);

	/* Check if all program headers are valid. */
	LIBELF_CHECK(filename, *phdr = elf32_getphdr(*elf), err);

	for (int i = 0; i < (*ehdr)->e_phnum; i++) {
		if (((*phdr)[i].p_type != PT_LOAD) && ((*phdr)[i].p_type != PT_GNU_STACK)) {
			fprintf(stderr, "Unknown program header entry #%d/, type=%x\n", i, (*phdr)[i].p_type);
			return -1;
		}
	}

	return 0;
err:
	return -1;
}

int open_tty(const char *filename, int *fd_tty, speed_t comspeed) {
	struct termios term;

	*fd_tty = open(filename, O_RDWR);
	POSIX_CHECK(filename, *fd_tty >= 0, err);

	/* Set baud rate. */
	POSIX_CHECK(filename, tcgetattr(*fd_tty, &term) >= 0, err);
	POSIX_CHECK(filename, cfsetispeed(&term, comspeed) >= 0, err);
	POSIX_CHECK(filename, cfsetospeed(&term, comspeed) >= 0, err);
	POSIX_CHECK(filename, tcsetattr(*fd_tty, TCSAFLUSH, &term) >= 0, err);

	cfmakeraw(&term);

	return 0;
err:
	return -1;
}

int main(int argc, char *argv[])
{
	int status = EXIT_FAILURE;
	int fd_elf = 0, fd_tty = 0;
	upload_elf_args args;

	Elf *elf = NULL;
	Elf32_Ehdr *ehdr = NULL;
	Elf32_Phdr *phdr = NULL;

	if (elf_version(1) == EV_NONE) {
		ERR_LIBELF();
		goto err0;
	}

	if (parse_args(argc, argv, &args) < 0)
		goto err0;

	if (open_tty(args.tty_filename, &fd_tty, args.opt_comspeed) < 0)
		goto err1;

	/* Upload ELF file. */
	if (args.elf_filename != NULL) {
		if (load_elf(args.elf_filename, &fd_elf, &elf, &ehdr, &phdr) < 0)
			goto err2;

		for (int i = 0; i < ehdr->e_phnum; i++) {
			if (phdr[i].p_type == PT_LOAD) {
				printf("Sending program header PT_LOAD (%d/%d)...\n", i+1, ehdr->e_phnum);
			}
			else if (phdr[i].p_type == PT_GNU_STACK) {
				printf("Skipping program header PT_GNU_STACK (%d/%d).\n", i+1, ehdr->e_phnum);
				continue;
			}

			if (gdb_send_program_header(fd_tty, elf, &phdr[i]) < 0)
				goto err2;

			putc('\n', stdout);
		}
	}

	/* Set entry point. */
	if ((args.opt_entry == 0xFFFFFFFF) && (args.elf_filename != NULL))
		args.opt_entry = ehdr->e_entry;

	if (args.opt_entry != 0xFFFFFFFF) {
		printf("Sending entry point 0x%08x...\n", args.opt_entry);
		if (gdb_send_entry(fd_tty, args.opt_entry) < 0) {
			fprintf(stderr, "Error when sending registers packet.\n");
			goto err2;
		}
	}

	/* Run code. */
	if (args.opt_run) {
		printf("Sending continue packet...\n");
		if (gdb_send_continue(fd_tty) < 0) {
			fprintf(stderr, "Error when sending continue packet.\n");
		}
	}

	status = EXIT_SUCCESS;
err2:
	if (elf)
		elf_end(elf);
	close(fd_elf);
err1:
	tcflush(fd_tty, TCIFLUSH);
	close(fd_tty);
err0:
	return status;
}
