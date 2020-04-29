#ifndef READINT_H
#define READINT_H

#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
//Takes address of an integer that has had raw bytes written into it,
//and adds 1s to the beginning of it if it needs it (to keep signdness)
//for when the number of raw bytes is different from sizeof(T)
template <typename T> void castInt(T *dest, int rawBytes) {
	int size = sizeof(T);
	if (size == 1) return;
	int offset = size - rawBytes;//in bytes, diff between word and sample
	if (offset < 0) throw "Int not big enough";
	if (1 == (*dest >> (rawBytes * 8 - 1))) {
		//set all bits to the left of offset to be 1
		T ones = 1;
		for (size_t i = 0; i < offset * 8 - 1; ++i) {
			ones <<= 1;
			ones |= 1;
		}
		ones <<= rawBytes * 8;
		*dest |= ones;
	}
	//*dest <<= (offset * 8);
	//*dest >>= (offset * 8);
}

//Reads in rawBytes to an integer type one byte at a time
template <typename T> void bytesToInt(T *dest, unsigned char* rawBytes, int numBytes) {
	int size = sizeof(T);
	if (numBytes <= 0) throw "Invalid numBytes";
	if (numBytes > size) throw "Int not big enough in bytesToInt";
	*dest = rawBytes[numBytes - 1];
	for (int i = numBytes - 2; i >= 0; --i) {
		*dest = (*dest << 8) | rawBytes[i];
	}
}

template <typename T> void fileToInt(std::ifstream &f, T *dest, int numBytes) {
	unsigned char* rawBytes = new unsigned char[numBytes];
	f.read((char*)rawBytes, numBytes);
	bytesToInt(dest, rawBytes, numBytes);
	delete [] rawBytes;
}

#endif
