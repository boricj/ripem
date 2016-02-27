#define CPIO_MAGIC	"070701"
#define CPIO_CRC_MAGIC	"070702"

struct cpio_newc_header {
	char    c_magic[6];
	char    c_ino[8];
	char    c_mode[8];
	char    c_uid[8];
	char    c_gid[8];
	char    c_nlink[8];
	char    c_mtime[8];
	char    c_filesize[8];
	char    c_devmajor[8];
	char    c_devminor[8];
	char    c_rdevmajor[8];
	char    c_rdevminor[8];
	char    c_namesize[8];
	char    c_check[8];
} ;