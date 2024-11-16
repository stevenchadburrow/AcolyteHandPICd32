<b>Acolyte Hand PIC'd 32 Retraux Computer</b>

Using the PIC32MZ2048EFH144 microcontroller, this new computer runs at 150 MHz, with internal memory of 512KB of RAM and 2MB of Flash ROM.  

Video output of 512x384 VGA with 256-colors.  Supports 2 independent audio square-wave signals.  Input comes from 2x Genesis controllers, PS/2 Port with support for keyboard and mouse through splitter, and USB port with support for keyboard, mouse, or Xbox360-type gamepad using Aidan Mocke's base code (copied).  The USB port could eventually support flash drives, and can also be used to talk directly to a PC in device-mode.

Comes with attachment possibilities for an SPI Micro SDcard Adapter and a FT232RL USB-to-UART Adapter.  SDcard is required for BadApple and using Elm-Chan's FatFs files (included).  If using UART, use the command 'sudo picocom /dev/ttyUSB0'.  The PIC32 is programmed through ICSP port connecting to a PICkit3.

Links:

<a href="https://www.aidanmocke.com/">https://www.aidanmocke.com/</a> for a bunch of PIC32MZ projects, including USB and FatFs.<br>
<a href="http://elm-chan.org/">http://elm-chan.org/</a> for a bunch of other projects, including MMC (SDcard) and FatFs.<br>

*** Board Edits: Exchange the 74HCT273 for a 74AHCT273 for faster timing responses and thus less jitter and bleeding. ***

<img src="BOARD-PICTURE.jpg">

<img src="MIKU-MENU.jpg">

<img src="MIKU-TETRA.jpg">

<img src="BAD-APPLE.jpg">

<img src="SCRATCH-PAD.jpg">
