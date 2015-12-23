# Rip'Em
Rip'Em is a 3rd party firmware for the HP Prime calculator.

For now, unless you can connect to the 3.3v TTL serial port inside the calculator nothing _too_ exciting will happen.

As always : please void your warranty in a responsible manner. I will decline any responsibility should you turn your HP Prime into the thinnest CAS calculator brick available currently on the market.

## How to build
On Debian and Debian-like systems :
 * Install required development packages (`apt-get install binutils-arm-none-eabi gcc-arm-none-eabi gdb-arm-none-eabi`) ;
 * Run `make` inside the top-level directory.

Everything else : you are on your own.

## How to install
Using the HP Connectivity Kit :
 * Replace `PRIME_OS.ROM` with the file `ripem/ripem.rom` generated when building Rip'Em ;
 * Flash the new firmware.

Everything else : you are on your own.

## How to use

### Launching the GDB stub
To boot the GDB stub :
 * Cold-boot the calculator ;
 * Keep the `ON` key pressed for about 2 seconds until the blue LED lights up.

The GDB stub implements only the bare minimum required to poke around the memory of the calculator and to upload/execute code through it. When busy with a request, the stub will light up the red LED.

To reset the calculator when the stub isn't busy, press the `ON` key.

### Launching the payload
To boot the payload :
 * Cold-boot the calculator ;
 * Immediately release the `ON` key.

A dummy payload is provided by default to cycle the LEDs.

The official `PRIME_OS.ROM` can also be launched as a payload, provided one uses the truncating ability of `osrom2elf` to make it fit.

# License
Haven't picked one yet. Should somebody else tries to contribute to this, a `LICENSE` file will appear in the repository, possibly with the BSD license text inside.
