#ifndef _GDB_STUB_H_
#define _GDB_STUB_H_

#define GDB_PACKET_BUFFER_LEN 2048

#include "gdb_stub.h"

#include "lib.h"
#include "serial.h"


void gdb_command_status(char *in, int inLen, char *out, int *outLen);
void gdb_command_thread(char *in, int inLen, char *out, int *outLen);
void gdb_command_unknown(char *in, int inLen, char *out, int *outLen);

void gdb_command_read_memory(char *in, int inLen, char *out, int *outLen);
void gdb_command_write_memory(char *in, int inLen, char *out, int *outLen);

void gdb_command_read_registers(char *in, int inLen, char *out, int *outLen);
void gdb_command_write_registers(char *in, int inLen, char *out, int *outLen);

int gdb_read_packet(char *in);
void gdb_write_packet(char *out, int len);

void gdb_mainloop(void);

#endif
