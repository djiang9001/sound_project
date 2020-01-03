#include <vector>

#include "Spectrogram.h"

Spectrogram::Spectrogram(const std::vector<double> *waveform, int sampleRate, int numChannels, int N,
	int n, int overlap, std::vector<double>(*window_f)(double, int32_t)): waveform{waveform},
		sampleRate{sampleRate}, numChannels{numChannels}, N{N}, n{n}, overlap{overlap}, 
		window_f{window_f} {
	computeSpectrogram();
}

void Spectrogram::computeSpectrogram() {

}

void Spectrogram::computeSpectrogram(int N, int n, int overlap, std::vector<double>(*window_f)(double, int32_t)) {
	this->N = N;
	this->n = n;
	this->overlap = overlap;
	this->window_f = window_f;
}
