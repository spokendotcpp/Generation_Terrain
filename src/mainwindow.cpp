#include "../include/mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent), ui(new Ui::MainWindow())
{
    ui->setupUi(this);
    ui->viewer->set_frames_per_second(60);

    this->setWindowTitle("Field generation");
    this->resize(1280, 720);
    this->center();

    connect_signals_and_slots();

    // start a Callback timer of (1s, 1000ms)
    startTimer(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
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
        ui->viewer->handle_key_events(event);
}

void
MainWindow::timerEvent(QTimerEvent*)
{
    std::cerr << ui->viewer->get_computed_frames() << std::endl;
    ui->viewer->reset_computed_frames();
}

void
MainWindow::connect_signals_and_slots()
{
    // QUIT APP
    connect(ui->actionExit, &QAction::triggered, this, &QMainWindow::close);
}
