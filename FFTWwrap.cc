#include <complex>
#include <string>
#include <fftw3.h>
#include "FFTWwrap.h"

double hann (double signal, size_t j, size_t window) {
	double multiplier = std::sin(M_PI * j / (window - 1));
	multiplier = multiplier * multiplier;
	return signal * multiplier;
}

std::vector<std::complex<double>> FFTW_DFT(size_t index, size_t window, const std::vector<double> &signal, std::string window_func) {
	fftw_complex *in, *out;
	fftw_plan p;

	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window);
	p = fftw_plan_dft_1d(window, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	for (size_t i = 0; i < window; ++i) {
		in[i][1] = 0;
		in[i][0] = signal[i + index];
	}
	if (window_func == "hann") {
		for (int i = 0; i < window; ++i) {
			in[i][0] = hann(in[i][0], i, window);
		}
	}
	fftw_execute(p);
	std::vector<std::complex<double>> output;
	output.reserve(window);
	for (size_t i = 0; i < window; ++i) {
		output.emplace_back(out[i][0], out[i][1]);
	}
	return output;
}
