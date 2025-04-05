*** These files are for the AcolyteHandPICd32 project, but do not show off it's full capability. ***

*** NesEmulator.c is my Linux/OpenGL port, but without audio.  It was made for debugging purposes. ***

These files contain an NES emulator for the PIC32MZ.  It currently is running at 260 MHz (overclocked from 252 MHz max).  Most games run at perfect speed at 20 FPS with audio enabled, even Super Mario Bros 3!

This emulator currently works with the following mappers: NROM, UNROM, CNROM, ANROM, MMC1, and MMC3 (only some implementation).

Some MMC3 games have issues with IRQ timing, though adjusting both cycle and scanline delays help in most cases.

Only one hack is currently employed: Sprite priority in Super Mario Bros 3.

The journey has been documented on the 6502.org Forum, here: http://forum.6502.org/viewtopic.php?f=1&t=8236

