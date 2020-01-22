#include "Display.h"
#include "WAVFile.h"
#include "WAVPlayer.h"

Display::Display(WAVFile *theFile, WAVPlayer *player): theFile{theFile}, player{player},
	path{theFile->path}, sampleRate{theFile->sampleRate}, numChannels{theFile->numChannels},
	quit{false} {}

Display::~Display() {}

bool Display::getQuit() { return quit; }

void Display::setLatency(double latency) { this->latency = latency; }
