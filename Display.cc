#include "Display.h"
#include "WAVFile.h"
#include "WAVPlayer.h"

Display::Display(WAVFile *theFile, WAVPlayer *player): theFile{theFile}, player{player}, 
	path{theFile->path}, sampleRate{theFile->sampleRate}, numChannels{theFile->numChannels} {}

Display::~Display() {}
