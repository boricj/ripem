CFLAGS = -Wall -Itools/

ARM_CFLAGS = -Os -nostdlib -Wall -W -Wno-attributes -marm -mcpu=arm926ej-s -s -ffreestanding -std=gnu1x
ARM_AFLAGS =
ARM_LDFLAGS = -nostdlib -ffreestanding -s -n

CROSS_COMPILER_TRIPLET = arm-none-eabi

#
# >>> Put your payloads here <<<
#
RIPEM_PAYLOAD = bin/dummy/dummy.elf bin/gdbstub/gdbstub_serial.elf
#RIPEM_PAYLOAD += prime_os.elf

#-------------------------------------------------------------------------------

ARM_CC = $(CROSS_COMPILER_TRIPLET)-gcc
ARM_AS = $(CROSS_COMPILER_TRIPLET)-as
ARM_LD = $(CROSS_COMPILER_TRIPLET)-gcc
ARM_AR = $(CROSS_COMPILER_TRIPLET)-ar
ARM_OBJCOPY = $(CROSS_COMPILER_TRIPLET)-objcopy

define SUBST_OBJ =
$(patsubst $(2)/%.c,$(2)/%.o,$(patsubst $(2)/%.S,$(2)/%.o,$(1)))
endef

define LIST_SRCS =
$(wildcard $(1)/*.c) $(wildcard $(1)/*.S)
endef

.PHONY: all build clean

all: build

include tools/*/Makefile.in
include lib/*/Makefile.in
include bin/*/Makefile.in

build: $(BUILD_FILES)

clean:
	@for i in $(CLEAN_FILES) ; do \
		echo rm -f $$i ; \
		rm -f $$i ; \
	done
	@for i in $(CLEAN_DIRS) ; do \
		echo rm -rf $$i ; \
		rm -rf $$i ; \
	done
