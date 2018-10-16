#include "../include/mainwindow.h"
#include "../include/meshviewerwidget.h"

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent)
{
    this->setWindowTitle("Projet 5 : Génération de terrain");
    this->setCentralWidget(new MeshViewerWidget());
    this->setMinimumSize(600, 500);
    this->center();
}

void
MainWindow::center()
{
    QMainWindow::setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            QApplication::primaryScreen()->geometry()
        )
    );
}
