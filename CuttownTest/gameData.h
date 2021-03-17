#pragma once

#include <QString>

#include "GameType.h"
#include "GameRegion.h"

class GameData
{
public:
    GameData() : gameRegion(GameRegion::UNKNOWN), gameType(GameType::UNKNOWN) {}

    void read(QString rootDir, QString worldName);

private:
    GameRegion gameRegion;
    GameType gameType;

    /* The directory to the root of the disc, where the elf is located. */
    QString rootDir;

    /* Relative path from the root where the data files live. */
    QString dataRelPath;

private:
    void findGame(QString rootDir);
    void findGameFromElf(QString elfname);
};
