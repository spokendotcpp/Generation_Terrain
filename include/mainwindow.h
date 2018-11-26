#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream> //debug mod

#include <QMainWindow>
#include <QStyle>
#include <QApplication>
#include <QKeyEvent>
#include <QScreen>
#include <QSizePolicy>

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMenuBar>
#include <QPushButton>

#include "meshviewerwidget.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
private:
    // MeshViewerWidget* mesh_viewer;
    Ui::MainWindow* ui;

public:
    MainWindow(QWidget *parent=nullptr);
    ~MainWindow() override;
    void center();
    void keyPressEvent(QKeyEvent*) override;
};

#endif // MAINWINDOW_H
