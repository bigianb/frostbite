#pragma once

#include <QString>

class QWidget;
class QTableWidget;
class GameData;

class InfoPanel
{
public:
    InfoPanel(QWidget* parent, GameData& gameData);
    ~InfoPanel();

    QWidget* getWidget();

private:
    void populateGamedata();
    void addRow(const char* name, QString val);

private:
    QTableWidget* widget;
    GameData& gameData;
};

