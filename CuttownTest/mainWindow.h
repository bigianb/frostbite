

#pragma once

#include <QMainWindow>

#include "gameData.h"

QT_BEGIN_NAMESPACE
class QAction;
class QListWidget;
class QMenu;
class QTextEdit;
QT_END_NAMESPACE

class InfoPanel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(GameData& gameData);

private slots:
    void about();

private:
    void createActions();
    void createStatusBar();
    void createDockWindows();

    QTextEdit* textEdit;
    InfoPanel* infoPanel;

    QMenu* viewMenu;

    GameData& gameData;
};

