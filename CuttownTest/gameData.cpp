#include "gameData.h"

#include <QDir>

#include "LmpRepository.h"
#include "GobFile.h"
#include "VifDecoder.h"
#include "TexDecoder.h"
#include "AnimDecoder.h"
#include "Model.h"
#include "Texture.h"
#include "AnimData.h"
#include "WorldReader.h"
#include "World.h"

void GameData::read(QString rootDir, QString worldName)
{
	findGame(rootDir);
	/*
	LmpRepository* lmpRepository = new LmpRepositoryImpl(dataPath, GameType::DARK_ALLIANCE);

	GobFile cuttownGob = GobFile(dataPath + "CUTTOWN.GOB", GameType::DARK_ALLIANCE);
	World* world = WorldReader().readWorld(&cuttownGob, "cuttown");
	*/
}

void GameData::findGame(QString rootDir)
{
	QDir rootDirectory(rootDir);
	if (rootDirectory.exists()) {
		auto entries = rootDirectory.entryList(QStringList() << "SL*.*");

	}
}
