# niji

clears your terminal screen but rainbow

based on [the old X68000 niji](https://twitter.com/kata68k/status/1782780099148046481)

## building

build via CMake. release mode via `-DCMAKE_BUILD_TYPE=Release`.

if building via MinGW cut the binary size in 4 by `strip`ping it afterward.

## linux when?

currently this functions on Windows by using `ReadConsoleOutputCharacterW` to get the current state of the screen. for a
Linux version, this would have to be replaced with
- DECRQCRA, to retrieve the hash of a character+formatting at a location
- DECSERA, to replace the formatting of a character at a location with known formatting, to crack the character hash
- BSU/DSU (DECSET 2026), to prevent the screen flashes (to mimic `WriteConsoleW`'s single-chunk behavior)

Windows Terminal's DECRQCRA is turned off by default, reasonably so as it allows malicious processes to see anything in
the terminal. I believe (but am not sure) that the `kitty` and `alacritty` terminals support both DECRQCRA and
synchronized updates; if I could test them I would add CMake options for choosing the target environment.

additionally, the Windows version uses `DwmFlush` to do a small frame wait, rather than deal with the 15ms Windows
scheduler resolution. due to Linux scheduling being higher-resolution, this could probably be replaced with some sort of
sleep.

## DECCARA

DECCARA does not support changing colors: it supports a finite subset of SGI commands, of which the only one even
relevant to our cause is text inversion. if there were to be a more "modern" equivalent for DECCARA that could change
the colors of a rectangle in the console, this program could be achieved without any sort of security holes.
