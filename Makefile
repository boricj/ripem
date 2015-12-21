CFLAGS = -Wall
LDFLAGS = -lelf

ARM_CC = arm-none-eabi-gcc
ARM_AS = arm-none-eabi-as
ARM_LD = arm-none-eabi-gcc
ARM_CFLAGS = -Os -nostdlib -Wall -W -Wno-attributes -marm -mcpu=arm926ej-s -s -ffreestanding -std=gnu1x
ARM_AFLAGS =
ARM_LDFLAGS = -nostdlib -ffreestanding -s -n

ELF2OSROM = tools/elf2osrom
OSROM2ELF = tools/osrom2elf

TOOLS = $(ELF2OSROM) $(OSROM2ELF)

RIPEM_ELF = ripem/ripem.elf
RIPEM_ROM = ripem/ripem.rom
RIPEM_SRC = $(wildcard ripem/*.c) $(wildcard ripem/*.S)
RIPEM_OBJ = $(patsubst ripem/%.c,ripem/%.o,$(patsubst ripem/%.S,ripem/%.o,$(RIPEM_SRC)))

.PHONY: all clean dist

all: $(TOOLS) $(RIPEM_ROM)

clean:
	rm -f $(RIPEM_ELF) $(RIPEM_ROM) $(RIPEM_OBJ) $(TOOLS)

#
# tools
#

$(OSROM2ELF): tools/osrom2elf.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(ELF2OSROM): $(OSROM2ELF)
	ln -sf osrom2elf $@

#
# RipEm
#

$(RIPEM_ROM): $(RIPEM_ELF) $(OSROM2ELF)
	$(ELF2OSROM) $< $@

$(RIPEM_ELF): $(RIPEM_OBJ)
	$(ARM_LD) $(ARM_LDFLAGS) -T ripem/ldscript $^ -lgcc -o $@

ripem/%.o: ripem/%.c
	$(ARM_CC) $(ARM_CFLAGS) -c $< -o $@

ripem/%.o: ripem/%.S
	$(ARM_AS) $(ARM_AFLAGS) $< -o $@
