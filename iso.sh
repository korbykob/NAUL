#!/bin/bash
set -e

dd if=/dev/zero of=naul.iso bs=1M count=8 status=none
mformat -i naul.iso ::

mmd -i naul.iso ::/efi
mmd -i naul.iso ::/efi/boot
mcopy -i naul.iso os/bin/os.efi ::/efi/boot/bootx64.efi

mmd -i naul.iso ::/naul
mcopy -i naul.iso os/bin/symbols.txt ::/naul/symbols.txt
mcopy -i naul.iso os/font.psf ::/naul/font.psf

mmd -i naul.iso ::/programs
mcopy -i naul.iso programs/bin/shell/shell.bin ::/programs/shell.bin
mcopy -i naul.iso programs/bin/test/test.bin ::/programs/test.bin
