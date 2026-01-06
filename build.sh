#!/bin/bash
set -e

if [ ! -f "os/gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o" ]; then
    make -C os/gnu-efi
fi

flags="-Iinclude -ffreestanding -fno-stack-protector -fno-stack-check -mno-red-zone -maccumulate-outgoing-args"
kernelFlags="$flags -Ios/include -fpic -c"
programFlags="$flags -Iprograms/include -fno-pic -fno-pie -mcmodel=large -static -no-pie -nostartfiles -Wl,-Tprograms/linker.ld,-znoexecstack"

mkdir -p os/bin
gcc $kernelFlags -fshort-wchar -Ios/gnu-efi/inc os/src/bootloader.c -o os/bin/bootloader.o
gcc $kernelFlags os/src/serial.c -o os/bin/serial.o
gcc $kernelFlags os/src/allocator.c -o os/bin/allocator.o
gcc $kernelFlags os/src/filesystem.c -o os/bin/filesystem.o
gcc $kernelFlags os/src/symbols.c -o os/bin/symbols.o
gcc $kernelFlags os/src/gdt.c -o os/bin/gdt.o
gcc $kernelFlags os/src/idt.c -o os/bin/idt.o
gcc $kernelFlags os/src/terminal.c -o os/bin/terminal.o
gcc $kernelFlags os/src/paging.c -o os/bin/paging.o
gcc $kernelFlags os/src/syscalls.c -o os/bin/syscalls.o
gcc $kernelFlags os/src/pic.c -o os/bin/pic.o
gcc $kernelFlags os/src/scheduler.c -o os/bin/scheduler.o
gcc $kernelFlags os/src/hpet.c -o os/bin/hpet.o
gcc $kernelFlags os/src/keyboard.c -o os/bin/keyboard.o
gcc $kernelFlags os/src/kernel.c -o os/bin/kernel.o

ld -shared -Bsymbolic -Los/gnu-efi/x86_64/lib -Los/gnu-efi/x86_64/gnuefi -Tos/gnu-efi/gnuefi/elf_x86_64_efi.lds -znoexecstack os/gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o \
os/bin/bootloader.o \
os/bin/serial.o \
os/bin/allocator.o \
os/bin/filesystem.o \
os/bin/symbols.o \
os/bin/gdt.o \
os/bin/idt.o \
os/bin/terminal.o \
os/bin/paging.o \
os/bin/syscalls.o \
os/bin/pic.o \
os/bin/scheduler.o \
os/bin/hpet.o \
os/bin/keyboard.o \
os/bin/kernel.o \
-o os/bin/os.so -lgnuefi -lefi
objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 os/bin/os.so os/bin/os.efi

mkdir -p programs/bin/test
gcc $programFlags programs/src/test/test.c -o programs/bin/test/test.bin