#include <fstream>
#include <iostream>
#include <string>
#include <cstdio>

#include "portaudio.h"

#include "WAVFile.h"
#include "WAVPlayer.h"
#include "WaveformDisplay.h"

int main(int argc, char* argv[]) {
	freopen ("logError.txt","w",stderr);	
	if (argc < 2) {
		std::cout << "Usage: ./WaveformPlayer FILE [OPTION]" << std::endl;
		std::cout << "Try './WaveformPlayer -help' for details" << std::endl;
		return 1;
	} else if (strcmp(argv[1], "-help") == 0) {
		std::cout << "This program plays a .wav file and displays some visuals. Press 'q' to stop playing. This program calculates an entire spectrogram for the file, which can take a lot of RAM (a 4 minute 2-channel file at 44100 Hz with window = 4096 and overlap = 2048 took about 2.2 GB. Disable the spectrum by using the option 'nospec' to use less RAM." << std::endl;
		std::cout << "Options:" << std::endl;
		std::cout << "  -debug : Generates logs for data of the WAV file. 'logNormData.txt' contains the data chunk normalized, 'logHeader.txt' contains header information, 'logRawBytes.txt' displays the data chunk in hex, and 'logNumData.txt' displays the data chunk numerically." << std::endl;
		std::cout << "  -nospec : Does not generate spectrogram or display spectrum while playing." << std::endl;
		std::cout << "  -custom_spec : will prompt the user to enter an integer for the window and overlap (both must be positive integers, with window > overlap). This means each frame is window - overlap apart. Will also ask if a Hann window will be used." << std::endl;
		std::cout << "  By default (no options), uses window = 4096, overlap = 2048, with Hann windowing." << std::endl;
		return 0;
	}

	try {
		WAVFile theFile{argv[1]};
		bool spec = true;
		int window = 2048;
		int overlap = 1024;
		bool use_hann = true;
		if (argc >= 3) {
			if (strcmp(argv[2], "-debug") == 0) {
				std::cout << "Generating logs ..." << std::endl;
				std::cout << "Generating logNormData.txt ..." << std::endl;
				std::ofstream normData{"logNormData.txt"};
				normData << argv[1] << std::endl;
				theFile.printNormData(normData);
				normData.close();
				
				std::cout << "Generating logHeader.txt ..." << std::endl;
				std::ofstream header{"logHeader.txt"};
				header << argv[1] << std::endl;
				theFile.printHeader(header);
				header.close();
				
				std::cout << "Generating logRawBytes.txt ..." << std::endl;
				std::ofstream rawBytes{"logRawBytes.txt"};
				rawBytes << argv[1] << std::endl;
				theFile.printData(rawBytes);
				rawBytes.close();
				
				std::cout << "Generating logNumData.txt ..." << std::endl;
				std::ofstream numData{"logNumData.txt"};
				numData << argv[1] << std::endl;
				theFile.printNumData(numData);
				numData.close();
			} else if (strcmp(argv[2], "-nospec") == 0) {
				spec = false;
			} else if (strcmp(argv[2], "-custom_spec") == 0) {
				std::cout << "Enter window:" << std::endl;
				std::cin >> window;
				std::cout << "Enter overlap:" << std::endl;
				std::cin >> overlap;
				std::cout << "Use Hann window? (y/n)" << std::endl;
				char c;
				std::cin >> c;
				use_hann = (c == 'y');
			}
		}
		if (spec) {
			theFile.calculateSpectrogram(window, overlap, use_hann);
		}
		WAVPlayer player{&theFile};
		WaveformDisplay display{&theFile, &player};
		player.display = &display;
		player.play();

	} catch (char const *e) {
		std::cout << e << std::endl;
	}
	
	return 0;
}
