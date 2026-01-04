#include "Archive.h"
#include "../utils/BinaryUtils.h"
#include <cstdio>
#include <direct.h>

bool Archive::writeHeader() {

	if (filePtr == nullptr)
		return false;
	const char signature[4] = { 'A','R','C','1' };
	if (fwrite(signature, 1, 4, filePtr) != 4)
		return false;
	uint16_t version = 1;
	if (!BinaryUtils::write(filePtr, version))
		return false;
	if (files.size() > 65535) //max sur 16 octet la limite est atteinte, mettre moins de fichier
		return false;
	uint16_t nbFiles = static_cast<uint16_t>(files.size());
	if (!BinaryUtils::write(filePtr, nbFiles))
		return false;
	uint32_t flags = 0;
	if (!BinaryUtils::write(filePtr, flags))
		return false;
	uint32_t reserved = 0;
	if (!BinaryUtils::write(filePtr, reserved))
		return false;

	return true;
}

bool Archive::readHeader() {
	if (filePtr == nullptr)
		return false;
	char signature[4];
	if (fread(signature, 1, 4, filePtr) != 4)
		return false;
	if (signature[0] != 'A' || signature[1] != 'R' || signature[2] != 'C' || signature[3] != '1')
		return false;
	uint16_t version;
	if (!BinaryUtils::read(filePtr, version))
		return false;
	if (version != 1)
		return false;
	uint16_t nbFiles;
	if (!BinaryUtils::read(filePtr, nbFiles))
		return false;
	fileCount = nbFiles;
	files.clear();
	files.reserve(fileCount);
	uint32_t flags;
	if (!BinaryUtils::read(filePtr, flags))
		return false;
	uint32_t reserved;
	if (!BinaryUtils::read(filePtr, reserved))
		return false;

	return true;
}

bool Archive::create(const std::string& path) {
	m_path = path;
	fopen_s(&filePtr, m_path.c_str(), "wb");
	if (filePtr == nullptr)
		return false;
	if (!writeHeader()) {
		fclose(filePtr);
		filePtr = nullptr;
		return false;
	}
	fclose(filePtr);
	filePtr = nullptr;
	return true;
}

bool Archive::addFile(const std::string& filePath) {


	FILE* sourceFile;
	fopen_s(&sourceFile, filePath.c_str(), "rb");

	if (sourceFile == nullptr)
		return false;
	fseek(sourceFile, 0, SEEK_END);
	long fileSizeLong = ftell(sourceFile);
	if (fileSizeLong < 0 || fileSizeLong > UINT32_MAX)
		return false;
	uint32_t fileSize = static_cast<uint32_t>(fileSizeLong);
	fseek(sourceFile, 0, SEEK_SET);

	uint8_t* buffer = new uint8_t[fileSize];
	if (fread(buffer, 1, fileSize, sourceFile) != fileSize)
	{
		delete[] buffer;
		fclose(sourceFile);
		return false;
	}
	uint32_t checksum = BinaryUtils::checksum(buffer, fileSize);

	size_t lastSlash = filePath.find_last_of("/\\");
	std::string fileName;
	if (lastSlash != std::string::npos)
		fileName = filePath.substr(lastSlash + 1);
	else
		fileName = filePath;

	FileEntry newFile = FileEntry(fileName);
	newFile.setFilePath(filePath);
	newFile.setFileSize(fileSize);
	newFile.setFileCompressedSize(fileSize);
	newFile.setFileChecksum(checksum);
	files.push_back(newFile);
	delete[] buffer;
	fclose(sourceFile);
	return true;
}

bool Archive::writeIndex() {

	if (filePtr == nullptr)
		return false;

	for (const FileEntry& file : files) {
		char nameBuffer[256] = { 0 };
		strncpy_s(nameBuffer, file.getFileName().c_str(), 255);
		if (fwrite(nameBuffer, 1, 256, filePtr) != 256)
			return false;

		if (!BinaryUtils::write(filePtr, file.getFileSize()))
			return false;
		if (!BinaryUtils::write(filePtr, file.getFileCompressedSize()))
			return false;
		if (!BinaryUtils::write(filePtr, file.getFileOffSet()))
			return false;
		if (!BinaryUtils::write(filePtr, file.getFileFlags()))
			return false;
		if (!BinaryUtils::write(filePtr, file.getFileChecksum()))
			return false;
	}
	return true;
}

bool Archive::readIndex() {
	if (filePtr == nullptr)
		return false;

	for (int index = 0; index < fileCount; index++) {
		char nameBuffer[256];
		if (fread(nameBuffer, 1, 256, filePtr) != 256)
			return false;
		FileEntry newFile = FileEntry(nameBuffer);
		uint32_t size;
		if (!BinaryUtils::read(filePtr, size))
			return false;
		newFile.setFileSize(size);

		uint32_t compressedSize;
		if (!BinaryUtils::read(filePtr, compressedSize))
			return false;
		newFile.setFileCompressedSize(compressedSize);

		uint32_t offSet;
		if (!BinaryUtils::read(filePtr, offSet))
			return false;
		newFile.setFileOffSet(offSet);

		uint8_t flags;
		if (!BinaryUtils::read(filePtr, flags))
			return false;
		newFile.setFileFlags(flags);

		uint32_t checksum;
		if (!BinaryUtils::read(filePtr, checksum))
			return false;
		newFile.setFileChecksum(checksum);
		files.push_back(newFile);
	}
	return true;
}

