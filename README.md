# sound-project

This is a personal project for parsing, playing, and processing WAV files. It is a work in progress, so it isn't polished and might not be in working condition yet, but feel free to do whatever you like with it.

## Current Features

- Read a WAV file into a structure (Little-Endian, IEE 754 float compliant systems only) (see WAVFile files)
- Playback audio given the data in the structure (See WAVPlayer files)
- Display real-time information during playback (See WAVDisplay files)
An example that uses all three is WaveformPlayer, which parses a WAV file, plays it, and displays real-time amplitude information for each channel, as well as the current playback time.
## To Be Completed

- Live spectrum and spectrogram display during playback
- Better graphics (possibly with OpenGL)
- Improved WAV file parsing to account for more chunk types
- Generate spectrogram with STFT, allowing for customizable window functions and STFT parameters
## Building
To build WaveformPlayer, see the Makefile as a reference for what is needed to compile. 
The Makefile statically links with libportaudio.a (included in the repo) and dynamically links everything else. This libportaudio.a was built on Ubuntu with ALSA, which may not work for your system. In that case, build/install the libraries for your system and link them.
- Portaudio
		- See [http://portaudio.com/docs/v19-doxydocs/tutorial_start.html](http://portaudio.com/docs/v19-doxydocs/tutorial_start.html) for how to build Portaudio for your OS. Portaudio can be built for Linux, Windows, and Mac OS X.
- NCurses (or PDCurses for Windows, untested)
		- Used for the display. For Linux, `apt install libncurses5-dev` and compiling with `-lncurses` should work. For Windows, you can probably get it to work with PDCurses but I haven't tried it.

Example:
`g++ WaveformPlayer.cc WAVFile.cc WAVPlayer.cc Display.cc WaveformDisplay.cc -lportaudio -lncurses -lasound -lrt -lm -pthread -o WaveformPlayer`
If playback hangs on Linux systems that use PulseAudio, try commenting out "load-module module-suspend-on-idle" from /etc/pulse/
