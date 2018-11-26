#include <iostream>
#include <thread>
#include <future>

#include "../include/meshviewerwidget.h"

MeshViewerWidget::MeshViewerWidget(QWidget* parent)
    :QOpenGLWidget(parent)
{
    setMouseTracking(true);

    mouse_pressed = false;
    wheel_pressed = false;

    frames = 0;
    set_frames_per_second(60);
    lap = Clock::now();

    arcball = nullptr;
    program = nullptr;
    bunny = nullptr;
    light = nullptr;
    axis = nullptr;
}

/*
 * Destructor:
 *
 * Destroy every Buffers (Vertex Buffer Object: ARRAY_BUFFER, INDEX_BUFFER AND Vertex Array Object).
 * Free memory.
*/
MeshViewerWidget::~MeshViewerWidget()
{
    if( arcball == nullptr ){
        delete arcball;
        arcball = nullptr;
    }

    if( program != nullptr ){
        program->removeAllShaders();
        delete program;
        program = nullptr;
    }

    if( light != nullptr ){
        delete light;
        light = nullptr;
    }

    if( axis != nullptr ){
        delete axis;
        axis = nullptr;
    }

    if( bunny != nullptr ){
        delete bunny;
        bunny = nullptr;
    }
}

/* Update the model matrix
 * -> no specific actions yet.
*/
void
MeshViewerWidget::update_model()
{
    model.setToIdentity();
}

/* Update the view matrix
 * i.e.: can be called after every user inputs (mouse, keyboard ...)
*/
void
MeshViewerWidget::update_view()
{
    view.setToIdentity();
    view.translate(position);
    view.rotate(angle.x(), 1.0f, 0.0f, 0.0f);
    view.rotate(angle.y(), 0.0f, 1.0f, 0.0f);
}

/* Update the projection matrix
 * -> used when changing window size.
*/
void
MeshViewerWidget::update_projection()
{
    projection.setToIdentity();
    projection.perspective(
        fov,
        window_ratio,
        zNear, zFar
    );
}

/* Load default values for the model matrix + update */
void
MeshViewerWidget::default_model()
{
    /* ?? */
    update_model();
}

/* Load default values for the view matrix + update */
void
MeshViewerWidget::default_view()
{
    angle = QVector3D(25.0f, 25.0f, 0.0f);
    position = QVector3D(0.0f, 0.0f, -5.0f);
    update_view();
}

/* Load default values for the projection matrix + update */
void
MeshViewerWidget::default_projection()
{
    fov = 45.0f;
    zNear = 0.1f;
    zFar = 1000.0f;
    update_projection();
}

/* Load every defaults */
void
MeshViewerWidget::default_ModelViewPosition()
{
    default_model();
    default_view();
    default_projection();
}

/* Setup our OpenGL context
 * Vertex Array Buffer,
 * Vertex Buffer Object,
 * Elements Buffer Object,
 * Shaders programs,
 * [ ... ]
*/
void
MeshViewerWidget::initializeGL()
{
    // Check if OpenGL context was successfully initialized
    if( !isValid() ){
        std::cerr << "Failed to init OpenGL context" << std::endl;
        return;
    }

    arcball = new ArcBall(size().width(), size().height());

    // Create Object(s) :
    axis = new Axis();
    axis->scale(50.0f, 50.0f, 50.0f);

    bunny = new MeshObject("../mesh_files/bunnyLowPoly.obj");
    bunny->scale(5.0f, 5.0f, 5.0f);

    program = new QOpenGLShaderProgram();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, "../shaders/simple.vert.glsl");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, "../shaders/simple.frag.glsl");
    program->link();
    program->bind();
    {
        light = new Light();
        light->set_position(0.0f, 50.0f, 100.0f, program->uniformLocation("light_position"))
             ->set_color(0.6f, 0.6f, 0.6f, program->uniformLocation("light_color"))
             ->set_ambient(0.4f, program->uniformLocation("light_ambient"))
             ->enable(program->uniformLocation("light_on"));

        axis->build(program);
        axis->update_buffers(program);

        bunny->build(program);
        bunny->use_unique_color(1.0f, 1.0f, 0.0f);
        bunny->update_buffers(program);
    }
    program->release();

    glClearColor(1.0f, 191.0f/255.0f, 179.0f/255.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    default_ModelViewPosition();

    // IDLE Function
    startTimer(0);
    startTimer(1000);
}

