#include "mainwindow.h"
#include "plot.h"
#include "QHBoxLayout"
#include "QDockWidget"
#include "depot.h"

MainWindow::MainWindow(Depot *depot, QWidget *parent)
    : QMainWindow(parent)

{
    QDockWidget *dockWidget1 = new QDockWidget(tr("Dock Widget"), this);
    dockWidget1->setAllowedAreas(Qt::LeftDockWidgetArea |
                                Qt::RightDockWidgetArea);
    dockWidget1->setWidget(new Plot(depot));
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget1);

    //QDockWidget *dockWidget2 = new QDockWidget(tr("Dock Widget"), this);
    //dockWidget2->setAllowedAreas(Qt::LeftDockWidgetArea |
    //                            Qt::RightDockWidgetArea);
    //dockWidget2->setWidget(new Plot());
    //addDockWidget(Qt::RightDockWidgetArea, dockWidget2);

}

MainWindow::~MainWindow()
{

}
