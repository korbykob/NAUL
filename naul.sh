#!/bin/bash
set -e

clean()
{
    rm -r os/bin
    rm -r programs/programs/shell/bin
    rm -r programs/programs/test/bin
    rm naul.iso
}

build()
{
    if [ ! -f "os/gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o" ]; then
        make -C os/gnu-efi
    fi

    cflags="-Iinclude -ffreestanding -fno-stack-protector -fno-stack-check -mno-red-zone -maccumulate-outgoing-args -g -mavx2 -O2 -fvect-cost-model=dynamic -fno-omit-frame-pointer"
    bootloaderCflags="$cflags -Ios/include -fpic -c -fshort-wchar -Ios/gnu-efi/inc"
    kernelCflags="$cflags -Ios/include -fpic -c -nostdinc"
    programCflags="$cflags -Iprograms/include -static -fno-pic -fno-pie -mcmodel=large -c -nostdinc"

    lflags="-znoexecstack"
    kernelLflags="$lflags -shared -Bsymbolic -Los/gnu-efi/x86_64/lib -Los/gnu-efi/x86_64/gnuefi -Tos/gnu-efi/gnuefi/elf_x86_64_efi.lds os/gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o"
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
    gcc $kernelCflags os/src/tty.c -o os/bin/tty.o
    gcc $kernelCflags os/src/terminal.c -o os/bin/terminal.o
    gcc $kernelCflags os/src/processes.c -o os/bin/processes.o
    gcc $kernelCflags os/src/kernel.c -o os/bin/kernel.o

    ld $kernelLflags \
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
    os/bin/tty.o \
    os/bin/terminal.o \
    os/bin/processes.o \
    os/bin/kernel.o \
    -o os/bin/naul.so -lgnuefi -lefi

    nm os/bin/naul.so > os/bin/naul.sym

    objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 os/bin/naul.so os/bin/naul.efi

    mkdir -p programs/programs/shell/bin
    gcc $programCflags programs/programs/shell/src/shell.c -o programs/programs/shell/bin/shell.o
    nm programs/programs/shell/bin/shell.o > programs/programs/shell/bin/shell.sym
    ld $programLflags programs/programs/shell/bin/shell.o -o programs/programs/shell/bin/shell.bin

    mkdir -p programs/programs/test/bin
    gcc $programCflags programs/programs/test/src/test.c -o programs/programs/test/bin/test.o
    nm programs/programs/test/bin/test.o > programs/programs/test/bin/test.sym
    ld $programLflags programs/programs/test/bin/test.o -o programs/programs/test/bin/test.bin
}

commands()
{
    bear --output .vscode/compile_commands.json -- ./naul.sh build
    sed -i 's|'$PWD'|${workspaceFolder}|g' .vscode/compile_commands.json
}

iso()
{
    dd if=/dev/zero of=naul.iso bs=1M count=8 status=none
    mformat -i naul.iso ::

    mmd -i naul.iso ::/efi
    mmd -i naul.iso ::/efi/boot
    mcopy -i naul.iso os/bin/naul.efi ::/efi/boot/bootx64.efi

    mmd -i naul.iso ::/naul
    mcopy -i naul.iso os/bin/naul.sym ::/naul/naul.sym
    mcopy -i naul.iso os/font.psf ::/naul/font.psf

    mmd -i naul.iso ::/programs
    mmd -i naul.iso ::/programs/shell
    mcopy -i naul.iso programs/programs/shell/bin/shell.bin ::/programs/shell/shell.bin
    mcopy -i naul.iso programs/programs/shell/bin/shell.sym ::/programs/shell/shell.sym

    mmd -i naul.iso ::/programs/test
    mcopy -i naul.iso programs/programs/test/bin/test.bin ::/programs/test/test.bin
    mcopy -i naul.iso programs/programs/test/bin/test.sym ::/programs/test/test.sym
}

run()
{
    build
    iso
    clear

    qemu-system-x86_64 -enable-kvm -bios OVMF-pure-efi.fd -cdrom naul.iso -m 4G -cpu host -serial null -serial null -serial stdio -display sdl
}

usage()
{
    echo "Usage:"
    echo "    ./naul.sh [command]"
    echo
    echo "Commands:"
    echo "    clean: Remove the build files"
    echo "    build: Generate the build files"
    echo "    commands: Generate compile_commands.json"
    echo "    iso: Create an iso from build"
    echo "    run: Build, create iso and run in QEMU"
}

case "$1" in
    clean) clean ;;
    build) build ;;
    commands) commands ;;
    iso) iso ;;
    run) run ;;
    *) usage ;;
esac
