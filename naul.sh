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
    export COMPILER_FLAGS="-I${PWD}/include -std=gnu17 -g -fno-omit-frame-pointer -ffreestanding -fno-stack-protector -fno-stack-check -mno-red-zone -mavx2 -O2 -fvect-cost-model=dynamic"
    export BOOTLOADER_COMPILER_FLAGS="$COMPILER_FLAGS -Ios/include -I/usr/include/efi -fshort-wchar -maccumulate-outgoing-args -fpic -c"
    export KERNEL_COMPILER_FLAGS="$COMPILER_FLAGS -Ios/include -nostdinc -fpic -c"
    export PROGRAM_COMPILER_FLAGS="$COMPILER_FLAGS -I${PWD}/programs/include -nostdinc -mcmodel=large -static -fno-pic -fno-pie -c"

    export KERNEL_LINKER_FLAGS="-shared -Bsymbolic -T/usr/lib/elf_x86_64_efi.lds /usr/lib/crt0-efi-x86_64.o"
    export KERNEL_LINKER_LIBS="/usr/lib/libgnuefi.a /usr/lib/libefi.a"
    export PROGRAM_LINKER_FLAGS="-no-pie -T${PWD}/programs/nxe.ld"

    mkdir -p os/bin
    gcc $BOOTLOADER_COMPILER_FLAGS os/src/bootloader.c -o os/bin/bootloader.o
    gcc $KERNEL_COMPILER_FLAGS os/src/allocator.c -o os/bin/allocator.o
    gcc $KERNEL_COMPILER_FLAGS os/src/gdt.c -o os/bin/gdt.o
    gcc $KERNEL_COMPILER_FLAGS os/src/idt.c -o os/bin/idt.o
    gcc $KERNEL_COMPILER_FLAGS os/src/pic.c -o os/bin/pic.o
    gcc $KERNEL_COMPILER_FLAGS os/src/filesystem.c -o os/bin/filesystem.o
    gcc $KERNEL_COMPILER_FLAGS os/src/symbols.c -o os/bin/symbols.o
    gcc $KERNEL_COMPILER_FLAGS os/src/panic.c -o os/bin/panic.o
    gcc $KERNEL_COMPILER_FLAGS os/src/paging.c -o os/bin/paging.o
    gcc $KERNEL_COMPILER_FLAGS os/src/syscalls.c -o os/bin/syscalls.o
    gcc $KERNEL_COMPILER_FLAGS os/src/hpet.c -o os/bin/hpet.o
    gcc $KERNEL_COMPILER_FLAGS os/src/scheduler.c -o os/bin/scheduler.o
    gcc $KERNEL_COMPILER_FLAGS os/src/keyboard.c -o os/bin/keyboard.o
    gcc $KERNEL_COMPILER_FLAGS os/src/mouse.c -o os/bin/mouse.o
    gcc $KERNEL_COMPILER_FLAGS os/src/display.c -o os/bin/display.o
    gcc $KERNEL_COMPILER_FLAGS os/src/tty.c -o os/bin/tty.o
    gcc $KERNEL_COMPILER_FLAGS os/src/terminal.c -o os/bin/terminal.o
    gcc $KERNEL_COMPILER_FLAGS os/src/processes.c -o os/bin/processes.o
    gcc $KERNEL_COMPILER_FLAGS os/src/ipc.c -o os/bin/ipc.o
    gcc $KERNEL_COMPILER_FLAGS os/src/kernel.c -o os/bin/kernel.o

    ld $KERNEL_LINKER_FLAGS \
    os/bin/bootloader.o \
    os/bin/allocator.o \
    os/bin/gdt.o \
    os/bin/idt.o \
    os/bin/pic.o \
    os/bin/filesystem.o \
    os/bin/symbols.o \
    os/bin/panic.o \
    os/bin/paging.o \
    os/bin/syscalls.o \
    os/bin/hpet.o \
    os/bin/scheduler.o \
    os/bin/keyboard.o \
    os/bin/mouse.o \
    os/bin/display.o \
    os/bin/tty.o \
    os/bin/terminal.o \
    os/bin/processes.o \
    os/bin/ipc.o \
    os/bin/kernel.o \
    -o os/bin/naul.so $KERNEL_LINKER_LIBS

    nm os/bin/naul.so > os/bin/naul.sym

    objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --output-target efi-app-x86_64 --subsystem=10 os/bin/naul.so os/bin/naul.efi

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
    build

    dd if=/dev/zero of=naul.iso bs=1M count=12 status=none
    mformat -i naul.iso ::

    mmd -i naul.iso ::/efi
    mmd -i naul.iso ::/efi/boot
    mcopy -i naul.iso os/bin/naul.efi ::/efi/boot/bootx64.efi

    mmd -i naul.iso ::/naul
    mcopy -i naul.iso os/bin/naul.sym ::/naul/naul.sym
    mcopy -i naul.iso os/res/font.psf ::/naul/font.psf

    mmd -i naul.iso ::/programs
    for program in programs/programs/*/; do
        name=$(basename $program)

        mmd -i naul.iso ::/programs/$name
        mcopy -i naul.iso ${program}bin/${name}.nxe ::/programs/${name}/${name}.nxe
        mcopy -i naul.iso ${program}bin/${name}.sym ::/programs/${name}/${name}.sym

        if [ -d "${program}res" ]; then
            for file in ${program}res/*; do
                mcopy -i naul.iso $file ::/programs/${name}/$(basename $file)
            done
        fi
    done
}

run()
{
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
    echo "    iso: Build and create an iso"
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
