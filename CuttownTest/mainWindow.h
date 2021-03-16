

#pragma once

#include <QMainWindow>

#include "gameData.h"

QT_BEGIN_NAMESPACE
class QAction;
class QListWidget;
class QMenu;
class QTextEdit;
QT_END_NAMESPACE


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
    QListWidget* listWidget;

    QMenu* viewMenu;

    GameData& gameData;
};

