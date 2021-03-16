#pragma once

#include <QString>

class GameData
{
public:
    void read(QString rootDir, QString worldName);

private:
    void findGame(QString rootDir);
};
