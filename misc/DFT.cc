#include <vector>
#include <cmath>
#include <complex>

#include "DFT.h"

using namespace std::complex_literals;
/*
double hann (double signal, size_t j, size_t window) {
	double multiplier = std::sin(M_PI * j / (window - 1));
	multiplier = multiplier * multiplier;
	return signal * multiplier;
}
*/
std::vector<std::complex<double>> slowDFT (size_t index, size_t window, size_t channel, size_t numChannels, const std::vector<double> &signal) {
	std::vector<std::complex<double>> toReturn;
	toReturn.reserve(window);
	size_t startIndex = index*numChannels + channel;
	for (size_t k = 0; k < window; ++k) {
		std::complex<double> sum(0, 0);
		for (size_t j = 0; j < window; ++j) {
			std::complex<double> exponent = M_PI * j * k / window * 2i;
			sum += signal.at(startIndex + j*numChannels) * std::exp(-exponent);
		}
		toReturn.push_back(sum);
	}
	return toReturn;
}

std::vector<std::complex<double>> recRad2DFT (size_t index, size_t window, size_t channel, size_t numChannels, const std::vector<double> &signal) {
	if (window % 2 != 0) throw "window needs to be a power of 2";
	if (window <= 32) return slowDFT(0, window, channel, numChannels, signal);
	
	std::vector<std::complex<double>> even = rad2DFT(index / 2, window / 2, channel, numChannels * 2, signal);
	std::vector<std::complex<double>> odd = rad2DFT(index / 2, window / 2, channel + numChannels, numChannels * 2, signal);

	std::vector<std::complex<double>> toReturn;
	toReturn.reserve(window);

	for (size_t i = 0; i < window; ++i) {
		size_t m = i % (window / 2);
		std::complex<double> exponent = i * M_PI / window * 2i;
		toReturn.push_back(even[m] + std::exp(-exponent) * odd[m]);
	}
	
	return toReturn;
}

std::vector<std::complex<double>> rad2DFT (size_t index, size_t window, size_t channel, size_t numChannels, const std::vector<double> &signal) {
	std::vector<double> simplifiedSignal;
	simplifiedSignal.reserve(window);
	for (size_t i = 0; i < window; ++i) {
		simplifiedSignal.push_back(signal[(index + i) * numChannels + channel]);
	}
	return recRad2DFT(0, window, 0, 1, simplifiedSignal);
}

std::vector<std::complex<double>> slowWindowedDFT (size_t index, size_t window, size_t channel, size_t numChannels, const std::vector<double> &signal, double (*windowFunction) (double, size_t, size_t)) {
	std::vector<double> windowedSignal;
	windowedSignal.reserve(window);
	size_t startIndex = index * numChannels + channel;
	for (size_t j = 0; j < window; ++j) {
		windowedSignal.push_back((*windowFunction)(signal[startIndex + j * numChannels], j, window));
	}
	return slowDFT(0, window, 0, 1, windowedSignal);
}

std::vector<std::complex<double>> windowedDFT (size_t index, size_t window, size_t channel, size_t numChannels, const std::vector<double> &signal, double (*windowFunction) (double, size_t, size_t)) {
	std::vector<double> windowedSignal;
	windowedSignal.reserve(window);
	size_t startIndex = index * numChannels + channel;
	for (size_t j = 0; j < window; ++j) {
		windowedSignal.push_back((*windowFunction)(signal[startIndex + j * numChannels], j, window));
	}
	return rad2DFT(0, window, 0, 1, windowedSignal);
}

std::vector<std::complex<double>> nextDFT (size_t index, size_t window, size_t channel, size_t numChannels, const std::vector<double> &signal, const std::vector<std::complex<double>> &oldTransform) {
	std::vector<std::complex<double>> toReturn;
	toReturn.reserve(window);
	double nextSignal = 0;
	size_t nextIndex = (index + window) * numChannels + channel;
	size_t firstIndex = index * numChannels + channel;
	if (nextIndex < signal.size()) {
		nextSignal = signal[nextIndex];
	}
	for (size_t j = 0; j < oldTransform.size(); ++j) {
		std::complex<double> exponent = M_PI * j / window * 2i;
		std::complex<double> next = std::exp(exponent) * (oldTransform[j] - signal[firstIndex] + nextSignal);
		toReturn.push_back(next);
	}

	return toReturn;
}
