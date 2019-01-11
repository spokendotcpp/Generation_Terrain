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
    ui->fps->display(int(ui->viewer->get_computed_frames()));
    ui->viewer->reset_computed_frames();
}

void
MainWindow::connect_signals_and_slots()
{
    connect(ui->action_quit, &QAction::triggered, this, &QMainWindow::close);  // QUIT APP

    // Render Flat or Smooth
    connect(ui->action_flat, &QAction::triggered, ui->viewer, [this]{ ui->viewer->smooth_render(false); });
    connect(ui->action_smooth, &QAction::triggered, ui->viewer, [this]{ ui->viewer->smooth_render(true); });

    // Generate Push Button
    connect(ui->button_generate, &QPushButton::released, ui->viewer, [this]{
        ui->viewer->generate_new_field(
            float(ui->spinbox_width->value()),
            float(ui->spinbox_height->value()),
            float(ui->spinbox_length->value()),
            size_t(ui->spinbox_size->value())
        );

        const Field* field = ui->viewer->get_field();
        if( field != nullptr ){

            size_t size = (1 << ui->spinbox_size->value())+1;

            ui->statusBar->showMessage(QString(
                "Number of faces: "
                + QString::number(field->nb_faces())
                + " | Number of vertices: "
                + QString::number(field->nb_vertices())
                + " (" + QString::number(size) + "x" + QString::number(size) + ")"
            ));
        }
    });
}
