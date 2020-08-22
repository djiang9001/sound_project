#include <string>
#include <curses.h>
#include <cstdint>
#include <complex>
#include <cmath>

#include "DFT.h"
#include "WaveformDisplay.h"
#include "WAVFile.h"

WaveformDisplay::WaveformDisplay(WAVFile *theFile, WAVPlayer *player): Display{theFile, player} {
	if (!stdscr) {
		initscr();
	}
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	curs_set(0);
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
	wmove(botBar, 0, 0);
	wclrtoeol(botBar);
	mvwaddstr(botBar, 0, 0, bot.c_str());
	wrefresh(botBar);
}

void WaveformDisplay::update(int currentSample) {
	int c = getch();
	if (c == 'q') quit = true;
	/*
	uint32_t latSamples = latency * sampleRate / numChannels;
	if (currentSample < latSamples) {
		this->currentSample = 0;
	} else {
		this->currentSample = currentSample - latSamples;
	}*/
	this->currentSample = currentSample - latency * sampleRate;//Compensate for latency, else the display is too early
	if (this->currentSample < 0) this->currentSample = 0;
	move (1, 0);
	addstr("Latency: ");
	addstr(std::to_string(latency).c_str());
	for (int i = 0; i < numChannels; ++i) {
		move (i + 2, 0);
		uint32_t index = this->currentSample * numChannels + i;
		int length;
		if (index >= theFile->normData.size()) {
			length = 0;
		} else {
			length = theFile->normData[index] * col;
			if (length < 0) length *= -1;
		}
		clrtoeol();
		//std::wcout <<std::wstring(length, static_cast<wchar_t>(219)) << std::endl;
		addstr(std::string(length, '#').c_str());
	}
	if (theFile->spectrogram_window != 0) {
		// Draw spectrum
		std::vector<std::complex<double>> spectrum = theFile->spectrogram[0]
			[this->currentSample / (theFile->spectrogram_window - theFile->spectrogram_overlap)];
		int max_height = row - 3 - numChannels;
		std::vector<double> magnitudes;
		magnitudes.reserve(spectrum.size()/2);
		for (int i = 0; i < spectrum.size()/2; ++i) {
			magnitudes.push_back(std::abs(spectrum[i]));
		}
		std::vector<double> bin_magnitudes(col, 0.0);
		int per_bin = magnitudes.size() / col;
		int s = 0;
		double max_magnitude = 0;
		for (int i = 0; i < bin_magnitudes.size(); ++i) {
			for (int j = 0; j < per_bin; ++j) {
				if (bin_magnitudes[i] < magnitudes[s]) {
					bin_magnitudes[i] = magnitudes[s];
				}
				++s;
			}
			if (bin_magnitudes[i] > max_magnitude) {
				max_magnitude = bin_magnitudes[i];
			}
		}
		while (s < magnitudes.size()) {
			if (bin_magnitudes[bin_magnitudes.size() - 1] < magnitudes[s]) {
				bin_magnitudes[bin_magnitudes.size() - 1] = magnitudes[s];
			}
			++s;
		}
		if (bin_magnitudes[bin_magnitudes.size() - 1] > max_magnitude) {
			max_magnitude = bin_magnitudes[bin_magnitudes.size() - 1];
		}
		std::vector<int> heights;
		heights.reserve(bin_magnitudes.size());
		for (int i = 0; i < bin_magnitudes.size(); ++i) {
			if (max_magnitude > 0) {
				heights.push_back(max_height * bin_magnitudes[i] / max_magnitude);
			} else {
				heights.push_back(0);
			}
		}
		// Draw the spectrogram
		
		for (int i = 0; i < max_height; ++i) {
			move(i + 2 + numChannels, 0);
			for (int j = 0; j < col; ++j) {
				if (heights[j] >= max_height - i) {
					addch('X');
				} else {
					addch(' ');
				}
			}
		}
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

