#!/bin/bash
set -e

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
mcopy -i naul.iso programs/bin/shell/shell.bin ::/programs/shell/shell.bin
mcopy -i naul.iso programs/bin/shell/shell.sym ::/programs/shell/shell.sym

mmd -i naul.iso ::/programs/test
mcopy -i naul.iso programs/bin/test/test.bin ::/programs/test/test.bin
mcopy -i naul.iso programs/bin/test/test.sym ::/programs/test/test.sym

mmd -i naul.iso ::/programs/doom
mcopy -i naul.iso programs/bin/doom/doom.bin ::/programs/doom/doom.bin
mcopy -i naul.iso programs/bin/doom/doom.sym ::/programs/doom/doom.sym
mcopy -i naul.iso programs/src/doom/doom1.wad ::/programs/doom/doom1.wad
