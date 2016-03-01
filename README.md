# Rip'Em
Rip'Em is a 3rd party firmware for the HP Prime calculator.

As always : please void your warranty in a responsible manner. I will decline any responsibility should you turn your HP Prime into the thinnest CAS calculator brick available currently on the market.

## How to build
On Debian and Debian-like systems :
 * Install the ARM cross-compiler toolchain (`apt-get install binutils-arm-none-eabi gcc-arm-none-eabi gdb-arm-none-eabi`) ;
 * Install support libraries for tools (`apt-get install libelf1 libelf-dev`) ;
 * Run `make` inside the top-level directory.

Everything else : you are on your own.

## How to install
Using the HP Connectivity Kit :
 * Replace `PRIME_OS.ROM` with the file `ripem/ripem.rom` generated when building Rip'Em ;
 * Flash the new firmware.

Everything else : you are on your own.

## How to use

A graphical menu is shown to select a payload to run.

These payloads are currently available :
* dumbcalc.elf : a dumb integer RPN calculator, as a concrete demo ;
* dummy.elf : a dumb payload to showcase functionality ;
* gdbstub_serial.elf : a bare-bones GDB stub over the UART.

# License
Haven't picked one yet. Should somebody else tries to contribute to this, a `LICENSE` file will appear in the repository, possibly with the BSD license text inside.
