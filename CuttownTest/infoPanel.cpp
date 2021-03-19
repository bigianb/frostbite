#include "InfoPanel.h"

#include <QtWidgets>

InfoPanel::InfoPanel(QWidget* parent, GameData& gameData) : gameData(gameData)
{
    widget = new QTableWidget(1, 2, parent);
    widget->setHorizontalHeaderLabels(QStringList() << "Property" << "Value");
    populateGamedata();
}

void InfoPanel::populateGamedata()
{
    widget->clearContents();
    widget->setRowCount(0);
    widget->setSortingEnabled(false);
    addRow("Name", QString("xxx"));
    addRow("Name2", QString("xxx2"));

    widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void InfoPanel::addRow(const char* name, QString val)
{
    int row = widget->rowCount();
    widget->insertRow(row);
    widget->setItem(row, 0, new QTableWidgetItem(name));
    widget->setItem(row, 1, new QTableWidgetItem(val));
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
