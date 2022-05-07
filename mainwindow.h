#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class Depot;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Depot* depot, QWidget *parent = nullptr);
    ~MainWindow();
};

#endif // MAINWINDOW_H
