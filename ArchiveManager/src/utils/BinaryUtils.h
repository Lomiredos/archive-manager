#ifndef BINARY_UTILS_H
#define BINARY_UTILS_H


#include <cstdint>
#include <cstdio>


namespace BinaryUtils {



	//Lecture/Ecritre binaire

	bool write(FILE* f, uint32_t value);

	bool read(FILE* f, uint32_t& outValue);

	bool write(FILE* f, uint16_t value);

	bool read(FILE* f, uint16_t& outValue);

	bool write(FILE* f, uint8_t value);

	bool read(FILE* f, uint8_t& outValue);

	//Cheksum

	uint32_t  checksum(const uint8_t* data, size_t size);

	//Manipulation de flags

	void activeBit(uint8_t& flags, int bitPosition);

	bool isBit(uint8_t flags, int bitPosition);

	void unactiveBit(uint8_t& flags, int bitPosition);
}

#endif