/* When window (this widget) is resized */
void
MeshViewerWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    window_ratio = width/float(height);

    arcball->update_window_size(width, height);

    update_projection();
    update();
}

/* RENDER TIME */
void
MeshViewerWidget::paintGL()
{
    long mcs = microseconds_diff(Clock::now(), lap);

    if( mcs < frequency ){
        std::this_thread::sleep_for(
            std::chrono::microseconds(frequency - mcs)
        );
    }

    update_lap();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program->bind();
    {
        // in case user has modified light pos
        light->to_gpu(program);

        program->setUniformValue("projection", projection);
        program->setUniformValue("view", view);
        program->setUniformValue("view_inverse", view.transposed().inverted());

        program->setUniformValue("model", bunny->model_matrix());
        program->setUniformValue("model_inverse", bunny->model_matrix().transposed().inverted());
        bunny->show(GL_TRIANGLES);

        light->off(program);
        program->setUniformValue("model", axis->model_matrix());
        axis->show(GL_LINES);
        light->on(program);
    }
    program->release();
}

/* When mouse is moving inside the widget */
void
MeshViewerWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pos = event->pos();
    arcball->update_window_coord(pos.x(), pos.y());

    if( mouse_pressed ){
        //angle.setX(angle.x() + pos.y() - mouse.y());
        //angle.setY(angle.y() + pos.x() - mouse.x());
        //update_view();
        // std::cerr << mouse.x() << " - " << mouse.y() << std::endl;

        view *= arcball->get_rotation_matrix(view);

        update();
    }
    else
    if( wheel_pressed ){
        float step = 0.5f;

        position.setX(position.x() + ((pos.x() - mouse.x())*step));
        position.setY(position.y() - ((pos.y() - mouse.y())*step));

        update_view();
        update();
    }

    mouse = pos;
}

void
MeshViewerWidget::mousePressEvent(QMouseEvent* event)
{
    if( event->button() == Qt::MouseButton::LeftButton )
        mouse_pressed = true;
    else
    if( event->button() == Qt::MouseButton::MiddleButton )
        wheel_pressed = true;
}

void
MeshViewerWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if( event->button() == Qt::MouseButton::LeftButton )
        mouse_pressed = false;
    else
    if( event->button() == Qt::MouseButton::MiddleButton )
        wheel_pressed = false;
}

// IDLE Function: when no user inputs repaint
void
MeshViewerWidget::timerEvent(QTimerEvent* event)
{
    int id = event->timerId();

    if( id == 1 )
        update();
    else {
        std::cerr << "fps: " << frames << std::endl;
        frames = 0;
    }
}

/*
 * Make sure that position.z()
 * stay between ]zNear+step & zFar-step[
*/
void
MeshViewerWidget::wheelEvent(QWheelEvent* event)
{
    float step = 5.0f;
    float z = position.z();

    /* Wheel go down */
    if( event->delta() < 0 ){
        if( -z < (zFar-step) ){
            position.setZ(z-step);
            update_view();
            update();
        }
    }
    /* Wheel go up */
    else {
        if( -z > (zNear+step) ){
            position.setZ(z+step);
            update_view();
            update();
        }
    }
}

void
MeshViewerWidget::handle_key_events(QKeyEvent* event)
{
    float step = 5.0f;

    switch( event->key() ){
    case Qt::Key_Up :
        position.setZ(position.z()+step);
        update_view();
        break;

    case Qt::Key_Down :
        position.setZ(position.z()-step);
        update_view();
        break;

    case Qt::Key_Left :
        position.setX(position.x()+step);
        update_view();
        break;

    case Qt::Key_Right :
        position.setX(position.x()-step);
        update_view();
        break;

    case Qt::Key_V :
        default_view();
        update_view();
        break;

    case Qt::Key_W :
        makeCurrent();

        GLint mode;
        glGetIntegerv(GL_POLYGON_MODE, &mode);

        if( mode == GL_FILL )
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        doneCurrent();
        break;
    }

    update();
}

void
MeshViewerWidget::set_frames_per_second(size_t fps)
{
    frequency = long(1.0f/fps * 1000000);
}

void
MeshViewerWidget::update_lap()
{
    ++frames;
    lap = Clock::now();
}

// The delay between two time in microseconds
long
MeshViewerWidget::microseconds_diff(
        Clock::time_point t1,
        Clock::time_point t2)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(t1-t2).count();
}
