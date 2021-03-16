#pragma once

#include <QApplication>
#include <QCommandLineParser>

#include "mainWindow.h"
#include "gameData.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	QCoreApplication::setApplicationName("World Viewer");
	QCoreApplication::setApplicationVersion("1.0");

	QCommandLineParser parser;
	parser.setApplicationDescription("World Viewer");
	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption rootDirectoryOption(QStringList() << "r" << "root-directory",
		"The root directory of the game.",
		 "directory");
	parser.addOption(rootDirectoryOption);

	QCommandLineOption worldNameOption(QStringList() << "w" << "world",
		"The name of the world to open.",
		"worldname");
	parser.addOption(worldNameOption);

	parser.process(app);

	QString rootDir = parser.value(rootDirectoryOption);
	QString worldName = parser.value(worldNameOption);

	//Q_INIT_RESOURCE(dockwidgets);
	GameData gameData;
	gameData.read(rootDir, worldName);
	MainWindow mainWin(gameData);
	mainWin.show();
	return app.exec();
	
}



