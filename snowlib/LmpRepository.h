#pragma once

#include <unordered_map>
#include "LmpFile.h"

using namespace std;

class LmpRepository
{
public:
	virtual LmpFile* getLmp(string lmpName) = 0;
};

/// Stores Lmp Files
class LmpRepositoryImpl : public LmpRepository
{
	unordered_map<string, LmpFile*> lmpFileMap;
	string dataPath;
	GameType gameType;
	LmpFile* readLmpFile(string lmpName);

public:
	LmpRepositoryImpl(string dataPath, GameType gameType)
	{
		this->dataPath = dataPath;
		this->gameType = gameType;
	}

	~LmpRepositoryImpl();

	LmpFile* getLmp(string lmpName)
	{
		if (lmpFileMap.count(lmpName) == 0){
			lmpFileMap[lmpName] = readLmpFile(lmpName);
		}
		return lmpFileMap[lmpName];
	}
};
