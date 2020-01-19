#include <fstream>
#include <iostream>
#include <string>

#include "portaudio.h"

#include "WAVFile.h"
#include "WAVPlayer.h"
#include "WaveformDisplay.h"

int main(int argc, char* argv[]) {
	freopen ("error.log","w",stderr);	
	if (argc == 1) {
		std::cout << "Needs filename as arg." << std::endl;
		return 1;
	}

	try {
		WAVFile theFile{argv[1]};
		if (argc > 2) {
			std::ofstream f{"normData.txt"};
			f << argv[1] << std::endl;
			f << "numChannels " << theFile.numChannels << std::endl;
			f << "numSamplesPerChannel " << theFile.numSamplesPerChannel << std::endl;
			f << "subchunkSize2 " << theFile.subchunkSize2 << std::endl;
			f << "numSamples " << theFile.numSamples << std::endl;
			f << "normData.size() " << theFile.normData.size() << std::endl;
			for (size_t i = 0; i < theFile.normData.size(); ++i) {
				f << theFile.normData[i] << " ";
				if (i % 2 != 0) f << std::endl;
			}
			f << std::endl;
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
