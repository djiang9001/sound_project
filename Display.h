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

	public:

	Display(WAVFile *theFile, WAVPlayer *player);
	
	virtual void update(uint32_t currentSample) = 0;
};

class WaveformDisplay: public Display {
	
	public:

	WaveformDisplay(WAVFile *theFile, WAVPlayer *player);
	void update(uint32_t currentSample) override;
};

#endif
