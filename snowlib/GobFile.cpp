#include "GobFile.h"
#include "Logger.h"
#include "DataUtil.h"
#include <fstream>

void GobFile::buildLmpFileMap()
{
	lmpFileMap.clear();
	int offset = 0;
	while (gobData[offset] != 0){
		std::string lmpName((char*)gobData + offset);
		int lmpDataOffset = DataUtil::getLEInt(gobData, offset + 0x20);
		int lmpDataLength = DataUtil::getLEInt(gobData, offset + 0x24);
		LmpFile* lmpFile = new LmpFile(gameType, gobData, lmpDataOffset, lmpDataLength);
		lmpFileMap[lmpName] = lmpFile;
		offset += 0x28;
	}
}

void GobFile::readGobFile()
{
	FILE* file = fopen(gobPath.c_str(), "r");
	if (file != nullptr){
		fseek(file, 0, SEEK_END);
		long size = ftell(file);
		rewind(file);

		gobDataLength = (int)size;
		delete[] gobData;
		gobData = new unsigned char[gobDataLength];

		long bytesRead = fread(gobData, 1, size, file);
		fclose(file);
	}
	else {
		Logger::getLogger()->logFormat("failed to read file: '%s'\n", gobPath.c_str());
	}
}


LmpFile* GobFile::getLmp(string lmpName)
{
	if (gobDataLength == 0){
		readGobFile();
		buildLmpFileMap();
	}
	return lmpFileMap[lmpName];
}

