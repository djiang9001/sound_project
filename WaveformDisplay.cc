#include <string>
#include <cstdint>
#include <complex>
#include <cmath>
#include <csignal>

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
		std::to_string(numChannels) + " channels. Press 'q' to quit.";
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

void WaveformDisplay::resize() {
	getmaxyx(stdscr, row, col);
	delwin(botBar);
	delwin(topBar);
	botBar = subwin(stdscr, 1, 0, row - 1, 0);
	topBar = subwin(stdscr, 1, 0, 0, 0);
	setTop();
	refresh();
}

void WaveformDisplay::update(int currentSample) {
	int c = getch();
	if (c == 'q') { 
		quit = true;
	} else if (c == KEY_RESIZE) {
		resize();
	}
	this->currentSample = currentSample - latency * sampleRate * numChannels;//Compensate for latency, else the display is too early
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
		int overlapping_frames = (theFile->spectrogram_window * 2) / 
			(theFile->spectrogram_window - theFile->spectrogram_overlap) - 1;
		std::vector<double> magnitudes(theFile->spectrogram_window/2, 0.0);
		int max_height = row - 3 - numChannels;
		for (int c = 0; c < numChannels; ++c) {
			for (int j = 0; j < overlapping_frames; ++j) {
				int begin_frame = this->currentSample / 
					(theFile->spectrogram_window - theFile->spectrogram_overlap)
					- overlapping_frames / 2;
				int index;
				if (begin_frame + j < 0) {
					index = 0;
				} else if (begin_frame + j >= theFile->spectrogram[c].size()) {
					index = theFile->spectrogram[c].size() - 1;
				} else {
					index = begin_frame + j;
				}
				std::vector<std::complex<double>> spectrum = theFile->spectrogram[c]
					[index];
				for (int i = 0; i < magnitudes.size(); ++i) {
					magnitudes[i] += std::abs(spectrum[i]);
				}
			}
		}
		for (int i = 0; i < magnitudes.size(); ++i) {
			magnitudes[i] = magnitudes[i] / (overlapping_frames * numChannels);
		}
		double log_base = exp(log(theFile->spectrogram_window/2)/col);
		std::vector<double> bin_magnitudes(col, 0.0);
		int s = 0;
		int bin_number = 0;
		double max_magnitude = 0;
		for (int i = 0; i < magnitudes.size(); ++i) {
			int bin_new = (log(i + 1)/log(log_base));
			if (bin_new >= col) {
				bin_new = col - 1;
			}
			if (bin_new > bin_number) {
				for (int j = bin_number; j < bin_new; ++j) {
					bin_magnitudes[j] = bin_magnitudes[bin_number];
				}
			}
			bin_number = bin_new;
			if (bin_magnitudes[bin_new] < magnitudes[i]) {
				bin_magnitudes[bin_new] = magnitudes[i];
			}
			if (bin_magnitudes[bin_new] > max_magnitude) {
				max_magnitude = bin_magnitudes[bin_new];
			}
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


