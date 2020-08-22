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

	int currentSample;
	double latency;

	bool quit;

	public:

	Display(WAVFile *theFile, WAVPlayer *player);
	virtual ~Display();
	virtual void update(int currentSample) = 0;
	bool getQuit();
	void setLatency(double latency);
};

#endif
