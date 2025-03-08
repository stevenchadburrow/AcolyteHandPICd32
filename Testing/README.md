*** These are just backups of files that are not fully implemented yet. ***

These files contain an NES emulator for the PIC32MZ.  It currently is running at 216 MHz, and if emulating the NES at 20 FPS with audio it is nearly at full speed.

Currently this works with NROM, UxROM, CxROM, and also MMC1 mappers.  Next up is MMC3 mappers.

Well tested games for this system include:

Super Mario Bros
Tetris
Micro Mages
Donkey Kong
Mario Bros
Ballon Fight
Ice Climber
Excitebike - the far background scrolling isn't on the right scanline
Paperboy
Gradius - some enemy sprites are from incorrect table?
Contra
1943: Battle for Midway
Duck Tales - HUD displays from wrong table, vertical scrolling doesn't work well
Castlevania
Castlevania 2
Legend of Zelda - vertical scrolling is shifted by some scanlines
Zelda 2
Metroid
Ninja Gaiden - some cutscene tiles are wrong
Bionic Commando
Dragon Warrior 3
Final Fantasy

Some games that do not work well:

Rad Racer - scanlines are all over the place
Nuts and Milk - background is missing most of the time
Dragon Warrior 2 - locks up between intro screens

The journey has been documented on the 6502.org Forum, here: http://forum.6502.org/viewtopic.php?f=1&t=8236

