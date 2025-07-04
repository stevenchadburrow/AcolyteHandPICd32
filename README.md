<b><h1>Acolyte Hand PIC'd 32 Retraux Computer and Video Game Console</h1></b>

<b>Background</b>

Why build a "computer"?  Why build a "video game console"?  

I had been programming video games for about 20 years before all of this.  It started in high school on Borland Turbo C++ in the computer lab, and then in college on a Ubuntu Linux computer with g++ and OpenGL.  Most of my early attempts are not even worth mentioning though I tried different genres such as FPS and RTS.  After graduate school and getting a job at the local college as a math teacher, I began trying to make a non-violent video game, namely a <a href="https://en.wikipedia.org/wiki/Harvest_Moon_(video_game)">Harvest Moon</a> clone.  It seems that <a href="https://en.wikipedia.org/wiki/Stardew_Valley">Stardew Valley</a> beat me to it!  After many attempts on many different platforms, including Linux/OpenGL/C++, HTML/Javascript, and Android/Java, I have declared them all as 'demos', though I learned a lot from that time.  Meanwhile, I began programming 'arcade' video games for our annual Math Appreciation Day events, to have on display and allow students to play during the event.  A few are listed <a href="https://github.com/stevenchadburrow/MathDayGames">here</a>.  

In Fall 2021, after starting research on a water-based calculator, I thought to myself, "Wouldn't electricity be so much easier?"  After stumbling upon <a href="https://wilsonminesco.com/">Garth Wilson's 6502 Primer</a>, I emailed him and he invited me to join the <a href="http://forum.6502.org/">6502.org Forum</a>.  I began researching and prototyping (aka breadboarding) with basic logic chips, and learning how to create schematics.  While learning more about digital electronics, I found The 8-Bit Guy, Adrian's Digital Basement, and Ben Eater on Youtube.

Inspired by The 8-Bit Guy's <a href="https://www.youtube.com/watch?v=ayh0qebfD2g">"Dream Computer"</a>, and after many initial attempts (aka failures) and a couple of awesome successes, I eventually produced the original <a href="https://github.com/stevenchadburrow/AcolyteComputer">Acolyte Computer</a> using a W65C02 processor.  I had great help from Garth Wilson and the guys on the 6502.org Forum as I worked through many prototypes to get to that point.  I was able to display this and other projects at the Math Appreciation Day events playing video games such as Space Invaders, Tetris, and Rogue.

From there, the project grew to the <a href="https://github.com/stevenchadburrow/NewAcolyte">New Acolyte</a> coupled with an XC9572XL CPLD, then the <a href="https://github.com/stevenchadburrow/Acolyte816">Acolyte '816</a> using a W65C816 processor coupled with a PIC16F886 microcontroller as a bootloader.  Wanting to continue pursuing PIC microcontrollers, I created the original <a href="https://github.com/stevenchadburrow/AcolyteHandPICd">Acolyte Hand PIC'd</a> using the PIC18F47J13 microcontroller, then the <a href="https://github.com/stevenchadburrow/AcolyteHandPICd24">Acolyte Hand PIC'd 24</a> using the PIC24EP512GP204 microcontroller finally without a coupled CPLD.  During Summer 2024, I programmed a 2-player <a href="https://www.youtube.com/watch?v=8EHRpKk9gPc">Pokemon demo</a> using <a href="https://tuxemon.org/">Tuxemon</a> art assets, pushing that PIC24 to the limit.

This project, the Acolyte Hand PIC'd 32, is supposed to show the culmination of my knowledge in PCB design and microcontroller programming over the past 3 years.  The intention is to create a standardized development platform for many new video games that will be displayed during the Math Appreciation Day events.

<b>Microcontroller</b>

PIC32MZ2048EFH144 running at 200 MHz
<br>
Internal memory of 512KB of RAM and 2MB of Flash ROM

<b>Video</b>

VGA 800x600 max resolution (720x512 typical resolution) at 72 Hz with 256 colors or 65K colors

<b>Audio</b>

Dual 8-bit audio channels

<b>HID</b>

2x Sega Genesis controller ports
<br>
Dual PS/2 port for keyboard and mouse through splitter
<br>
USB port for keyboard, mouse, or Xbox-360 controller

<b>File System</b>

Elm-Chan's <a href="http://elm-chan.org/fsw/ff/">FatFS Generic FAT Filesystem Module</a> using SPI Micro SD Card adapter
<br>
(use command 'sudo mkfs.vfat /dev/sdX' to format SDcard)

<b>UART</b>

FT232RL USB-to-UART adapter
<br>
(use command 'sudo picocom /dev/ttyUSB0' for default 9600 baud connection)

<b>Custom Applications</b>

Tetra - Tetris clone recopied from my previous PIC-related projects
<br>
Scratchpad - Proof-of-concept 'text editor' without save/load features
<br>
AV Demo - Audio/Video playback of modified video files with a resolution 240x192 at 15 FPS with 65K colors and dual 8-bit audio

<b>Gameboy Emulator</b>

Mahyar Koshkouei's <a href="https://github.com/deltabeard/Peanut-GB">Peanut-GB (with MiniGB-APU) Gameboy Emulator</a>
<br>
Added support for GBC default color palettes, custom palettes, game file save/load, and screen size and refresh rate adjustments
<br>
Tested with many games, including: Tetris, Dr. Mario, Balloon Kid, Zelda: Link's Awakening, Tobu Tobu Girl, Pokemon Red, and Pokemon Kanto Expansion Pak

<b>Links</b>

<a href="https://www.aidanmocke.com/">https://www.aidanmocke.com/</a> for a bunch of PIC32MZ projects (without Harmony), including USB.<br>
<a href="http://elm-chan.org/">http://elm-chan.org/</a> for a bunch of projects, including MMC (aka SDcard) and FatFs.<br>
<a href="https://github.com/deltabeard/">https://github.com/deltabeard/</a> for a bunch of projects, including Peanut-GB and MiniGB-APU projects.<br>
<a href="https://hackaday.com/2025/01/13/homebrew-retro-console-runs-on-pic32/">https://hackaday.com/2025/01/13/homebrew-retro-console-runs-on-pic32/</a> for the Hack-A-Day article!<br>

<b>Look Ma, no bodges! (showing Version 2 board)</b>
<br>
<img src="BOARD-PICTURE.jpg">

<b>3D printed case fits perfectly! (showing Version 1 board)</b>
<br>
<img src="PRINTED-CASE.jpg">

<b>Main menu format with splash screen.</b>
<br>
<img src="MIKU-MENU.jpg">

<b>Gotta love some 2-player Tetris!</b>
<br>
<img src="MIKU-TETRA.jpg">

<b>It can now play any Gameboy game!</b>
<br>
<img src="TOBU-GB.jpg">

<b>Audio/Video playback running at 65K colors and dual 8-bit audio.</b>
<br>
<img src="AV-BADAPPLE.gif">&nbsp;&nbsp;&nbsp;&nbsp;<img src="AV-YOGURTING.gif">&nbsp;&nbsp;&nbsp;&nbsp;<img src="AV-SKYRIM.gif">

