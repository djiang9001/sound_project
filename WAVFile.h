#ifndef WAVFILE_H
#define WAVFILE_H

#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <functional>
#include <map>
#include <fstream>
#include <complex>

#include "ReadInt.h"

// WAVFile will be the default class for an audio file, other file formats
//   will have classes that inherit from WAVFile, and probably will be the same
//   but will convert all the data in the destructor to be compatible

// Assumes the file is an 8, 12/16, 16, 24, 32, 64-bit PCM WAVFile or a 32, 64-bit IEEE float WAVFile
// Assumes that double, float are 64, 32-bit and follow IEEE standards
// Assumes that the machine has little endian architecture

// On construction, populates normData vector with double values from -1.0 to 1.0
struct WAVFile {

	// Conversion to float constants

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

	// file name/path
	
	std::string path;
	std::ifstream f;
	
	// WAV header data

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

	// Convenient data calculated from header data

	uint16_t bytesPerSample;// bytes needed to hold each sample. bitsPerSample / 8 rounded up
	uint32_t numSamples;

	// temp vars for holding input
	char tempID[4];
	uint32_t tempSize;
	
	// reading functions and the map

	void read_fmt();
	void read_fact();
	void read_data();
	void read_LIST();

	typedef void(WAVFile::*p_func)();

	const std::map<std::string, p_func> func_map = {
		{"fmt ", &WAVFile::read_fmt},
		{"fact", &WAVFile::read_fact},
		{"data", &WAVFile::read_data},
		{"LIST", &WAVFile::read_LIST}
	};
	
	// vector of normed (-1.0, 1.0) doubles that will be used for calculations
	std::vector<double> normData;

	// Full spectrogram of audio, for each [channel][time][frequency]
	std::vector<std::vector<std::vector<std::complex<double>>>> spectrogram;
	int spectrogram_window = 0;
	int spectrogram_overlap = 0;
	
	WAVFile(std::string path);
	virtual ~WAVFile();

	void readChunk(); //reads the next chunk, skips if unrecognized, or writes to vars

	// Template functions for reading the data from the WAV, templated on integer type and/or fn

	//assumes that all input integers are multiples of 8 bit integers, and that
	//bytesPerSample is the size of the input integer.
	template<typename T> void getSample(T *dest, const size_t &i) {
		//*dest = 0;
		//std::memcpy(dest, &(data[i * bytesPerSample]), bytesPerSample);
		*dest = data[i * bytesPerSample + bytesPerSample - 1];
		for (int j = bytesPerSample - 2; j >= 0; --j) {
			*dest = (*dest << 8) | data[i * bytesPerSample + j];
		}
		castInt(dest, bytesPerSample);
	}
	
	template<typename T> void getFloatSample (T *dest, const size_t &i) {
		//*dest = 0;
		//std::memcpy(dest, &(data[i * bytesPerSample]), bytesPerSample);
		if (sizeof(T) == sizeof(uint32_t)) {
			*(uint32_t*)dest = data[i * bytesPerSample + bytesPerSample - 1];
			for (int j = bytesPerSample - 2; j >= 0; --j) {
				*(uint32_t*)dest = (*(uint32_t*)dest << 8) | data[i * bytesPerSample + j];
			}
		} else if (sizeof(T) == sizeof(uint64_t)) {
			*(uint64_t*)dest = data[i * bytesPerSample + bytesPerSample - 1];
			for (int j = bytesPerSample - 2; j >= 0; --j) {
				*(uint64_t*)dest = (*(uint64_t*)dest << 8) | data[i * bytesPerSample + j];
			}
		} else {
			throw "float/double int/long size does not match";
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

	template<typename T> double normedSample(const T &n) {
		double toReturn;
		if (typeid(T) == typeid(uint8_t)) {
			toReturn = static_cast<double>(n) * 2 * getFactor() - 1;
		} else {
			toReturn = static_cast<double>(n) * getFactor();
		}
		return toReturn;
	}

	// Defined in .cc

	double getFactor();

	// from normData, get item at index (i * numChannels + channelNum)
	double getSampleFromChannel(uint16_t channelNum, int i);
	
	void printHeader(std::ostream &out = std::cout);
	void printData(std::ostream &out = std::cout);
	void printNormData(std::ostream &out = std::cout);
	void printNumData(std::ostream &out = std::cout);
	void normalizeData();
	void calculateSpectrogram(int window = 8192, int overlap = 4096, bool use_hann = true);
};

#endif
