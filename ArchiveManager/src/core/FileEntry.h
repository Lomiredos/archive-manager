
#ifndef FILE_ENTRY_H
#define FILE_ENTRY_H

#include <cstdint>
#include <string>
#include "../utils/BinaryUtils.h"

class FileEntry {
private:

	std::string name;
	uint32_t size;
	uint32_t compressedSize;
	uint32_t offset;
	uint8_t flags;
	uint32_t checksum;


	std::string sourcePath;

public:

	FileEntry() : name(""), size(0), compressedSize(0), offset(0), flags(0), checksum(0) {}
	FileEntry(std::string fileName) : name(fileName), size(0), compressedSize(0), offset(0), flags(0), checksum(0) {}

	//Getter

	const std::string& getFileName() const { return name; }
	const uint32_t getFileSize() const { return size; }
	const uint32_t getFileCompressedSize() const { return compressedSize; }
	const uint32_t getFileOffSet() const { return offset; }
	const uint8_t getFileFlags() const { return flags; }
	const uint32_t getFileChecksum() const { return checksum; }
	const std::string& getFilePath() const { return sourcePath; }

	//Setter

	void setFileSize(uint32_t value) { size = value; }
	void setFileCompressedSize(uint32_t value) { compressedSize = value; }
	void setFileOffSet(int value) { offset = value; }
	void setFileChecksum(uint32_t value) { checksum = value; }
	void setFileFlags(uint8_t value) { flags = value; }
	void setFilePath(std::string path) { sourcePath = path; }

	bool isCompressed() const {
		return BinaryUtils::isBit(flags, 0);
	}

	void setCompressed(bool compressed) {
		if (compressed)
			BinaryUtils::activeBit(flags, 0);
		else
			BinaryUtils::unactiveBit(flags, 0);
	}

};

#endif