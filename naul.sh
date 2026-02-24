#!/bin/bash
set -e

clean()
{
    rm -r os/bin
    for program in programs/programs/*/; do
        cd $program
        ./clean.sh
        cd - > /dev/null
    done
    rm naul.iso
}

build()
{
    export COMPILER_FLAGS="-std=gnu17 -I$(pwd)/include -ffreestanding -fno-stack-protector -fno-stack-check -mno-red-zone -maccumulate-outgoing-args -g -mavx2 -O2 -fvect-cost-model=dynamic -fno-omit-frame-pointer"
    export BOOTLOADER_COMPILER_FLAGS="$COMPILER_FLAGS -Ios/include -fpic -c -fshort-wchar -Ios/gnu-efi/inc"
    export KERNEL_COMPILER_FLAGS="$COMPILER_FLAGS -Ios/include -fpic -c -nostdinc"
    export PROGRAM_COMPILER_FLAGS="$COMPILER_FLAGS -I$(pwd)/programs/include -static -fno-pic -fno-pie -mcmodel=large -c -nostdinc"

    export LINKER_FLAGS="-znoexecstack"
    export KERNEL_LINKER_FLAGS="$LINKER_FLAGS -shared -Bsymbolic -Los/gnu-efi/x86_64/lib -Los/gnu-efi/x86_64/gnuefi -Tos/gnu-efi/gnuefi/elf_x86_64_efi.lds os/gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o"
    export PROGRAM_LINKER_FLAGS="$LINKER_FLAGS -T$(pwd)/programs/linker.ld -no-pie"

    if [ ! -f "os/gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o" ]; then
        make -C os/gnu-efi
    fi

    mkdir -p os/bin
    gcc $BOOTLOADER_COMPILER_FLAGS os/src/bootloader.c -o os/bin/bootloader.o
    gcc $KERNEL_COMPILER_FLAGS os/src/allocator.c -o os/bin/allocator.o
    gcc $KERNEL_COMPILER_FLAGS os/src/gdt.c -o os/bin/gdt.o
    gcc $KERNEL_COMPILER_FLAGS os/src/idt.c -o os/bin/idt.o
    gcc $KERNEL_COMPILER_FLAGS os/src/filesystem.c -o os/bin/filesystem.o
    gcc $KERNEL_COMPILER_FLAGS os/src/symbols.c -o os/bin/symbols.o
    gcc $KERNEL_COMPILER_FLAGS os/src/panic.c -o os/bin/panic.o
    gcc $KERNEL_COMPILER_FLAGS os/src/paging.c -o os/bin/paging.o
    gcc $KERNEL_COMPILER_FLAGS os/src/syscalls.c -o os/bin/syscalls.o
    gcc $KERNEL_COMPILER_FLAGS os/src/pic.c -o os/bin/pic.o
    gcc $KERNEL_COMPILER_FLAGS os/src/hpet.c -o os/bin/hpet.o
    gcc $KERNEL_COMPILER_FLAGS os/src/scheduler.c -o os/bin/scheduler.o
    gcc $KERNEL_COMPILER_FLAGS os/src/keyboard.c -o os/bin/keyboard.o
    gcc $KERNEL_COMPILER_FLAGS os/src/mouse.c -o os/bin/mouse.o
    gcc $KERNEL_COMPILER_FLAGS os/src/display.c -o os/bin/display.o
    gcc $KERNEL_COMPILER_FLAGS os/src/tty.c -o os/bin/tty.o
    gcc $KERNEL_COMPILER_FLAGS os/src/terminal.c -o os/bin/terminal.o
    gcc $KERNEL_COMPILER_FLAGS os/src/processes.c -o os/bin/processes.o
    gcc $KERNEL_COMPILER_FLAGS os/src/kernel.c -o os/bin/kernel.o

    ld $KERNEL_LINKER_FLAGS \
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

    for program in programs/programs/*/; do
        cd $program
        ./build.sh
        cd - > /dev/null
    done
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
    for program in programs/programs/*/; do
        name=$(basename "$program")
        mmd -i naul.iso ::/programs/$name
        mcopy -i naul.iso ${program}bin/${name}.bin ::/programs/${name}/${name}.bin
        mcopy -i naul.iso ${program}bin/${name}.sym ::/programs/${name}/${name}.sym
    done
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
