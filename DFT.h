#ifndef DFT_H
#define DFT_H

#include <vector>

// Hann window function
double hann (double signal, size_t j, size_t window);

// Return a vector of size window of the DFT of signal starting at index, where index is the
// sample number of the multi-channel signal
std::vector<std::complex<double>> slowDFT (size_t index, size_t window, size_t channel, size_t numChannels, const std::vector<double> &signal);

std::vector<std::complex<double>> recRad2DFT (size_t index, size_t window, size_t channel, size_t numChannels, const std::vector<double> &signal);

std::vector<std::complex<double>> rad2DFT (size_t index, size_t window, size_t channel, size_t numChannels, const std::vector<double> &signal);

std::vector<std::complex<double>> windowedDFT (size_t index, size_t window, size_t channel, size_t numChannels, const std::vector<double> &signal, double (*windowFunction) (double, size_t, size_t));

std::vector<std::complex<double>> slowWindowedDFT (size_t index, size_t window, size_t channel, size_t numChannels, const std::vector<double> &signal, double (*windowfunction) (double, size_t, size_t));

std::vector<std::complex<double>> nextDFT (size_t index, size_t window, size_t channel, size_t numChannels, const std::vector<double> &signal, const std::vector<std::complex<double>> &oldTransform);

#endif
