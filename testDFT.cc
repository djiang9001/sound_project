#include <iostream>
#include <complex>
#include <fstream>

#include <fftw3.h>

#include "DFT.h"
#include "WAVFile.h"

void outputFFTW(size_t index, size_t window, size_t channel, const WAVFile &theFile, std::vector<std::complex<double>> &output) {
	fftw_complex *in, *out;
	fftw_plan p;

	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window);
	p = fftw_plan_dft_1d(window, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	size_t j = 0;
	size_t startIndex = index * theFile.numChannels + channel - 1;
	for (size_t i = 0; i < window; ++i) {
		in[j][1] = 0;
		in[j++][0] = theFile.normData[startIndex + i * theFile.numChannels];
	}

	fftw_execute(p);

	std::ofstream f{"FFTW_test.txt"};
	
	for (size_t i = 0; i < window; ++i) {
		f << "bin " << i << " (" << out[i][0] << "," << out[i][1] << ")" << std::endl;
		output.push_back(std::complex<double>(out[i][0], out[i][1]));
	}

	fftw_destroy_plan(p);
	fftw_free(in);
	fftw_free(out);
	return;
}

double maxDiff (const std::vector<std::complex<double>> a, const std::vector<std::complex<double>> b) {
	double max = 0;
	for (size_t i = 0; i < std::min(a.size(), b.size()); ++i) {
		double diff = std::abs(a[i] - b[i]);
		if (diff > max) max = diff;
	}
	return max;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cout << "Need WAV file name as argument" << std::endl;
		return 1;
	}
	try {
		WAVFile theFile{argv[1]};
		
		size_t index = 0;//1000;
		size_t window = 8192;
		size_t channel = 2;
		
		std::vector<std::complex<double>> transformFFTW;
		outputFFTW(index, window, channel, theFile, transformFFTW);

		std::vector<std::complex<double>> transform = slowDFT(index, window, channel, theFile.numChannels, theFile.normData);
		std::ofstream out{"DFT_test_good.txt"};
		for (size_t i = 0; i < window; ++i) {
			out << "bin " << i << " " << transform[i] << std::endl;
		}
		std::cout << maxDiff(transformFFTW, transform) << std::endl;

		std::vector<std::complex<double>> transformRec = rad2DFT(index, window, channel, theFile.numChannels, theFile.normData);
		std::ofstream out2{"DFT_test_rec.txt"};
		for (size_t i = 0; i < window; ++i) {
			out2 << "bin " << i << " " << transformRec[i] << std::endl;
		}
		std::cout << maxDiff(transform, transformRec) << std::endl;
		
		std::vector<std::complex<double>> transformHann = windowedDFT(index, window, channel, theFile.numChannels, theFile.normData, hann);
		std::ofstream out3{"DFT_test_hann.txt"};
		for (size_t i = 0; i < window; ++i) {
			out3 << "bin " << i << " " << transformHann[i] << std::endl;
		}
		
		std::vector<std::complex<double>> transformHannSlow = slowWindowedDFT(index, window, channel, theFile.numChannels, theFile.normData, hann);
		std::ofstream out4{"DFT_test_hann_slow.txt"};
		for (size_t i = 0; i < window; ++i) {
			out4 << "bin " << i << " " << transformHann[i] << std::endl;
		}
		std::cout << maxDiff(transformHann, transformHannSlow) << std::endl;


		std::vector<std::complex<double>> transformIndex = rad2DFT(index, window, channel, theFile.numChannels, theFile.normData);
		std::vector<std::complex<double>> transformIndex0 = rad2DFT(0, window, channel, theFile.numChannels, theFile.normData);
		std::vector<std::complex<double>> transformSlid = transformIndex0;
	        for (int i = 0; i < index; ++i) {
			transformSlid = nextDFT(i, window, channel, theFile.numChannels, theFile.normData, transformSlid);
		}
		std::ofstream out5{"DFT_test_index1000.txt"};
		for (size_t i = 0; i < window; ++i) {
			out5 << "bin " << i << " " << transformIndex[i] << std::endl;
		}
		std::ofstream out6{"DFT_test_slid1000.txt"};
		for (size_t i = 0; i < window; ++i) {
			out6 << "bin " << i << " " << transformSlid[i] << std::endl;
		} 
		std::cout << maxDiff(transformIndex, transformSlid) << std::endl;
		/*std::cout << "signal" << std::endl;
		for (size_t i = 0; i < window; ++i) {
			std::cout << hann(channel_1.at(index + i), i, window) << std::endl;
		}*/
	} catch (char const *e) {
		std::cout << e << std::endl;
	}
	return 0;
}

