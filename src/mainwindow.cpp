#include "../include/mainwindow.h"

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent), mesh_viewer(new MeshViewerWidget(this))
{
    this->setWindowTitle("Génération de Terrain");
    this->setCentralWidget(mesh_viewer);
    this->setMinimumSize(600, 500);
    this->resize(1280, 720);
    this->center();

    mesh_viewer->set_frames_per_second(9999);
}

MainWindow::~MainWindow()
{
    if( mesh_viewer != nullptr ){
        delete mesh_viewer;
        mesh_viewer = nullptr;
    }
}

void
MainWindow::center()
{
    QSize ssize = QApplication::primaryScreen()->size();
    int w = size().width();
    int h = size().height();

    this->setGeometry(
        (ssize.width() - w)/2,
        (ssize.height() - h)/2,
        w, h
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
