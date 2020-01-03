#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include <vector>
#include <cstdint>

class Spectrogram {
	
	const std::vector<double> *waveform;
	
	int sampleRate;
	int numChannels;
	
	int N;
	int n;
	int overlap;

	// function pointer to window function
	std::vector<double>(*window_f)(double, int32_t);

	std::vector<std::vector<std::vector<double>>> spectrogram;
	
	Spectrogram(const std::vector<double> *waveform, int sampleRate, int numChannels, int N,
		int n, int overlap, std::vector<double>(*window_f)(double, int32_t));
	~Spectrogram();
	void computeSpectrogram();
	void computeSpectrogram(int N, int n, int overlap, std::vector<double>(*window_f)(double, int32_t)); //recompute spectrogram for same wav but with different options
};

#endif
