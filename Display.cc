#include <cstdint>
#include <string>
#include <vector>

#include "Display.h"
#include "WAVFile.h"
#include "WAVPlayer.h"

Display::Display(WAVFile *theFile, WAVPlayer *player): theFile{theFile}, player{player} {}

WaveformDisplay::WaveformDisplay(WAVFile *theFile, WAVPlayer *player): Display{theFile, player} {}

void WaveformDisplay::update(uint32_t currentSample) {

	// update display based on data
	std::cout << "currentSample: " << std::to_string(currentSample); 
	for (int i = 0; i < theFile->numChannels; ++i) {
		std::cout << " " << i << " : " << std::to_string(theFile->normData[currentSample]);
	}
	std::cout << std::endl;
	
}
