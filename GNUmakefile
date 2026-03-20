CONFIG_ARCH := x86_64
KERNEL   := Petrichor
HDD      := $(KERNEL).hdd
ISO_ROOT := iso_root

CARCHFILES  := $(wildcard kernel/arch/$(CONFIG_ARCH)/*/*.c)
CFILES      := $(wildcard kernel/*/*.c)
ASMFILES    := $(wildcard kernel/arch/$(CONFIG_ARCH)/*/*.asm)
SFILES      := $(wildcard kernel/arch/$(CONFIG_ARCH)/*/*.S)

OBJ         := $(CARCHFILES:.c=.o) $(CFILES:.c=.o) $(ASMFILES:.asm=.o) $(SFILES:.S=.o)
HEADER_DEPS := $(CARCHFILES:.c=.d) $(CFILES:.c=.d) $(ASMFILES:.asm=.d) $(SFILES:.S=.d)

ifeq ($(CONFIG_ARCH),x86_64)
    CC := x86_64-elf-gcc
    AS := nasm
    LD := x86_64-elf-gcc

    CFLAGS := -Wall -Wextra -O2 -pipe \
              -I kernel/include/ \
              -I kernel/arch/$(CONFIG_ARCH)/include/ \
              -I limine/

    ASFLAGS := -g -MD -MP

    INTERNALLDFLAGS := \
        -T kernel/arch/$(CONFIG_ARCH)/linker.ld \
        -nostdlib \
        -mcmodel=kernel \
        -zmax-page-size=0x1000 \
        -static \
        -pie \
        -ztext

    INTERNALCFLAGS := \
        -I. \
        -std=gnu11 \
        -ffreestanding \
        -fno-stack-protector \
        -fpie \
        -masm=intel \
        -mno-80387 \
        -mno-mmx \
        -mno-3dnow \
        -mno-sse \
        -mno-sse2 \
        -mno-red-zone \
        -MMD
endif

.PHONY: all clean hdd run

all: $(KERNEL)-$(CONFIG_ARCH).elf

$(KERNEL)-$(CONFIG_ARCH).elf: $(OBJ)
	$(LD) $(INTERNALLDFLAGS) $^ -o $@

-include $(HEADER_DEPS)

%.o: %.asm
	$(AS) $(ASFLAGS) -f elf64 $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c $< -o $@

%.o: %.S
	$(AS) $(ASFLAGS) -c $< -o $@

hdd: $(KERNEL)-$(CONFIG_ARCH).elf
	dd if=/dev/zero bs=1M count=0 seek=64 of=$(HDD)

	parted -s $(HDD) mklabel gpt
	parted -s $(HDD) mkpart ESP fat32 2048s 100%
	parted -s $(HDD) set 1 esp on

	sudo losetup -Pf --show $(HDD) > /tmp/loopdev
	sudo mkfs.fat -F 32 $$(cat /tmp/loopdev)p1
	mkdir -p /tmp/iesp
	sudo mount $$(cat /tmp/loopdev)p1 /tmp/iesp

	sudo mkdir -p /tmp/iesp/EFI/BOOT
	sudo mkdir -p /tmp/iesp/boot/limine
	sudo cp $(KERNEL)-$(CONFIG_ARCH).elf /tmp/iesp/boot/
	sudo cp limine.conf                   /tmp/iesp/boot/limine/
	sudo cp limine/BOOTX64.EFI          /tmp/iesp/EFI/BOOT/

	sudo umount /tmp/iesp
	sudo losetup -d $$(cat /tmp/loopdev)
	rm -f /tmp/loopdev
	rmdir /tmp/iesp

run: hdd
	qemu-system-x86_64 \
		-M q35 \
		-m 256M \
		-bios /usr/share/ovmf/x64/OVMF.4m.fd \
		-drive file=$(HDD),format=raw \
		-serial stdio

.PHONY: clean
clean:
	rm -rf *.elf *.hdd $(OBJ) $(HEADER_DEPS)
	find . -name "*.o.d" -delete