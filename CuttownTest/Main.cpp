#pragma once
#include "SDL.h"
#include <QApplication>

#include "windows.h"
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

#include "mainWindow.h"

LmpRepository* lmpRepository;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	//Q_INIT_RESOURCE(dockwidgets);
	MainWindow mainWin;
	mainWin.show();
	return app.exec();
	/*
	string dataPath = "D:\\emu\\bgda\\BG\\DATA\\";
	if (argc == 2){
		dataPath = argv[1];
	}
	lmpRepository = new LmpRepositoryImpl(dataPath, GameType::DARK_ALLIANCE);

	GobFile cuttownGob = GobFile(dataPath + "CUTTOWN.GOB", GameType::DARK_ALLIANCE);
	World* world = WorldReader().readWorld(&cuttownGob, "cuttown");

	
	return 0;
	*/
}



