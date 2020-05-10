#include <vector>
#include <cmath>
#include <complex>

#include "DFT.h"

double hann (double signal, size_t j, size_t window) {
	double multiplier = std::sin(M_PI * j / (window - 1));
	multiplier = multiplier * multiplier;
	return signal * multiplier;
}

std::vector<std::complex<double>> slowDFT (size_t index, size_t window, size_t channel, size_t numChannels, const std::vector<double> &signal) {
	std::vector<std::complex<double>> toReturn;
	toReturn.reserve(window);
	size_t startIndex = index*numChannels + channel - 1;
	for (size_t k = 0; k < window; ++k) {
		std::complex<double> sum(0, 0);
		for (size_t j = 0; j < window; ++j) {
			std::complex<double> exponent = 2i * M_PI * j * k / window;
			sum += signal.at(startIndex + j*numChannels) * std::exp(-exponent);
		}
		toReturn.push_back(sum);
	}
	return toReturn;
}

std::vector<std::complex<double>> rad2DFT (size_t index, size_t window, size_t channel, size_t numChannels, const std::vector<double> &signal) {
	if (window % 2 != 0) throw "window needs to be a power of 2";
	if (window <= 32) return slowDFT(index, window, channel, numChannels, signal);
	
	std::vector<std::complex<double>> even = rad2DFT(index / 2, window / 2, channel, numChannels * 2, signal);
	std::vector<std::complex<double>> odd = rad2DFT(index / 2, window / 2, channel + numChannels, numChannels * 2, signal);

	std::vector<std::complex<double>> toReturn;
	toReturn.reserve(window);

	for (size_t i = 0; i < window; ++i) {
		size_t m = i % (window / 2);
		std::complex<double> exponent = 2i * i * M_PI / window;
		toReturn.push_back(even[m] + std::exp(-exponent) * odd[m]);
	}
	
	return toReturn;
}

std::vector<std::complex<double>> slowWindowedDFT (size_t index, size_t window, size_t channel, size_t numChannels, const std::vector<double> &signal, double (*windowFunction) (double, size_t, size_t)) {
	std::vector<double> windowedSignal;
	windowedSignal.reserve(window);
	size_t startIndex = index * numChannels + channel - 1;
	for (size_t j = 0; j < window; ++j) {
		windowedSignal.push_back((*windowFunction)(signal[startIndex + j * numChannels], j, window));
	}
	return slowDFT(0, window, 1, 1, windowedSignal);
}

std::vector<std::complex<double>> windowedDFT (size_t index, size_t window, size_t channel, size_t numChannels, const std::vector<double> &signal, double (*windowFunction) (double, size_t, size_t)) {
	std::vector<double> windowedSignal;
	windowedSignal.reserve(window);
	size_t startIndex = index * numChannels + channel - 1;
	for (size_t j = 0; j < window; ++j) {
		windowedSignal.push_back((*windowFunction)(signal[startIndex + j * numChannels], j, window));
	}
	return rad2DFT(0, window, 1, 1, windowedSignal);
}
