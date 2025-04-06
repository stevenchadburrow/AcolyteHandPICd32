<b>NES Emulator for PIC32MZ</b>

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
Donkey Kong Jr<br>
Double Dribble<br>
Dragon Warrior - Screen flickers sometimes<br>
Dragon Warrior 2<br>
Dragon Warrior 3<br>
Dragon Warrior 4<br>
Dr. Mario<br>
Duck Tales<br>
Excitebike - Scroll scanline is too high<br>
Faria<br>
Faxanadu<br>
Final Fantasy<br>
Formula One: Built to Win<br>
Galaga<br>
Gauntlet 2 - Needs IRQ adjustments to help seam and status bar, though never perfect<br>
Ghost Lion<br>
Ghosts and Goblins<br>
Golf<br>
Gradius<br>
Guardian Legend - Bottom status bar comes in and out sometimes<br>
Ice Climber<br>
Joust<br>
Jurassic Park<br>
Kid Icarus<br>
King's Quest 5<br>
Kirby's Adventure<br>
Kung Fu<br>
Legacy of Wizards<br>
Lemmings<br>
Life Force<br>
Little Mermaid<br>
Little Samson<br>
Lode Runner<br>
Mach Rider<br>
Marble Madness<br>
Mario Bros<br>
Mega Man<br>
Mega Man 2<br>
Mega Man 3 - Must have IRQ turned off at boot to run<br>
Mega Man 4 - Must have IRQ turned off at boot to run<br>
Mega Man 5<br>
Mega Man 6<br>
Metal Gear<br>
Metroid<br>
Micro Mages<br>
Micro Mages: Second Quest<br>
Moon Crystal<br>
Metal Storm<br>
Ninja Gaiden<br>
Ninja Gaiden 2 - Needs IRQ adjusted so game is not halted<br>
Ninja Gaiden 3<br>
Pac-man<br>
Paperboy<br>
Paperboy 2<br>
Prince of Persia - Sprite priority hack only helps a little when behind columns<br>
Rad Racer<br>
Rampage<br>
RC Pro AM<br>
RC Pro AM 2<br>
River City Ransom<br>
S.C.A.T.<br>
Skate or Die<br>
Slalom<br>
Super Mario Bros<br>
Super Mario Bros 2<br>
Super Mario Bros 3 - Sprite priority hack helps a lot, can unadjust IRQ to make multi-scrolling perfect<br>
Smash TV<br>
Super Off Road<br>
Solstice<br>
Spy Hunter<br>
Silver Surfer<br>
Star Tropics - Top tiles of bottom status bar are cut off<br>
Strider<br>
Super C<br>
Tecmo Super Bowl<br>
Tetris<br>
Time Lord<br>
TMNT<br>
TMNT 2<br>
TMNT 3<br>
Turrican<br>
Ultima - Quest of the Avatar<br>
Wario's Woods<br>
Wizardry - Proving Grounds<br>
Wizards and Warriors<br>
Pro Wrestling<br>
Legend of Zelda<br>
Zelda 2: Link's Adventure<br>


