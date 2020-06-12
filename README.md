# Open Revolution
C++ BRSTM tools

Decode, encode, play and convert BRSTM files and other Nintendo audio formats.

Supports lossless conversion between supported formats and up to 8 tracks/16 channels.

## Audio formats

| Format       | Read                | Write               |
|:------------ |:-------------------:|:-------------------:|
| BRSTM        | Yes                 | Yes (only ADPCM)    |
| BCSTM        | No                  | No                  |
| BFSTM        | Yes                 | No                  |
| BWAV         | Yes                 | Yes                 |

## Usage
Compile everything by running build.sh or another compiler with the correct options

Usage guides:
- [src/](/src): Command line tools
- [src/rt_player](/src/rt_player): RtAudio command line player
- [src/lib](/src/lib) Library documentation

## Thanks to

- [WiiBrew](https://wiibrew.org/wiki/BRSTM_file): BRSTM file structure reference
- [kenrick95/nikku](https://github.com/kenrick95/nikku) and [BrawlLib](https://github.com/libertyernie/brawltools): DSPADPCM decoder reference
- [jackoalan/gc-dspadpcm-encode](https://github.com/jackoalan/gc-dspadpcm-encode): DSPADPCM encoder
- [gota7](https://gota7.github.io/Citric-Composer/specs/binaryWav.html): BWAV file structure reference
- [mk8.tockdom.com](http://mk8.tockdom.com/wiki/BFSTM_\(File_Format\)): BFSTM file structure reference
- [Gianmarco Gargiulo](https://gianmarco.ga/): Logo
- [RtAudio](https://github.com/thestk/rtaudio): RtAudio library

## Planned features

- Qt GUI
- Support for more file formats
- Multithreaded encoding

