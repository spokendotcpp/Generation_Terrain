#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream> //debug mod
#include <QMainWindow>
#include <QStyle>
#include <QApplication>
#include <QKeyEvent>
#include <QScreen>

#include "../include/meshviewerwidget.h"

class MainWindow : public QMainWindow
{
public:
    MeshViewerWidget* mesh_viewer;

public:
    MainWindow(QWidget *parent=nullptr);
    ~MainWindow() override;
    void center();
    void keyPressEvent(QKeyEvent*) override;
};

#endif // MAINWINDOW_H
