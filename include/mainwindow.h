#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream> //debug mod
#include <QMainWindow>

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget *parent=nullptr);
    void center();
    void keyPressEvent(QKeyEvent* event) override;
};

#endif // MAINWINDOW_H
