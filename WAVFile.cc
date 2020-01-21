#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <map>

#include "WAVFile.h"

WAVFile::WAVFile(std::string path): path{path}, f{path, std::ifstream::binary} {

	f.read(chunkID, 4);
	f.read((char*)&chunkSize, 4);
	f.read(format, 4);
	
	if (strncmp(chunkID, "RIFF", 4) != 0 || strncmp(format, "WAVE", 4) != 0) throw "Not a WAV";

	while (f.peek() != EOF) readChunk();
	f.close();

	bytesPerSample = (bitsPerSample + 7) / 8;
	numSamples = subchunkSize2 / bytesPerSample;
	
	
	/*printHeader();
	printData();
	printNumData();*/
	normalizeData();
}

// Assumes the first 12 bytes were already read and are "RIFF", size of file - 8, and "WAVE"
void WAVFile::readChunk() {
	f.read((char*)&tempID, 4);
	f.read((char*)&tempSize, 4);
	
	//use map of four-byte arrays to function pointers of what to do
	auto iter = func_map.find(std::string(tempID, sizeof(tempID)));
	if (iter != func_map.end()) {
		std::cout << "Reading ";
		std::cout.write(tempID, 4) << " of size " << tempSize << std::endl;
		p_func p = iter->second;
		(this->*p)();
	} else {
		std::cout << "Skipped ";
		std::cout.write(tempID, 4) << " of size " << tempSize << std::endl;
		f.ignore(tempSize);
	}
}

// func_map definitions

void WAVFile::read_fmt() {
	strncpy(subchunkID1, tempID, 4);
	subchunkSize1 = tempSize;
	if (subchunkSize1 < 16) throw "fmt chunk size less than 16";
	f.read((char*)&audioFormat, 2);
	f.read((char*)&numChannels, 2);
	f.read((char*)&sampleRate, 4);
	f.read((char*)&byteRate, 4);
	f.read((char*)&blockAlign, 2);
	f.read((char*)&bitsPerSample, 2);
	if (subchunkSize1 > 16) {
		std::cout << "fmt chunk has an ext" << std::endl;	
		f.read((char*)&extSize, 2);
		if (extSize == 22) {
			f.read((char*)&validBitsPerSample, 2);
			f.read((char*)&channelMask, 4);
			f.read((char*)&GUID, 2);
			f.read(GUIDjunk, 14);
		} else {
			std::cout << "ext had different fields than expected" << std::endl;
			validBitsPerSample = bitsPerSample;
			channelMask = 0;
			GUID = 0;
			GUIDjunk[0] = 0;
			f.ignore(extSize);
		}
	} else {
		std::cout << "fmt chunk has no ext" << std::endl;
	}
}

void WAVFile::read_fact() {
	strncpy(fact, tempID, 4);
	factSize = tempSize;
	if (factSize < 4) throw "factSize < 4";
	f.read((char*)&numSamplesPerChannel, 4);
	f.ignore(factSize - 4);
}

void WAVFile::read_data() {
	strncpy(subchunkID2, tempID, 4);
	subchunkSize2 = tempSize;
	data.reserve(subchunkSize2);
	f.read((char*)(&data[0]), subchunkSize2);
}

void WAVFile::read_LIST() {
	//ignore list for now
	f.ignore(tempSize);
}

void WAVFile::printHeader(std::ostream &out) {
	out << "path: " << path << "\n";
	//print the header
	out << "chunkID: ";
	out.write(chunkID, 4) << "\n";
	out << "chunkSize: " << std::to_string(chunkSize) << "\n";
	out << "format: ";
	out.write(format, 4) << "\n";

	out << "subchunkID1: ";
	out.write(subchunkID1, 4) << "\n";
	out << "subchunkSize1: " << std::to_string(subchunkSize1) << "\n";
	out << "audioFormat: " << std::to_string(audioFormat) << "\n";
	out << "numChannels: " << std::to_string(numChannels) << "\n";
	out << "sampleRate: " << std::to_string(sampleRate) << "\n";
	out << "byteRate: " << std::to_string(byteRate) << "\n";
	out << "blockAlign: " << std::to_string(blockAlign) << "\n";
	out << "bitsPerSample: " << std::to_string(bitsPerSample) << "\n";
	
	out << "extSize: " << std::to_string(extSize) << "\n";
	out << "validBitsPerSample: " << std::to_string(validBitsPerSample) << "\n";
	out << "channelMask: " << std::to_string(channelMask) << "\n";
	out << "GUID: " << std::to_string(GUID) << "\n";
	out.write(GUIDjunk, 16) << "\n";
		
	out << "fact: ";
	out.write(fact, 4) << "\n";
	out << "factSize: " << std::to_string(factSize) << "\n";
	out << "numSamplesPerChannel: " << std::to_string(numSamplesPerChannel) << "\n";

	out << "subchunkID2: ";
	out.write(subchunkID2, 4) << "\n";
	out << "subchunkSize2: " << std::to_string(subchunkSize2) << "\n";

	out << "bytesPerSample: " << std::to_string(bytesPerSample) << "\n";
	out << "numSamples: " << std::to_string(numSamples) << std::endl;
}

void WAVFile::printData(std::ostream &out) {
	int align = bytesPerSample * numChannels;
	for (size_t i = 0; i < subchunkSize2; ++i) {
		out << std::hex << (unsigned int)data[i] << " ";
		if ((i + 1) % align == 0) out << "\n";
	}
	out.flush();
}

void WAVFile::printNormData(std::ostream &out) {
	for (size_t i = 0; i < normData.size(); ++i) {
		out << std::to_string(normData[i]) << " ";
		if ((i + 1) % numChannels == 0) out << "\n";
	}
	out.flush();
}

void WAVFile::printNumData(std::ostream &out) {
	out << "printing num data" << std::endl;
	int i = 0;
	mapData(0, numSamples, [i, this, &out](auto *n) mutable {
		out << std::to_string(*n) << " ";
		if ((i + 1) % numChannels == 0) out << "\n";
		++i;
	}); 
	out.flush();
}

void WAVFile::normalizeData() {
	mapData(0, numSamples, [this](auto *n) mutable {
		// need to first normalize the value, which is based on bitsPerSample
		if (audioFormat == 1) normData.push_back(normedSample(*n));
		else normData.push_back(*n);
		//std::cout << std::to_string(normData.back()) << std::endl;
	});
	//std::cout << "elements in normData: " << std::to_string(normData.size()) << std::endl;
}

double WAVFile::getSampleFromChannel(uint16_t channelNum, int i) {
	return normData[i * numChannels + channelNum];
}
/*
double WAVFile::getSampleAtTime(int t) {
	return 0;
}*/

double WAVFile::getFactor() {
	if (bytesPerSample == 1) return F_8;
	else if (bytesPerSample == 2) return F_16;
	else if (bytesPerSample == 3) return F_24;
	else if (bytesPerSample == 4) return F_32;
	else if (bytesPerSample == 8) return F_64;
	else throw "Invalid bytesPerSample";
}

WAVFile::~WAVFile() {}
