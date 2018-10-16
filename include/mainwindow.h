#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
public:
    QScreen* screen;

    MainWindow(QWidget *parent=nullptr);
    void center();
};

#endif // MAINWINDOW_H
