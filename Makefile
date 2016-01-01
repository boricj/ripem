CFLAGS = -Wall

ARM_CC = arm-none-eabi-gcc
ARM_AS = arm-none-eabi-as
ARM_LD = arm-none-eabi-gcc
ARM_AR = arm-none-eabi-ar
ARM_OBJCOPY = arm-none-eabi-objcopy
ARM_CFLAGS = -Os -nostdlib -Wall -W -Wno-attributes -marm -mcpu=arm926ej-s -s -ffreestanding -std=gnu1x -Ilibhpbsp/ -Ilibhputils/ -Ilibgdbstub/
ARM_AFLAGS =
ARM_LDFLAGS = -nostdlib -ffreestanding -s -n

ELF2OSROM = tools/elf2osrom
OSROM2ELF = tools/osrom2elf
UPLOAD_ELF = tools/upload_elf

TOOLS = $(ELF2OSROM) $(OSROM2ELF) $(UPLOAD_ELF)

LIBHPBSP = libhpbsp/libhpbsp.a
LIBHPBSP_SRC = $(wildcard libhpbsp/*.c) $(wildcard libhpbsp/*.S)
LIBHPBSP_OBJ = $(patsubst libhpbsp/%.c,libhpbsp/%.o,$(patsubst libhpbsp/%.S,libhpbsp/%.o,$(LIBHPBSP_SRC)))

LIBHPUTILS = libhputils/libhputils.a
LIBHPUTILS_SRC = $(wildcard libhputils/*.c) $(wildcard libhputils/*.S)
LIBHPUTILS_OBJ = $(patsubst libhputils/%.c,libhputils/%.o,$(patsubst libhputils/%.S,libhputils/%.o,$(LIBHPUTILS_SRC)))

LIBGDBSTUB = libgdbstub/libgdbstub.a
LIBGDBSTUB_SRC = $(wildcard libgdbstub/*.c) $(wildcard libgdbstub/*.S)
LIBGDBSTUB_OBJ = $(patsubst libgdbstub/%.c,libgdbstub/%.o,$(patsubst libgdbstub/%.S,libgdbstub/%.o,$(LIBGDBSTUB_SRC)))

RIPEM_ELF = ripem/ripem.elf
RIPEM_ROM = ripem/ripem.rom
RIPEM_SRC = $(wildcard ripem/*.c) $(wildcard ripem/*.S)
RIPEM_OBJ = $(patsubst ripem/%.c,ripem/%.o,$(patsubst ripem/%.S,ripem/%.o,$(RIPEM_SRC)))
RIPEM_PAYLOAD_OBJ = ripem/ripem_payload.o

DUMMY_ELF = dummy/dummy.elf
DUMMY_SRC = $(wildcard dummy/*.c) $(wildcard dummy/*.S)
DUMMY_OBJ = $(patsubst dummy/%.c,dummy/%.o,$(patsubst dummy/%.S,dummy/%.o,$(DUMMY_SRC)))

#
# >>> Select your payload here <<<
#
RIPEM_PAYLOAD = $(DUMMY_ELF)
#RIPEM_PAYLOAD = prime_os.elf

.PHONY: all clean dist

all: $(TOOLS) $(RIPEM_ROM) $(DUMMY_ELF)

clean:
	rm -f $(LIBHPBSP) $(LIBHPBSP_OBJ)
	rm -f $(LIBHPUTILS) $(LIBHPUTILS_OBJ)
	rm -f $(LIBGDBSTUB) $(LIBGDBSTUB_OBJ)
	rm -f $(RIPEM_ELF) $(RIPEM_ROM) $(RIPEM_OBJ) $(RIPEM_PAYLOAD_OBJ)
	rm -f $(DUMMY_ELF) $(DUMMY_OBJ)
	rm -f $(TOOLS)

#
# tools
#

$(OSROM2ELF): tools/osrom2elf.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lelf

$(UPLOAD_ELF): tools/upload_elf.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lelf

$(ELF2OSROM): $(OSROM2ELF)
	ln -sf osrom2elf $@

#
# libhpbsp
#

$(LIBHPBSP): $(LIBHPBSP_OBJ)
	$(ARM_AR) rcs $@ $^

libhpbsp/%.o: libhpbsp/%.c
	$(ARM_CC) $(ARM_CFLAGS) -c $< -o $@

libhpbsp/%.o: libhpbsp/%.S
	$(ARM_AS) $(ARM_AFLAGS) $< -o $@

#
# libhputils
#

$(LIBHPUTILS): $(LIBHPUTILS_OBJ)
	$(ARM_AR) rcs $@ $^

libhputils/%.o: libhputils/%.c
	$(ARM_CC) $(ARM_CFLAGS) -c $< -o $@

libhputils/%.o: libhputils/%.S
	$(ARM_AS) $(ARM_AFLAGS) $< -o $@


#
# libgdbstub
#

$(LIBGDBSTUB): $(LIBGDBSTUB_OBJ)
	$(ARM_AR) rcs $@ $^

libgdbstub/%.o: libgdbstub/%.c
	$(ARM_CC) $(ARM_CFLAGS) -c $< -o $@

libgdbstub/%.o: libgdbstub/%.S
	$(ARM_AS) $(ARM_AFLAGS) $< -o $@

#
# RipEm
#

$(RIPEM_ROM): $(RIPEM_ELF) $(OSROM2ELF)
	$(ELF2OSROM) $< $@

$(RIPEM_ELF): $(RIPEM_OBJ) $(RIPEM_PAYLOAD_OBJ) $(LIBGDBSTUB) $(LIBHPBSP) $(LIBHPUTILS)
	$(ARM_LD) $(ARM_LDFLAGS) -T ripem/ldscript $^ -lgcc -o $@ --entry 0x30000020

ripem/%.o: ripem/%.c
	$(ARM_CC) $(ARM_CFLAGS) -c $< -o $@

ripem/%.o: ripem/%.S
	$(ARM_AS) $(ARM_AFLAGS) $< -o $@

$(RIPEM_PAYLOAD_OBJ): $(RIPEM_PAYLOAD)
	cp -f $(RIPEM_PAYLOAD) payload
	$(ARM_OBJCOPY) -I binary -O elf32-littlearm -B arm --rename-section .data=.payload payload ripem_payload.o
	rm payload
	mv -f ripem_payload.o $(RIPEM_PAYLOAD_OBJ)

#
# Dummy payload
#

$(DUMMY_ELF): $(DUMMY_OBJ) $(LIBHPBSP) $(LIBHPUTILS)
	$(ARM_LD) $(ARM_LDFLAGS) -T dummy/ldscript $^ -lgcc -o $@ --entry 0x30000000

dummy/%.o: dummy/%.c
	$(ARM_CC) $(ARM_CFLAGS) -c $< -o $@

dummy/%.o: dummy/%.S
	$(ARM_AS) $(ARM_AFLAGS) $< -o $@
