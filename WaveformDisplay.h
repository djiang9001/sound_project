#ifndef WAVEFORMDISPLAY_H
#define WAVEFORMDISPLAY_H

#include <curses.h>

#include "Display.h"

class WaveformDisplay: public Display {

	int row, col;

	WINDOW *topBar;
	WINDOW *botBar;

	int middle;

	void setTop();
	void setBot();
	void setPositions();
	void resize();	

	public:

	WaveformDisplay(WAVFile *theFile, WAVPlayer *player);
	~WaveformDisplay();
	void update(uint32_t currentSample, double latency) override;
};

#endif
