#!/bin/bash
set -e

./build.sh

sudo mkdir -p staging
sudo mount /dev/sdb1 staging

sudo mkdir -p staging/efi/boot
sudo cp os/bin/naul.efi staging/efi/boot/bootx64.efi

sudo mkdir -p staging/naul
sudo cp os/bin/naul.sym staging/naul/naul.sym
sudo cp os/font.psf staging/naul/font.psf

sudo mkdir -p staging/programs
sudo cp programs/bin/shell/shell.bin staging/programs/shell.bin
sudo cp programs/bin/test/test.bin staging/programs/test.bin

sudo umount staging
sudo rm -r staging
