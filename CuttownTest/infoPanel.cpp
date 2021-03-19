#include "InfoPanel.h"
#include "gameData.h"

#include <QtWidgets>

InfoPanel::InfoPanel(QWidget* parent, GameData& gameData) : gameData(gameData)
{
    widget = new QTableWidget(1, 2, parent);
    widget->setHorizontalHeaderLabels(QStringList() << "Property" << "Value");
    widget->setCornerButtonEnabled(false);
    widget->verticalHeader()->hide();
    widget->setAlternatingRowColors(true);
    populateGamedata();
}

void InfoPanel::populateGamedata()
{
    widget->clearContents();
    widget->setRowCount(0);
    widget->setSortingEnabled(false);
    addRow("Root Dir", gameData.getRootDir());
    addRow("GOB Name", gameData.getGobName());

    widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    widget->resizeColumnsToContents();
    widget->resizeRowsToContents();
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
