#pragma once

class QWidget;
class GameData;

class InfoPanel
{
public:
    InfoPanel(QWidget* parent, GameData& gameData);
    ~InfoPanel();

    QWidget* getWidget();

private:
    QWidget* widget;
    GameData& gameData;
};

