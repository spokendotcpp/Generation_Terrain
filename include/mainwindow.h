#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream> //debug mod
#include <QMainWindow>
#include <QStyle>
#include <QApplication>
#include <QKeyEvent>
#include <QScreen>

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget *parent=nullptr);
    void center();
    void keyPressEvent(QKeyEvent* event) override;
};

#endif // MAINWINDOW_H
