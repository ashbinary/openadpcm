# OpenRevolution
C++ BRSTM tools

Decode, encode, play and convert BRSTM files and other Nintendo audio formats.

Supports lossless conversion between supported formats and up to 8 tracks/16 channels.

## Audio formats

| Format       | Read                | Write               |
|:------------ |:-------------------:|:-------------------:|
| BRSTM        | Yes                 | Yes                 |
| BCSTM        | Yes                 | Yes                 |
| BFSTM        | Yes                 | Yes                 |
| BWAV         | Yes                 | Yes                 |
| BCWAV        | Yes                 | Yes                 |
| BFWAV        | Yes                 | Yes                 |
| IDSP         | Yes                 | No                  |

## Usage
Compile everything by running build.sh or using another compiler with the correct options.

**Windows**: Builds available [here](https://s.neofetch.win/openrevolution/).

**MacOS**: Install with Homebrew through [freeapp2014/stuff](https://github.com/FreeApp2014/homebrew-stuff) (instructions in repository).

**Arch Linux**: Install [openrevolution-git](https://aur.archlinux.org/packages/openrevolution-git/) from AUR.

Dependencies for library:
- None

Dependencies for converter (brstm_converter)
- ffmpeg (optional) - Audio manipulation with the --ffmpeg option

Dependencies for player (brstm_rt)
- librtaudio - Audio output
- unistd.h and termios.h
- POSIX Threads

Usage guides:
- [src/](/src): Command line tools
- [src/rt_player](/src/rt_player): RtAudio command line player
- [src/lib](/src/lib) Library documentation

## Thanks to

- [WiiBrew](https://wiibrew.org/wiki/BRSTM_file): BRSTM file structure reference
- [kenrick95/nikku](https://github.com/kenrick95/nikku) and [BrawlLib](https://github.com/libertyernie/brawltools): DSPADPCM decoder reference
- [jackoalan/gc-dspadpcm-encode](https://github.com/jackoalan/gc-dspadpcm-encode): DSPADPCM encoder
- [gota7](https://gota7.github.io/Citric-Composer/specs/binaryWav.html): BWAV file structure reference
- [mk8.tockdom.com](http://mk8.tockdom.com/wiki/Main_Page) ([\[1\]](http://mk8.tockdom.com/wiki/BFSTM_\(File_Format\))[\[2\]](http://mk8.tockdom.com/wiki/BFWAV_\(File_Format\))) and [3dbrew.org](https://www.3dbrew.org/wiki/Main_Page) ([\[1\]](https://www.3dbrew.org/wiki/BCSTM)[\[2\]](https://www.3dbrew.org/wiki/BCWAV)): BCSTM, BFSTM, BCWAV, BFWAV file structure references
- [FreeApp2014](https://github.com/FreeApp2014): Windows brstm_converter builds
- [Gianmarco Gargiulo](https://gianmarco.ga/): Icon/Logo
- [RtAudio](https://github.com/thestk/rtaudio): RtAudio library

## Planned features

- GUI program
- Support for more file formats
- Multithreaded encoding

