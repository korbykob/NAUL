#!/bin/bash
set -e

./build.sh
clear

uefi-run -b OVMF-pure-efi.fd -d -f os/bin/symbols.txt -f os/font.psf -f programs/bin/shell/shell.bin -f programs/bin/test/test.bin os/bin/os.efi -- -enable-kvm -m 4G -cpu host -serial null -serial null -serial stdio -display sdl