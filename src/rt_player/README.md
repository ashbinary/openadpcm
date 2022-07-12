# brstm_rt
C++ RtAudio BRSTM player

## Usage
./brstm_rt file.brstm

-v - Verbose output

-q - Quiet output (Don't display the player UI)

--force-sample-rate [sample rate] - Force playback sample rate

-l - Always loop files with no loop

**Memory modes**

-m - Load the entire file into memory (default for <15MB files)

-s - Stream the file from disk (default for >15MB files)

-d - Decode all audio data into memory before playing

**Controls**

Arrow keys: Seek 1 or 10 seconds

Number keys: Toggle tracks (for multi-track files)

Space: Pause

Q: Quit
