# Digi-XDft
Copyright (c) 2019 by Wayne E. Wright, W5XD<br/>
.NET wrappers for FT8 modulate and demodulate, including a sample program

This project contains the XDft.dll .NET assembly which wraps the FT8 encoder/decoder by Joe Taylor, K1JT. This assembly,
with some help reading/writing a sound card, can produce FT8 tones on a sound card, or decode received FT8 tones into text.
The "help reading/writing a sound card" is found in a submodule.

See COPYING for the license. This code depends on that published for wsjtx-2.0.1.

This project also contains XDft8Test.exe which is an example of how to put the various pieces together into a working program.
In addition to the submodule, you'll need the XDwsjtFT8.dll from the project <a href='https://github.com/w5xd/Digi-XDwsjt'>Digi-XDwsjt</a>.

The .props files are designed to be edited to reflect the disk locations of the various needed components.

