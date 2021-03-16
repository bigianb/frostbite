
#include <QtWidgets>
#include "mainWindow.h"

MainWindow::MainWindow(GameData& gameDataIn)
    : textEdit(new QTextEdit), gameData(gameDataIn)
{
    setCentralWidget(textEdit);

    createActions();
    createStatusBar();
    createDockWindows();

    setWindowTitle(tr("World Viewer"));

    setUnifiedTitleAndToolBarOnMac(true);
}



void MainWindow::about()
{
    QMessageBox::about(this, tr("About World Viewer"),
        tr("blah blah blah"));
}

void MainWindow::createActions()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar* fileToolBar = addToolBar(tr("File"));

    fileMenu->addSeparator();

    QAction* quitAct = fileMenu->addAction(tr("&Quit"), this, &QWidget::close);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));

    viewMenu = menuBar()->addMenu(tr("&View"));

    menuBar()->addSeparator();

    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction* aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));
}


void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createDockWindows()
{
    QDockWidget* dock = new QDockWidget(tr("Structure"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    listWidget = new QListWidget(dock);
    listWidget->addItems(QStringList()
        << "xxx"
        );
    dock->setWidget(listWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    viewMenu->addAction(dock->toggleViewAction());
}

