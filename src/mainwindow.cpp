#include "../include/mainwindow.h"

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent), mesh_viewer(new MeshViewerWidget())
{
    this->setWindowTitle("Projet 5 : Génération de terrain");
    this->setCentralWidget(mesh_viewer);
    this->setMinimumSize(600, 500);
    this->center();
}

MainWindow::~MainWindow()
{
    if( mesh_viewer != nullptr )
        delete mesh_viewer;
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

void
MainWindow::keyPressEvent(QKeyEvent* event)
{
    if( event->key() == Qt::Key_Escape )
        close();
    else
        mesh_viewer->handle_key_events(event);
}
