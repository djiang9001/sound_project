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
	if (argc == 1) {
		std::cout << "Needs filename as arg." << std::endl;
		return 1;
	}

	try {
		WAVFile theFile{argv[1]};
		if (argc > 2) {
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
			} else if (strcmp(argv[2], "-spec") == 0) {
				theFile.calculateSpectrogram(8192, 4096);
			}
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
