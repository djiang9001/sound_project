#ifndef WAVEFORMDISPLAY_H
#define WAVEFORMDISPLAY_H

#include <curses.h>

#include "Display.h"

class WaveformDisplay: public Display {

	int row, col;

	WINDOW *topBar;
	WINDOW *botBar;

	void setTop();
	void setBot();
	void resize();	

	public:

	WaveformDisplay(WAVFile *theFile, WAVPlayer *player);
	~WaveformDisplay();
	void update(int currentSample) override;
};

#endif
