#!/bin/bash
set -e

./build.sh
./iso.sh
clear

qemu-system-x86_64 -enable-kvm -bios OVMF-pure-efi.fd -cdrom naul.iso -m 4G -cpu host -serial null -serial null -serial stdio -display sdl
