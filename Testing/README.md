*** These are just backups of files that are not fully implemented yet. ***

These files contain an NES emulator for the PIC32MZ.  It currently is running at 216 MHz, and if emulating the NES at 20 FPS with audio it is nearly at full speed.  The audio is a work in progress, as coordinating the microcontroller timers to the emulated cpu cycles isn't always perfect.

This currently works with NROM, UNROM, CNROM, MMC1, and MMC3 (in progress).

Highly tested games for this system include:

Super Mario Bros<br>
Tetris<br>
Micro Mages<br>
Donkey Kong<br>
Mario Bros<br>
Ballon Fight<br>
Ice Climber<br>
Excitebike - the far background scrolling isn't on the right scanline<br>
Paperboy<br>
Gradius<br>
Contra<br>
1943: Battle for Midway<br>
Castlevania - sprite priority isn't right when entering the castle or going through doors<br>
Castlevania 2<br>
Legend of Zelda - vertical scrolling is shifted by some scanlines<br>
Zelda 2<br>
Metroid<br>
Ninja Gaiden - some cutscene tiles are wrong<br>
Bionic Commando<br>
Dragon Warrior 3<br>
Final Fantasy<br>
Super Mario Bros 2<br>

Some games that are currently in development:

Super Mario Bros 3 - wrong sprite tables, as well as sprite priority
Kirby's Adventure - wrong sprite tables
Crystalis - wrong sprite tables

Some games that do not work well:

Rad Racer - scanlines are all over the place<br>
Nuts and Milk - background is missing most of the time<br>
Dragon Warrior 2 - locks up between intro screens<br>

The journey has been documented on the 6502.org Forum, here: http://forum.6502.org/viewtopic.php?f=1&t=8236

