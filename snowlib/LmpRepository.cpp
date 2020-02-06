#include "LmpRepository.h"
#include "Logger.h"

LmpFile* LmpRepositoryImpl::readLmpFile(string lmpName)
{
	LmpFile* lmpFile = nullptr;
	string fullpath = dataPath + lmpName;
	FILE* file = fopen(fullpath.c_str(), "r");
	if (file != nullptr){
		fseek (file , 0 , SEEK_END);
		long size = ftell (file);
		rewind (file);

		char* buffer = (char*) malloc (size);

		long bytesRead = fread (buffer,1,size,file);
		fclose(file);

		lmpFile = new LmpFile(gameType, (unsigned char *)buffer, 0, size);
	} else {
		Logger::getLogger()->logFormat("failed to read file: '%s'\n", fullpath.c_str());
	}
	return lmpFile;
}

LmpRepositoryImpl::~LmpRepositoryImpl()
{
	for (auto& x : lmpFileMap){
		delete x.second;
	}
	lmpFileMap.clear();
}

