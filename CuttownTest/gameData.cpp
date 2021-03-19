#include "gameData.h"

#include <QDir>
#include <QDebug>

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

GameData::GameData() : gameRegion(GameRegion::UNKNOWN), gameType(GameType::UNKNOWN)
{
	worldGob = nullptr;
	world = nullptr;
}

GameData::~GameData()
{
	delete worldGob; worldGob = nullptr;
	delete world; world = nullptr;
}

void GameData::read(QString rootDir, QString worldName) 
{
	this->rootDir = rootDir;
	gameType = GameType::UNKNOWN;
	delete worldGob; worldGob = nullptr;
	delete world; world = nullptr;

	if (rootDir.isEmpty()) {
		return;
	}

	findGame(rootDir);
	if (gameType != GameType::UNKNOWN) {
		QString lmpDir = rootDir + "/" + dataRelPath;
		//LmpRepository* lmpRepository = new LmpRepositoryImpl(lmpDir.toStdString(), gameType);
		gobName = worldName.toUpper() + ".GOB";
		QString gobFile = lmpDir + "/" + gobName;
		worldGob = new GobFile(gobFile.toStdString(), gameType);
		world = WorldReader().readWorld(worldGob, worldName.toLower().toStdString().c_str());
	}
}

void GameData::findGame(QString rootDir)
{
	QDir rootDirectory(rootDir);
	if (rootDirectory.exists()) {
		auto entries = rootDirectory.entryList(QStringList() << "SL*.*");
		if (entries.length() == 1) {
			auto slFile = entries.first();
			findGameFromElf(slFile);
		}
		else {
			qWarning() << "Cannot find SL*.* file in " << rootDir;
		}
	}
}

void GameData::findGameFromElf(QString elfname)
{
	qDebug() << "found elf " << elfname;
	if (elfname == "SLES_506.72") {
		gameType = GameType::DARK_ALLIANCE;
		gameRegion = GameRegion::PAL;
		dataRelPath = "BG/DATA";
	}
	else if (elfname == "SLES_530.39") {
		gameType = GameType::CHAMPIONS_RTA;
		gameRegion = GameRegion::PAL;
		dataRelPath = "BG/DATA";
	}
	else if (elfname == "SLES_544.23") {
		gameType = GameType::JL_HEROES;
		gameRegion = GameRegion::PAL;
		dataRelPath = "GAME/DATA";
	}
	else {
		qWarning() << "Unknown game: " << elfname;
	}
}
