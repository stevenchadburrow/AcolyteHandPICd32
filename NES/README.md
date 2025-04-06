*** These files are for the AcolyteHandPICd32 project, but do not show off it's full capability. ***

*** NesEmulator.c is my Linux/OpenGL port, but without audio.  It was made for debugging purposes. ***

These files contain an NES emulator for the PIC32MZ.  It currently is running at 260 MHz (overclocked from 252 MHz max).  Most games run at perfect speed at 20 FPS with audio enabled, even Super Mario Bros 3!

This emulator currently works with the following mappers: NROM, UNROM, CNROM, ANROM, MMC1, and MMC3 (mostly implemented).

Some MMC3 games have issues with IRQ timing, though adjusting both cycle and scanline delays help in most cases.  Mega Man 3, Mega Man 4, and Ninja Gaiden 2 all need these adjusted to even run!  Crystalis needs it adjusted to remove the seam.

Only one hack is currently employed: Sprite priority for Super Mario Bros 3 (though also helps a little in Prince of Persia).

The journey has been documented on the 6502.org Forum, here: http://forum.6502.org/viewtopic.php?f=1&t=8236

Here is a list of games that are supported, with notes:

1942<br>
1943: Battle for Midway<br>
ADnD: Heroes of the Lance<br>
Adventure Island<br>
Adventure Island 2<br>
Adventure Island 3<br>
Adventures of Lolo<br>
Adventures of Lolo 2<br>
Adventures of Lolo 3<br>
Arkanoid<br>
Arkista's Ring<br>
Athena<br>
Balloon Fight<br>
Baseball<br>
Battle Kid<br>
Battletoads<Br>
Batman<br>
Bionic Commando<br>
Blades of Steel<br>
Blaster Master<br>
Battletoads and Double Dragon<br>
Bubble Bobble<br>
Castlevania<br>
Castlevania 2<br>
Chip and Dale<br>
Clu Clu Land<br>
Cobra Triangle<br>
Contra<br>
Championship Rally<br>
Crystalis - Needs IRQ adjustments to remove seam<br>
Double Dragon<br>
Double Dragon 2<br>
Dig Dug<br>
Donkey Kong<br>
Donkey Kong Jr.<br>
Double Dribble<br>


