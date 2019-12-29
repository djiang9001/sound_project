#ifndef READINT_H
#define READINT_H

#include <cstring>

//Takes address of an integer that has had raw bytes written into it,
//and adds 1s to the beginning of it if it needs it (to keep signdness)
//for when the number of raw bytes is different from sizeof(T)
template <typename T> void castInt(T *dest, int rawBytes) {
	int size = sizeof(T);
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

#endif
