#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>
#include <iostream>
#include <string>

class WAVFile;
class WAVPlayer;

class Display {
	
	protected:

	WAVFile *theFile;
	WAVPlayer *player;
	std::string path;
	uint32_t sampleRate;
	uint16_t numChannels;

	uint32_t currentSample;

	public:

	Display(WAVFile *theFile, WAVPlayer *player);
	virtual ~Display();
	virtual void update(uint32_t currentSample, double latency) = 0;
};

#endif
