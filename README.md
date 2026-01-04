# NAUL (Not A Unix Like)
My operating system which is fortunately not another unix like OS, kinda the OS analogue of:

![Nah imma do my own thing](https://media1.tenor.com/m/rRMj_95KPOUAAAAd/nah-imma-do-my-own-thing-miles-morales.gif)
## Features
- UEFI bootloader
- Long (64 bit) mode
- Serial
- Graphics
- PSF font parsing
- Interrupts
- PS/2 keyboard support
- Allocator
- Paging
- HPET support
- Ram filesystem
- Multithreading (and mutexes)
- Terminal input and output
- Syscalls (WIP)
- Programs (WIP)
## Plans
You think I have plans? Pfft. Everything I say I'll never do, I end up doing anyways.
## Compiling
First clone the repo and its submodules:
```
git clone --recursive https://github.com/korbykob/NAUL.git
```
Ensure you have modern gcc, ld, objcopy, and make binaries and run:
```
cd NAUL
./build.sh
```
## Running
After compiling you can run this under QEMU, ensure you have [uefi-run](https://github.com/richard-w/uefi-run) installed and run:
```
./run.sh
```
