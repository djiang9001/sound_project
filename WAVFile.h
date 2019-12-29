#ifndef WAVFILE_H
#define WAVFILE_H

#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

#include "ReadInt.h"

// WAVFile will be the default class for an audio file, other file formats
//   will have classes that inherit from WAVFile, and probably will be the same
//   but will convert all the data in the destructor to be compatible

// Assumes the file is a PCM WAVFile

struct WAVFile {

	std::string path;
	
	char chunkID[4];
	uint32_t chunkSize;
	char format[4];

	char subchunkID1[4];
	uint32_t subchunkSize1;
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
	// extraParams skipped

	uint16_t extSize;
	uint16_t validBitsPerSample;
	uint32_t channelMask;
	uint16_t GUID;
	char GUIDjunk[16];
	
	char fact[4];
	uint32_t factSize;
	uint32_t numSamplesPerChannel;

	char subchunkID2[4];
	uint32_t subchunkSize2;
	std::vector<unsigned char> data;

	uint16_t bytesPerSample;// bytes needed to hold each sample. bitsPerSample / 8 rounded up
	uint32_t numSamples;
	
	WAVFile(std::string path);
	virtual ~WAVFile();

	//assumes that all input integers are multiples of 8 bit integers, and that
	//bytesPerSample is the size of the input integer.
	template<typename T> void getSample(T *dest, const size_t &i) {
		if (typeid(T) != typeid(double)) {
			*dest = 0;
			std::memcpy(dest, &(data[i * bytesPerSample]), bytesPerSample);
			castInt(dest, bytesPerSample);
		} else {
			//we just use doubles and convert the sig and man manually
			
		}
	}
	// performs fn on sample start inclusive to sample end exclusive
	template<typename T, typename F> void mapNumData(const size_t &start, const size_t &end, F fn) {
		for (size_t i = start; i < end; ++i) {
			T sample;
			getSample<T>(&sample, i);
			fn(&sample);
		}	
	}

	void printHeader();
	void printData();
	void printNumData();
};

#endif
