#include "../include/mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent), ui(new Ui::MainWindow())
{
    ui->setupUi(this);
    ui->viewer->set_frames_per_second(60);

    // SIGNALS & SLOTS powered by C++ Lambda
    // CAP FPS
    connect(ui->spin_fps, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int i){ ui->viewer->set_frames_per_second(size_t(i)); });

    // WIREFRAME DISPLAY
    connect(ui->checkBox, QOverload<int>::of(&QCheckBox::stateChanged),
            [=](int i){ ui->viewer->display_wireframe(i); });

    // FILL DISPLAY
    connect(ui->checkBox_2, QOverload<int>::of(&QCheckBox::stateChanged),
            [=](int i){ ui->viewer->display_fill(i); });

    // RESET VIEW
    connect(ui->reset_view, QOverload<bool>::of(&QPushButton::clicked),
            [=](){ ui->viewer->reset_view(); });

    // SCALE MESH
    connect(ui->spin_scale, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [=](float factor){ ui->viewer->set_scale_factor(factor); });

    // LOAD OBJ FILE
    connect(ui->actionLoad_OBJ, QOverload<bool>::of(&QAction::triggered), [=]()
    {
        QString file = QFileDialog::getOpenFileName(
            this, "Open mesh object", "..", "Mesh Files (*.obj)",
            nullptr, QFileDialog::DontUseNativeDialog
        );

        if( !file.isEmpty() ){
            ui->viewer->get_obj_from_filesystem(file.toStdString());
            ui->statusbar->showMessage(ui->viewer->status_message());
            ui->spin_scale->setValue(1.0);
        }
    });

    // Create a lambda function to update light position
    std::function<void()> update_light_pos = [=](){
        ui->viewer->set_light_position(
            ui->lcd_x->intValue(),
            ui->lcd_y->intValue(),
            ui->lcd_z->intValue());
    };

    // LIGHT POSITION
    connect(ui->light_x, QOverload<int>::of(&QSlider::valueChanged),
            [=](int value){ ui->lcd_x->display(value); update_light_pos(); });

    connect(ui->light_y, QOverload<int>::of(&QSlider::valueChanged),
            [=](int value){ ui->lcd_y->display(value); update_light_pos(); });

    connect(ui->light_z, QOverload<int>::of(&QSlider::valueChanged),
            [=](int value){ ui->lcd_z->display(value); update_light_pos(); });

    // QUIT APP
    connect(ui->actionExit, &QAction::triggered, this, &QMainWindow::close);

    this->setWindowTitle("PGAI");
    this->resize(1280, 720);
    this->center();

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
    // update FPS "overlay"
    ui->fps_counter->display(int(ui->viewer->get_computed_frames()));
    ui->viewer->reset_computed_frames();
}
