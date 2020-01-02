#ifndef WAVFILE_H
#define WAVFILE_H

#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <iostream>

#include "ReadInt.h"

// WAVFile will be the default class for an audio file, other file formats
//   will have classes that inherit from WAVFile, and probably will be the same
//   but will convert all the data in the destructor to be compatible

// Assumes the file is a PCM WAVFile

struct WAVFile {

	static const uint8_t MAX_UINT8_WAV = 255;
	static const uint8_t MIN_UINT8_WAV = 0;

	static const int16_t MAX_INT16_WAV = 32767;
	//int16_t MIN_INT16_WAV = -32768;

	static const int32_t MAX_INT24_WAV = 8388607;
	//int32_t MIN_INT24_WAV = -8388608;

	static const int32_t MAX_INT32_WAV = 2147483647;
	//int32_t MIN_INT32_WAV = -214748348;

	static const int64_t MAX_INT64_WAV = 9223372036854775807;
	//int64_t MIN_INT64_WAV = -9223372036854775808;

	static constexpr const double F_8 = (double) 1 / MAX_UINT8_WAV;
	static constexpr const double F_16 = (double) 1 / MAX_INT16_WAV;
	static constexpr const double F_24 = (double) 1 / MAX_INT24_WAV;
	static constexpr const double F_32 = (double) 1 / MAX_INT32_WAV;
	static constexpr const double F_64 = (double) 1 / MAX_INT64_WAV;

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

	//vector of doubles that will be used for calculations
	std::vector<double> normData;
	
	WAVFile(std::string path);
	virtual ~WAVFile();

	//assumes that all input integers are multiples of 8 bit integers, and that
	//bytesPerSample is the size of the input integer.
	template<typename T> void getSample(T *dest, const size_t &i) {
		*dest = 0;
		std::memcpy(dest, &(data[i * bytesPerSample]), bytesPerSample);
		castInt(dest, bytesPerSample);
	}
	
	template<typename T> void getFloatSample (T *dest, const size_t &i) {
		*dest = 0;
		std::memcpy(dest, &(data[i * bytesPerSample]), bytesPerSample);

	}
	// performs fn on sample start inclusive to sample end exclusive
	template<typename T, typename F> void mapNumData(const size_t &start, const size_t &end, F fn) {
		for (size_t i = start; i < end; ++i) {
			T sample;
			getSample<T>(&sample, i);
			fn(&sample);
		}
	}

	template<typename T, typename F> void mapFloatData(const size_t &start, const size_t &end, F fn) {
		for (size_t i = start; i < end; ++i) {
			T sample;
			getFloatSample<T>(&sample, i);
			fn(&sample);
		}
	}

	template <typename F> void mapData(const size_t &start, const size_t &end, F fn) {
	        //size_t i = 0;
	        if (audioFormat == 1) {
	                if (bitsPerSample <= 8) {
	                        mapNumData<uint8_t>(0, numSamples, fn);
	                } else if (bitsPerSample <= 16) {
	                        mapNumData<int16_t>(0, numSamples, fn);
	                } else if (bitsPerSample <= 32) {
	                        mapNumData<int32_t>(0, numSamples, fn);
	                } else if (bitsPerSample <= 64) {
	                        mapNumData<int64_t>(0, numSamples, fn);
	                } else {        
	                        throw "bytesPerSample too big";
	                }               
	        } else if (audioFormat == 3) {
	                if (bitsPerSample == 32) {
	                        mapFloatData<float>(0, numSamples, fn);
	                } else if (bitsPerSample == 64) {
	                        mapFloatData<double>(0, numSamples, fn);
	                } else {
	                        throw "Unsupported floating point format";
	                }
	        } else {
	                throw "Unsupported format";
	                return;
	        }
	
	}

	template<typename T> double normedData (const T &n) {
		double toReturn = static_cast<double>(n) * getFactor();
		return toReturn;
	}

	double getFactor();
	void printHeader();
	void printData();
	void printNumData();
	void normalizeData();
};

#endif
