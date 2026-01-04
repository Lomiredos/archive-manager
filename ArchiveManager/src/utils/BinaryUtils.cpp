

#include "BinaryUtils.h"


namespace BinaryUtils {


	//Lecture/Ecriture
	bool write(FILE* f, uint32_t value) {

		if (fwrite(&value, sizeof(uint32_t), 1, f) == 0) {
			return false;
		}

		return true;
	}
	bool read(FILE* f, uint32_t& outValue) {
		if (fread(&outValue, sizeof(uint32_t), 1, f) == 0) {
			return false;
		}
		return true;
	}
	bool write(FILE* f, uint16_t value) {

		if (fwrite(&value, sizeof(uint16_t), 1, f) == 0) {
			return false;
		}

		return true;
	}
	bool read(FILE* f, uint16_t& outValue) {
		if (fread(&outValue, sizeof(uint16_t), 1, f) == 0) {
			return false;
		}
		return true;
	}
	bool write(FILE* f, uint8_t value) {

		if (fwrite(&value, sizeof(uint8_t), 1, f) == 0) {
			return false;
		}

		return true;
	}
	bool read(FILE* f, uint8_t& outValue) {
		if (fread(&outValue, sizeof(uint8_t), 1, f) == 0) {
			return false;
		}
		return true;
	}

	//ckeckSum

	uint32_t checksum(const uint8_t* data, size_t size) {

		uint32_t result = 0;
		for (size_t index = 0; index < size; index++) {
			result = result ^ data[index];
		}
		return result;
	}

	//Manipulation de bit

	void activeBit(uint8_t& flags, int bitPosition) {
		
		//d'apres ton exemple on compte la position 0 ? 
		int masque = 1 << bitPosition;
		flags |= masque;
	}

	bool isBit(uint8_t flags, int bitPosition) {

		int masque = 1 << bitPosition;
		return flags & masque;
	}

	void unactiveBit(uint8_t& flags, int bitPosition) {
		int masque = ~(1 << bitPosition);

		flags &= masque;
	}

}