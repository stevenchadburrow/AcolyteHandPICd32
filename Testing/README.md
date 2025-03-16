*** These are just backups of files that are not fully implemented yet. ***

These files contain an NES emulator for the PIC32MZ.  It currently is running at 216 MHz, and if emulating the NES at around 20 FPS with audio it is nearly at full speed.

This currently works with NROM, UNROM, CNROM, MMC1, and MMC3 (some implementation).

Highly tested NROM games:

Super Mario Bros<br>
Micro Mages<br>
Donkey Kong<br>
Mario Bros<br>
Ballon Fight<br>
Ice Climber<br>
Excitebike - the far background scrolling isn't on the right scanline (minimal)<br>
Road Fighter<br>

Highly tested UNROM games:

Contra<br>
1943: Battle for Midway<br>
Castlevania - wrong sprite priority (very minimal)<br>

Highly tested CNROM games:

Paperboy<br>
Gradius<br>

Highly tested MMC1 games:

Tetris<br>
Legend of Zelda - vertical scrolling is shifted by some scanlines (moderate)<br>
Zelda 2<br>
Metroid<br>
Castlevania 2<br>
Ninja Gaiden - some cutscene tiles are wrong (minimal)<br>
Bionic Commando<br>
Dragon Warrior 4<br>
Final Fantasy<br>
Master Blaster - wrong sprite priority (minimal)<br>

Highly tested MMC3 games:

Super Mario Bros 2<br>
Super Mario Bros 3 - wrong sprite priority*, status bar incorrect*, very slow (major)<br>
Kirby's Adventure - status bar incorrect*, intro screen has wrong nametable (minimal)<br>
* = hacks included to correct these graphical glitches

The journey has been documented on the 6502.org Forum, here: http://forum.6502.org/viewtopic.php?f=1&t=8236

