#!/bin/bash
set -e

if [ ! -f "os/gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o" ]; then
    make -C os/gnu-efi
fi

cflags="-Iinclude -ffreestanding -fno-stack-protector -fno-stack-check -mno-red-zone -maccumulate-outgoing-args -g -mavx2 -O3 -fno-omit-frame-pointer"
bootloaderCflags="$cflags -Ios/include -fpic -c -fshort-wchar -Ios/gnu-efi/inc"
kernelCflags="$cflags -Ios/include -fpic -c -nostdinc"
programCflags="$cflags -Iprograms/include -static -fno-pic -fno-pie -mcmodel=large -c -nostdinc"

lflags="-znoexecstack"
kernellLflags="$lflags -shared -Bsymbolic -Los/gnu-efi/x86_64/lib -Los/gnu-efi/x86_64/gnuefi -Tos/gnu-efi/gnuefi/elf_x86_64_efi.lds os/gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o"
programLflags="$lflags -Tprograms/linker.ld -no-pie"

mkdir -p os/bin
gcc $bootloaderCflags os/src/bootloader.c -o os/bin/bootloader.o
gcc $kernelCflags os/src/allocator.c -o os/bin/allocator.o
gcc $kernelCflags os/src/gdt.c -o os/bin/gdt.o
gcc $kernelCflags os/src/idt.c -o os/bin/idt.o
gcc $kernelCflags os/src/filesystem.c -o os/bin/filesystem.o
gcc $kernelCflags os/src/symbols.c -o os/bin/symbols.o
gcc $kernelCflags os/src/panic.c -o os/bin/panic.o
gcc $kernelCflags os/src/paging.c -o os/bin/paging.o
gcc $kernelCflags os/src/syscalls.c -o os/bin/syscalls.o
gcc $kernelCflags os/src/pic.c -o os/bin/pic.o
gcc $kernelCflags os/src/hpet.c -o os/bin/hpet.o
gcc $kernelCflags os/src/scheduler.c -o os/bin/scheduler.o
gcc $kernelCflags os/src/keyboard.c -o os/bin/keyboard.o
gcc $kernelCflags os/src/mouse.c -o os/bin/mouse.o
gcc $kernelCflags os/src/display.c -o os/bin/display.o
gcc $kernelCflags os/src/terminal.c -o os/bin/terminal.o
gcc $kernelCflags os/src/processes.c -o os/bin/processes.o
gcc $kernelCflags os/src/kernel.c -o os/bin/kernel.o

ld $kernellLflags \
os/bin/bootloader.o \
os/bin/allocator.o \
os/bin/gdt.o \
os/bin/idt.o \
os/bin/filesystem.o \
os/bin/symbols.o \
os/bin/panic.o \
os/bin/paging.o \
os/bin/syscalls.o \
os/bin/pic.o \
os/bin/hpet.o \
os/bin/scheduler.o \
os/bin/keyboard.o \
os/bin/mouse.o \
os/bin/display.o \
os/bin/terminal.o \
os/bin/processes.o \
os/bin/kernel.o \
-o os/bin/os.so -lgnuefi -lefi
objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 os/bin/os.so os/bin/os.efi

nm os/bin/os.so > os/bin/symbols.txt

mkdir -p programs/bin/shell
gcc $programCflags programs/src/shell/shell.c -o programs/bin/shell/shell.o
ld $programLflags programs/bin/shell/shell.o -o programs/bin/shell/shell.bin

mkdir -p programs/bin/test
gcc $programCflags programs/src/test/test.c -o programs/bin/test/test.o
ld $programLflags programs/bin/test/test.o -o programs/bin/test/test.bin
