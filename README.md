# sound-project

This is a personal project for parsing, playing, and processing WAV files. Currently, the WaveformPlayer application plays a WAV file while displaying a spectrum visualization.

You can try a demo of WaveformPlayer on the web here: 

## Libraries

WaveformPlayer uses the libraries Portaudio, NCurses (PDCurses for Windows), and FFTW. See the build instructions below for information on how to configure these libraries. I've tested on Windows and certain distros of Linux, but theoretically it should be able to be built on MacOS as well.

The web demo was ported by compiling PDCurses for SDL2, FFTW, and the main project files with Emscripten. Howler.js was used to replace Portaudio. You can quickly build it by running `emcc ../*.cc libfftw3.a pdcurses.a -I./ -s WASM=1 -s USE_SDL=2  -s ASYNCIFY -s ALLOW_MEMORY_GROWTH=1 --shell-file shell_minimal.html -o index.html` in the `browser` directory of this repo.

## WaveformPlayer

WaveformPlayer is a console application that plays a WAV file and displays a spectrum visualization in terminal. To run on Linux:

`Usage: ./WaveformPlayer FILE [OPTION]`

To run on Windows (must use a console compatible with PDCurses build for Windows Console):

`Usage: WaveformPlayer.exe FILE [OPTION]`

Use the `-help` option for details:

This program plays a .wav file and displays some visuals. Press 'q' to stop playing. This program calculates an entire spectrogram for the file, which can take a lot of RAM (a 4 minute 2-channel file at 44100 Hz with window = 4096 and overlap = 2048 took about 2.2 GB. Disable the spectrum by using the option 'nospec' to use less RAM.

Options:

  -debug : Generates logs for data of the WAV file. 'logNormData.txt' contains the data chunk normalized, 'logHeader.txt' contains header information, 'logRawBytes.txt' displays the data chunk in hex, and 'logNumData.txt' displays the data chunk numerically.

  -nospec : Does not generate spectrogram or display spectrum while playing.

  -custom_spec : will prompt the user to enter an integer for the window and overlap (both must be positive integers, with window > overlap). This means each frame is window - overlap apart. Will also ask if a Hann window will be used.

  By default (no options), uses window = 4096, overlap = 2048, with Hann windowing.

## Building on Linux

We need to install Portaudio, NCurses, and FFTW. These can be installed by simply using a package manager. Note that Portaudio will also require `libasound-dev` to be installed.

`apt install libasound-dev portaudio19-dev libncurses5-dev libfftw3-dev`

Note that depending on what is available on your system, Portaudio may be configured differently, and so you might need to link different libraries than the ones in the provided Makefile for WaveformPlayer. See http://portaudio.com/docs/v19-doxydocs/compile_linux.html for details. If Portaudio is built with only ALSA then it should work as-is.

Once libraries are installed, `make` in the root directory (the one with the Makefile).

If you want to build these libraries manually:

- Portaudio
		- See [http://portaudio.com/docs/v19-doxydocs/tutorial_start.html](http://portaudio.com/docs/v19-doxydocs/tutorial_start.html) for how to build Portaudio
- NCurses
		- Download NCurses and follow the instructions in the README: https://invisible-island.net/ncurses/
		- Install FFTW by downloading the source from: http://www.fftw.org/download.html and following the instructions here: http://www.fftw.org/fftw3_doc/Installation-on-Unix.html#Installation-on-Unix.
- FFTW
		- Download the source from: http://www.fftw.org/download.html and follow the instructions here: http://www.fftw.org/fftw3_doc/Installation-on-Unix.html#Installation-on-Unix.
		
Note:
If playback doesn't start on Linux systems that use PulseAudio, try commenting out `load-module module-suspend-on-idle` from /etc/pulse/default.pa

## Building on Windows

WaveformPlayer comes prebuilt for Windows x86 and x64. The .exe files are located in `windows/Win32` and `windows/x64`, and are ready to use.

These were built with Windows 10.0.17763.0 SDK. PortAudio was built with support for MME, DirectSound, WASAPI, WDM/KS, but not ASIO (see http://portaudio.com/docs/v19-doxydocs/compile_windows.html for more info on building PortAudio for Windows).

PDCurses was built for Windows Console following instructions here: https://github.com/wmcbrine/PDCurses/blob/master/wincon/README.md.

I used the precompiled FFTW .dll files from http://www.fftw.org/install/windows.html.

If you want to build it yourself, the provided .sln file can be used to open the project with Microsoft Visual Studio with relative paths set up, so you can just use the "Build Solution" button.
