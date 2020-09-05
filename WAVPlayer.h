#ifndef WAVPLAYER_H
#define WAVPLAYER_H

#include <cstdint>
#include <vector>

#ifndef __EMSCRIPTEN__

#include <portaudio.h>

#else

#include <emscripten.h>
#include <emscripten/html5.h>

#endif

class Display;
struct WAVFile;

#ifdef __EMSCRIPTEN__

struct dataStruct {

	uint32_t currentDataIndex;
	uint32_t sampleRate;

};

#else

struct dataStruct {
	
	uint32_t currentDataIndex;
	uint16_t numChannels;
	std::vector<double> *data;

};

#endif

class WAVPlayer {	

	WAVFile *theFile;
	
	uint16_t numChannels;
	uint32_t sampleRate;
	uint16_t blockAlign;
	uint16_t bytesPerSample;

	dataStruct currentData;

#ifdef __EMSCRIPTEN__

	static EM_BOOL iteration(double time, void *data);

#else

	static int patestCallback(const void *inputBuffer, void *outputBuffer,
                                unsigned long framesPerBuffer,
                                const PaStreamCallbackTimeInfo* timeInfo,
                                PaStreamCallbackFlags statusFlags,
                                void *userData);

#endif

	public:
	
	Display *display;

	WAVPlayer(WAVFile *theFile);

	void play();
	
	// Returns a number from 0 to theFile->numSamplesPerChannel - 1
	// equal to currentDataIndex / (blockAlign / bytesPerSample)
	int getSampleNumber();
};

#endif
