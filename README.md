# potato8
Basic, at the moment, rather broken, chip8 emulator.

Everyone working on emulators makes a chip8 emulator at some point, and I figured I'd hack one out. This was mostly written during one weekend, and largely implemented before doing much testing.

I figured out what was causing a big display issue, so now a number of demos work. Games generally don't, and the keyboard support is probably broken.

Once I actually have things working, things I may do include:
Implement Super Chip 8.
A more sophisticated display routine, since this is just slapping down rectangles. A Vulkan display would be amusing.
Get rid of the switch statement, and do function pointers. Implement JIT, to get some experience with JIT.
Customizable keys. (Not sure the keys even work yet...)

It may be a while, because this is basically something I'm going to pick up and play with when I'm bored.

Implemented:
Better logging with spdlog.
Moved most of the switch statement to functions.
Fixed up the display.

Things I've been looking at include (but are not limited to):
http://devernay.free.fr/hacks/chip8/
http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
https://github.com/wernsey/chip8

https://slack-files.com/T3CH37TNX-F3RKEUKL4-b05ab4930d
https://slack-files.com/T3CH37TNX-F3RF5KT43-0fb93dbd1f