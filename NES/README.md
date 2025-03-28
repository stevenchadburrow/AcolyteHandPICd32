*** These are just backups of files that are not fully implemented yet.  NesEmulator.c is my Linux/OpenGL port. ***

These files contain an NES emulator for the PIC32MZ.  It currently is running at 216 MHz, and if emulating the NES at around 20 FPS with audio it is nearly at full speed.

This currently works with these mappers: NROM, UNROM, CNROM, ANROM, MMC1, and MMC3 (only some implementation).

Some MMC3 games have issues with IRQ timing, though adjusting both cycle and scanline delays help in most cases.

The journey has been documented on the 6502.org Forum, here: http://forum.6502.org/viewtopic.php?f=1&t=8236

