#include <string>
#include <curses.h>
#include <cstdint>

#include "WaveformDisplay.h"
#include "WAVFile.h"

WaveformDisplay::WaveformDisplay(WAVFile *theFile, WAVPlayer *player): Display{theFile, player} {
	if (!stdscr) initscr();
	getmaxyx(stdscr, row, col);
	botBar = subwin(stdscr, 1, 0, row - 1, 0);
	topBar = subwin(stdscr, 1, 0, 0, 0);
	setTop();
	refresh();
}

WaveformDisplay::~WaveformDisplay() {
	delwin(botBar);
	delwin(topBar);
	if (!isendwin()) endwin();
}

void WaveformDisplay::setTop() {
	std::string top = "Playing \"" + path + "\" at " + std::to_string(sampleRate) + "Hz with " + 
		std::to_string(numChannels) + " channels";
	mvwaddstr(topBar, 0, 0, top.c_str());
	wrefresh(topBar);
}

void WaveformDisplay::setBot() {
	uint32_t timeSeconds = currentSample / sampleRate;
	uint32_t min = timeSeconds / 60;
	uint32_t sec = timeSeconds % 60;
	std::string bot = "Sample: " + std::to_string(currentSample) + 
		" Time: " + std::to_string(min) + ":" + std::string(sec < 10, '0') + std::to_string(sec);
	mvwaddstr(botBar, 0, 0, bot.c_str());
	wrefresh(botBar);
}
void WaveformDisplay::update(uint32_t currentSample) {
	this->currentSample = currentSample;
	int gap = (row - 2) / (numChannels + 1);
	for (int i = 0; i < numChannels; ++i) {
		move ((i + 1) * gap, 0);
		uint32_t index = currentSample * numChannels + i;
		int length;
		if (index >= theFile->normData.size()) {
			length = 0;
		} else {
			length = theFile->normData[index] * col;
			if (length < 0) length *= -1;
		}
		clrtoeol();
		addstr(std::string(length, static_cast<unsigned char>('\xDB')).c_str());
	}
	setBot();
	refresh();
	// update display based on data
	/*
	std::cout << "currentSample: " << std::to_string(currentSample);
	for (int i = 0; i < theFile->numChannels; ++i) {
		std::cout << " " << i << " : " << std::to_string(theFile->normData[currentSample]);
	}
	std::cout << std::endl;*/

}
