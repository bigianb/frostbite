#include "InfoPanel.h"

#include <QtWidgets>

InfoPanel::InfoPanel(QWidget* parent, GameData& gameData) : gameData(gameData)
{
    widget = new QTableWidget(1, 2, parent);
}

InfoPanel::~InfoPanel()
{
    delete widget;
    widget = nullptr;
}

QWidget* InfoPanel::getWidget()
{
    return widget;
}
