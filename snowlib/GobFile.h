#pragma once

#include "LmpRepository.h"

/// A Gob file is a collection of Lmp files.
class GobFile : public LmpRepository
{
	string gobPath;
	GameType gameType;
	
	unsigned char* gobData;
	int gobDataLength;

	unordered_map<string, LmpFile*> lmpFileMap;

	void readGobFile();
	void buildLmpFileMap();

public:
	GobFile(string gobPath, GameType gameType)
	{
		this->gobPath = gobPath;
		this->gameType = gameType;
		gobData = nullptr;
		gobDataLength = 0;
	}

	~GobFile()
	{
		for (auto& x : lmpFileMap){
			delete x.second;
		}
		lmpFileMap.clear();
		delete[] gobData;
	}

	LmpFile* getLmp(string lmpName);
};
