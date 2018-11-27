#include "../include/mainwindow.h"

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent), ui(new Ui::MainWindow())
{
    ui->setupUi(this);
    ui->viewer->set_frames_per_second(size_t(ui->spin_fps->value()));

    // SIGNALS
    connect(ui->spin_fps, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int i){ ui->viewer->set_frames_per_second(size_t(i)); }
    );

    connect(ui->actionExit, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->actionLoad_OBJ, &QAction::triggered, ui->viewer, &MeshViewerWidget::get_obj_from_filesystem);

    this->setWindowTitle("Génération de Terrain");
    this->resize(1280, 720);
    this->center();
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
