#!/bin/bash
set -e

sudo mkdir -p staging
sudo mount /dev/sdb1 staging

sudo mkdir -p staging/efi/boot
sudo cp os/bin/os.efi staging/efi/boot/bootx64.efi

sudo cp os/bin/symbols.txt staging/symbols.txt
sudo cp os/font.psf staging/font.psf
sudo cp programs/bin/shell/shell.bin staging/shell.bin
sudo cp programs/bin/test/test.bin staging/test.bin

sudo umount staging
sudo rm -r staging