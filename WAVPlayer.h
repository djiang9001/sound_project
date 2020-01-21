#ifndef WAVPLAYER_H
#define WAVPLAYER_H

#include <cstdint>
#include <vector>
#include "portaudio.h"

class Display;
class WAVFile;

class WAVPlayer {	

	WAVFile *theFile;
	
	uint16_t numChannels;
	uint32_t sampleRate;
	uint16_t blockAlign;
	uint16_t bytesPerSample;

	std::vector<double> *data;

	// A number from 0 to theFile->data.size()
	uint32_t currentDataIndex;

	static WAVPlayer *current;

	static int patestCallback(const void *inputBuffer, void *outputBuffer,
                                unsigned long framesPerBuffer,
                                const PaStreamCallbackTimeInfo* timeInfo,
                                PaStreamCallbackFlags statusFlags,
                                void *userData);

	public:
	
	Display *display;

	WAVPlayer(WAVFile *theFile);

	void play();
	
	// Returns a number from 0 to theFile->numSamplesPerChannel - 1
	// equal to currentDataIndex / (blockAlign / bytesPerSample)
	uint32_t getSampleNumber();
};

#endif
