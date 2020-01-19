#include <iostream>

#include "WAVFile.h"
#include "Spectrogram.h"

int main(int argc, char *argv[]) {
	try {
		//Note: these test files have an incorrect size for the afsp chunk
		//WAVFile file{"wav_files/test.wav"};
		//WAVFile file{"M1F1-int12-AFsp.wav"};
		//WAVFile file2{"wav_files/M1F1-int16-AFsp.wav"};
		//WAVFile file3{"M1F1-int32-AFsp.wav"};
		//WAVFile file4{"wav_files/M1F1-float32-AFsp.wav"};
		//WAVFile file5{"M1F1-int24-AFsp.wav"};
		//WAVFile file6{"M1F1-float64-AFsp.wav"};
		WAVFile file7{"wav_files/M1F1-uint8-AFsp.wav"};

		//WAVFile test32{"32_bit_float.WAV"};
		//WAVFile why{"why.wav"};
		//WAVFile cantina3{"wav_files/CantinaBand3.wav"};
	} catch (char const *e) {
		std::cout << "Exception: " << e << std::endl;
	}
	return 0;
}
