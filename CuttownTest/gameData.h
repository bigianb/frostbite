#pragma once

#include <QString>

#include "GameType.h"
#include "GameRegion.h"

class GobFile;
class World;

class GameData
{
public:
    GameData();
    ~GameData();

    void read(QString rootDir, QString worldName);

    QString getGobName() { return gobName; }
    QString getRootDir() { return rootDir; }

    World* getWorld() { return world;  }

private:
    GameRegion gameRegion;
    GameType gameType;

    /* The directory to the root of the disc, where the elf is located. */
    QString rootDir;

    /* Relative path from the root where the data files live. */
    QString dataRelPath;

    QString gobName;
    GobFile* worldGob;
    World* world;


private:
    void findGame(QString rootDir);
    void findGameFromElf(QString elfname);
};
