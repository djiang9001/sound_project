#ifndef FFTW_WRAP_H
#define FFTW_WRAP_H

#include <vector>
#include <complex>
#include <string>

std::vector<std::complex<double>> FFTW_DFT(size_t index, size_t window, const std::vector<double> &signal, std::string window_func = "");

#endif
