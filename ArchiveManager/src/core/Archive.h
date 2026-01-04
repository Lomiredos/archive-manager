#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <vector>
#include <cstdint>
#include <cstdio>
#include "FileEntry.h"


class Archive {

private:

	std::string m_path;
	std::vector<FileEntry> files;
	FILE* filePtr = nullptr;
	uint16_t fileCount = 0;


public:
	Archive() : m_path("") {}
	Archive(const std::string& archivePath) : m_path(archivePath) {}

	~Archive() {
		if (filePtr != nullptr)
			fclose(filePtr);
	}

	bool create(const std::string& path);
	bool open(const std::string& path);

	bool addFile(const std::string& filePath);

	bool save();

	const std::vector<FileEntry>& listFiles() const { return files; }

	bool extractFile(const std::string& fileName, const std::string& outputPath);

	bool extractAll(const std::string& outputDir);

private:

	bool writeHeader();

	bool readHeader();

	bool writeIndex();

	bool readIndex();




};


#endif