CFLAGS = -Wall
LDFLAGS = -lelf

ARM_CC = arm-none-eabi-gcc
ARM_AS = arm-none-eabi-as
ARM_LD = arm-none-eabi-gcc
ARM_AR = arm-none-eabi-ar
ARM_CFLAGS = -Os -nostdlib -Wall -W -Wno-attributes -marm -mcpu=arm926ej-s -s -ffreestanding -std=gnu1x -Ilibhpbsp/ -Ilibhputils/
ARM_AFLAGS =
ARM_LDFLAGS = -nostdlib -ffreestanding -s -n

ELF2OSROM = tools/elf2osrom
OSROM2ELF = tools/osrom2elf

TOOLS = $(ELF2OSROM) $(OSROM2ELF)

LIBHPBSP = libhpbsp/libhpbsp.a
LIBHPBSP_SRC = $(wildcard libhpbsp/*.c) $(wildcard libhpbsp/*.S)
LIBHPBSP_OBJ = $(patsubst libhpbsp/%.c,libhpbsp/%.o,$(patsubst libhpbsp/%.S,libhpbsp/%.o,$(LIBHPBSP_SRC)))

LIBHPUTILS = libhputils/libhputils.a
LIBHPUTILS_SRC = $(wildcard libhputils/*.c) $(wildcard libhputils/*.S)
LIBHPUTILS_OBJ = $(patsubst libhputils/%.c,libhputils/%.o,$(patsubst libhputils/%.S,libhputils/%.o,$(LIBHPUTILS_SRC)))

RIPEM_ELF = ripem/ripem.elf
RIPEM_ROM = ripem/ripem.rom
RIPEM_SRC = $(wildcard ripem/*.c) $(wildcard ripem/*.S)
RIPEM_OBJ = $(patsubst ripem/%.c,ripem/%.o,$(patsubst ripem/%.S,ripem/%.o,$(RIPEM_SRC)))

.PHONY: all clean dist

all: $(TOOLS) $(RIPEM_ROM)

clean:
	rm -f $(LIBHPBSP) $(LIBHPBSP_OBJ)
	rm -f $(LIBHPUTILS) $(LIBHPUTILS_OBJ)
	rm -f $(RIPEM_ELF) $(RIPEM_ROM) $(RIPEM_OBJ)
	rm -f $(TOOLS)

#
# tools
#

$(OSROM2ELF): tools/osrom2elf.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

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
# RipEm
#

$(RIPEM_ROM): $(RIPEM_ELF) $(OSROM2ELF)
	$(ELF2OSROM) $< $@

$(RIPEM_ELF): $(RIPEM_OBJ) $(LIBHPUTILS) $(LIBHPBSP)
	$(ARM_LD) $(ARM_LDFLAGS) -T ripem/ldscript $^ -lgcc -o $@

ripem/%.o: ripem/%.c
	$(ARM_CC) $(ARM_CFLAGS) -c $< -o $@

ripem/%.o: ripem/%.S
	$(ARM_AS) $(ARM_AFLAGS) $< -o $@
