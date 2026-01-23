# NAUL (Not A Unix Like)
My operating system which is my own very Unix unlike OS, kinda the OS analogue of:

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
- LAPIC timer support
- Ram filesystem
- Multithreading (and mutexes)
- Terminal input and output
- Symbol tables
- Processes
- Terminal
- Syscalls
## Plans
You think I have plans? Pfft. Everything I say I'll never do, I end up doing anyways.
## Emulating
First clone the repo and its submodules:
```
git clone --recursive https://github.com/korbykob/NAUL.git
```
Ensure you have QEMU, build essentials, and [uefi-run](https://github.com/richard-w/uefi-run) installed and run:
```
cd NAUL
./run.sh
```
