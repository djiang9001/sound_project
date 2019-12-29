#include <fstream>
#include <iostream>
#include <string>
#include <cstring>

#include "WAVFile.h"

WAVFile::WAVFile(std::string path): path{path} {
	std::ifstream f{path, std::ifstream::binary};
	
	f.read(chunkID, 4);
	f.read((char*)&chunkSize, 4);
	f.read(format, 4);	

	f.read(subchunkID1, 4);
	f.read((char*)&subchunkSize1, 4);
	f.read((char*)&audioFormat, 2);
	f.read((char*)&numChannels, 2);
	f.read((char*)&sampleRate, 4);
	f.read((char*)&byteRate, 4);
	f.read((char*)&blockAlign, 2);
	f.read((char*)&bitsPerSample, 2);
	
	//extension size for non-pcm
	if (subchunkSize1 > 16) {
		f.read((char*)&extSize, 2);
		if (extSize > 0) {
			f.read((char*)&validBitsPerSample, 2);
			f.read((char*)&channelMask, 4);
			f.read((char*)&GUID, 2);
			f.read(GUIDjunk, 16);
		} else {
			validBitsPerSample = bitsPerSample;
			channelMask = 0;
			GUID = 0;
			GUIDjunk[0] = 0;
		}
	} else {
		extSize = 0;
	}

	f.read(subchunkID2, 4);
	std::cout.write(subchunkID2, 4) << std::endl;
	if (strncmp(subchunkID2, "fact", 3) == 0) {
		fact[0] = 'f';
		fact[1] = 'a';
		fact[2] = 'c';
		fact[3] = 't';
		f.read((char*)&factSize, 4);
		f.read((char*)&numSamplesPerChannel, 4);
		subchunkID2[0] = 0;
		f.read(subchunkID2, 4);
	} else {
		fact[0] = 0;
		factSize = 0;
		numSamplesPerChannel = 0;
	}

	f.read((char*)&subchunkSize2, 4);
	data.reserve(subchunkSize2);
	f.read((char*)(&data[0]), subchunkSize2);

	bytesPerSample = (bitsPerSample + 7) / 8;
	numSamples = subchunkSize2 / bitsPerSample;

	printHeader();
	printData();
	printNumData();
}

void WAVFile::printHeader() {
	std::cout << "path: " << path << std::endl;
	//print the header
	std::cout << "chunkID: ";
	std::cout.write(chunkID, 4) << std::endl;
	std::cout << "chunkSize: " << std::to_string(chunkSize) << std::endl;
	std::cout << "format: ";
	std::cout.write(format, 4) << std::endl;

	std::cout << "subchunkID1: ";
	std::cout.write(subchunkID1, 4) << std::endl;
	std::cout << "subchunkSize1: " << std::to_string(subchunkSize1) << std::endl;
	std::cout << "audioFormat: " << std::to_string(audioFormat) << std::endl;
	std::cout << "numChannels: " << std::to_string(numChannels) << std::endl;
	std::cout << "sampleRate: " << std::to_string(sampleRate) << std::endl;
	std::cout << "byteRate: " << std::to_string(byteRate) << std::endl;
	std::cout << "blockAlign: " << std::to_string(blockAlign) << std::endl;
	std::cout << "bitsPerSample: " << std::to_string(bitsPerSample) << std::endl;
	
	if (extSize > 0) {
		std::cout << "extSize: " << std::to_string(extSize) << std::endl;
		std::cout << "validBitsPerSample: " << std::to_string(validBitsPerSample) << std::endl;
		std::cout << "channelMask: " << std::to_string(channelMask) << std::endl;
		std::cout << "GUID: " << std::to_string(GUID) << std::endl;
		std::cout.write(GUIDjunk, 16) << std::endl;
	}
	
	if (fact[0] != 0) {
		std::cout << "fact: ";
		std::cout.write(fact, 4) << std::endl;
		std::cout << "factSize: " << std::to_string(factSize) << std::endl;
		std::cout << "numSamplesPerChannel: " << std::to_string(numSamplesPerChannel) << std::endl;
	}

	std::cout << "subchunkID2: ";
	std::cout.write(subchunkID2, 4) << std::endl;
	std::cout << "subchunkSize2: " << std::to_string(subchunkSize2) << std::endl;

	std::cout << "bytesPerSample: " << std::to_string(bytesPerSample) << std::endl;
	std::cout << "numSamples: " << std::to_string(numSamples) << std::endl;
}

void WAVFile::printData() {
	for (size_t i = 0; i < subchunkSize2; ++i) {
		std::cout << std::hex << (unsigned int)data[i] << " ";
		if ((i + 1) % 8 == 0) std::cout << std::endl;
	}
}

void WAVFile::printNumData() {
	size_t i = 0;
	if (audioFormat == 1) {
		if (bitsPerSample <= 8) {
			mapNumData<uint8_t>(0, numSamples, [i](uint8_t *n) mutable {
				std::cout << std::to_string(*n) << " ";
				if ((i + 1) % 8 == 0) std::cout << std::endl;
				++i;
			});
		} else if (bitsPerSample <= 16) {
			mapNumData<int16_t>(0, numSamples, [i](int16_t *n) mutable {
				std::cout << std::to_string(*n) << " ";
				if ((i + 1) % 8 == 0) std::cout << std::endl;
				++i;
			});
		} else if (bitsPerSample <= 32) {
			mapNumData<int32_t>(0, numSamples, [i](int32_t *n) mutable {
				std::cout << std::to_string(*n) << " ";
				if ((i + 1) % 8 == 0) std::cout << std::endl;
				++i;
			});
		} else if (bitsPerSample <= 64) {
			mapNumData<int64_t>(0, numSamples, [i](int64_t *n) mutable {
				std::cout << std::to_string(*n) << " ";
				if ((i + 1) % 8 == 0) std::cout << std::endl;
				++i;
			});
		} else {
			throw "bytesPerSample too big";
		}
	} else if (audioFormat == 3) {
		/*
		for (size_t i = 0; i < subchunkSize2 / (bitsPerSample / 8); ++i) {
			float *p = (float*)(&(data[0]) + i * (bitsPerSample / 8));
			std::cout << std::to_string(*p) << " ";
			if ((i + 1) % 8 == 0) std::cout << std::endl;
		}*/
	} else {
		std::cout << "Unsupported format" << std::endl;
		return;
	}
	
}

WAVFile::~WAVFile() {}