bool Archive::save() {
	fileCount = static_cast<uint16_t>(files.size());

	
	FILE* oldArchive = nullptr;
	fopen_s(&oldArchive, m_path.c_str(), "rb");

	std::vector<uint32_t> oldOffsets;
	for (const FileEntry& file : files) {
		oldOffsets.push_back(file.getFileOffSet());
	}

	std::string tempPath = m_path + ".tmp";
	fopen_s(&filePtr, tempPath.c_str(), "wb");
	if (filePtr == nullptr) {
		if (oldArchive != nullptr) fclose(oldArchive);
		return false;
	}

	if (!writeHeader()) {
		fclose(filePtr);
		if (oldArchive != nullptr) fclose(oldArchive);
		filePtr = nullptr;
		return false;
	}

	uint32_t currentOffset = 16 + (fileCount * 273);
	for (FileEntry& file : files) {
		file.setFileOffSet(currentOffset);
		currentOffset += file.getFileCompressedSize();
	}

	if (!writeIndex()) {
		fclose(filePtr);
		if (oldArchive != nullptr) fclose(oldArchive);
		filePtr = nullptr;
		return false;
	}

	for (size_t i = 0; i < files.size(); i++) {
		FileEntry& file = files[i];
		uint8_t* buffer = new uint8_t[file.getFileCompressedSize()];

		if (!file.getFilePath().empty()) {
			FILE* sourceFile;
			fopen_s(&sourceFile, file.getFilePath().c_str(), "rb");
			if (sourceFile == nullptr) {
				delete[] buffer;
				fclose(filePtr);
				if (oldArchive != nullptr) fclose(oldArchive);
				filePtr = nullptr;
				return false;
			}

			if (fread(buffer, 1, file.getFileCompressedSize(), sourceFile) != file.getFileCompressedSize()) {
				delete[] buffer;
				fclose(sourceFile);
				fclose(filePtr);
				if (oldArchive != nullptr) fclose(oldArchive);
				filePtr = nullptr;
				return false;
			}

			fclose(sourceFile);
		}
		else {
			if (oldArchive == nullptr) {
				delete[] buffer;
				fclose(filePtr);
				return false;
			}

			
			fseek(oldArchive, oldOffsets[i], SEEK_SET);
			if (fread(buffer, 1, file.getFileCompressedSize(), oldArchive) != file.getFileCompressedSize()) {
				delete[] buffer;
				fclose(filePtr);
				fclose(oldArchive);
				filePtr = nullptr;
				return false;
			}
		}

		
		if (fwrite(buffer, 1, file.getFileCompressedSize(), filePtr) != file.getFileCompressedSize()) {
			delete[] buffer;
			fclose(filePtr);
			if (oldArchive != nullptr) fclose(oldArchive);
			filePtr = nullptr;
			return false;
		}

		delete[] buffer;
	}

	
	fclose(filePtr);
	filePtr = nullptr;
	if (oldArchive != nullptr) {
		fclose(oldArchive);
	}

	
	remove(m_path.c_str());
	rename(tempPath.c_str(), m_path.c_str());

	return true;
}



bool Archive::open(const std::string& path) {

	m_path = path;
	fopen_s(&filePtr, m_path.c_str(), "rb");
	if (filePtr == nullptr)
		return false;
	if (!readHeader()) {
		fclose(filePtr);
		filePtr = nullptr;
		return false;
	}

	if (!readIndex()) {
		fclose(filePtr);
		filePtr = nullptr;
		return false;
	}





	fclose(filePtr);
	filePtr = nullptr;


	return true;
}

bool Archive::extractFile(const std::string& fileName, const std::string& outputPath) {


	FILE* archiveFile = nullptr;
	fopen_s(&archiveFile, m_path.c_str(), "rb");
	if (archiveFile == nullptr)
		return false;

	FileEntry* foundFile = nullptr;
	for (FileEntry& file : files) {
		if (file.getFileName() == fileName) {
			foundFile = &file;
			break;
		}
	}

	if (foundFile == nullptr) {
		fclose(archiveFile);
		return false;
	}

	fseek(archiveFile, foundFile->getFileOffSet(), SEEK_SET);

	uint8_t* buffer = new uint8_t[foundFile->getFileCompressedSize()];

	if (fread(buffer, 1, foundFile->getFileCompressedSize(), archiveFile) != foundFile->getFileCompressedSize()) {
		delete[] buffer;
		fclose(archiveFile);
		return false;
	}

	uint32_t checksum = BinaryUtils::checksum(buffer, foundFile->getFileCompressedSize());
	if (checksum != foundFile->getFileChecksum())
	{
		delete[] buffer;
		fclose(archiveFile);
		return false;
	}
	FILE* outputFile;
	fopen_s(&outputFile, outputPath.c_str(), "wb");
	if (outputFile == nullptr) {
		delete[] buffer;
		fclose(archiveFile);
		return false;
	}
	if (fwrite(buffer, 1, foundFile->getFileCompressedSize(), outputFile) != foundFile->getFileCompressedSize())
	{
		delete[] buffer;
		fclose(outputFile);
		fclose(archiveFile);
		return false;
	}
	delete[] buffer;
	fclose(outputFile);
	fclose(archiveFile);
	return true;
}

bool Archive::extractAll(const std::string& outputDir) {

	if (_mkdir(outputDir.c_str()) != 0) {
		return false;
	}
	for (const FileEntry& file : files) {
		std::string outputPath = outputDir + "/" + file.getFileName();
		if (!extractFile(file.getFileName(), outputPath))
		{
			return false;
		}
	}
	return true;
}