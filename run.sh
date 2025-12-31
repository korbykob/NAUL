#!/bin/bash
set -e
clear

flags="-Iinclude -Ios/include -fpic -ffreestanding -fno-stack-protector -fno-stack-check -mno-red-zone -maccumulate-outgoing-args -c"

mkdir -p os/bin
gcc $flags -fshort-wchar -Ios/gnu-efi/inc os/src/bootloader.c -o os/bin/bootloader.o
gcc $flags os/src/serial.c -o os/bin/serial.o
gcc $flags os/src/allocator.c -o os/bin/allocator.o
gcc $flags os/src/filesystem.c -o os/bin/filesystem.o
gcc $flags os/src/gdt.c -o os/bin/gdt.o
gcc $flags os/src/idt.c -o os/bin/idt.o
gcc $flags os/src/terminal.c -o os/bin/terminal.o
gcc $flags os/src/paging.c -o os/bin/paging.o
gcc $flags os/src/syscalls.c -o os/bin/syscalls.o
gcc $flags os/src/pic.c -o os/bin/pic.o
gcc $flags os/src/scheduler.c -o os/bin/scheduler.o
gcc $flags os/src/hpet.c -o os/bin/hpet.o
gcc $flags os/src/keyboard.c -o os/bin/keyboard.o
gcc $flags os/src/kernel.c -o os/bin/kernel.o

ld -shared -Bsymbolic -Los/gnu-efi/x86_64/lib -Los/gnu-efi/x86_64/gnuefi -Tos/gnu-efi/gnuefi/elf_x86_64_efi.lds -znoexecstack os/gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o \
os/bin/bootloader.o \
os/bin/serial.o \
os/bin/allocator.o \
os/bin/filesystem.o \
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
gcc -Iinclude -Iprograms/include -static -fno-pic -fno-pie -mcmodel=large -ffreestanding -fno-stack-protector -fno-stack-check -mno-red-zone -maccumulate-outgoing-args -c programs/src/test/test.c -o programs/bin/test/test.o
ld -Tprograms/linker.ld -no-pie -znoexecstack programs/bin/test/test.o -o programs/bin/test/test.bin

uefi-run -b OVMF-pure-efi.fd -d -f os/font.psf -f programs/bin/test/test.bin os/bin/os.efi -- -enable-kvm -m 16G -cpu host -serial null -serial null -serial stdio -display sdl