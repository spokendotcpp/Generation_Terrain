#include <iostream>
#include <thread>
#include <future>

#include "../include/meshviewerwidget.h"
#include "../include/mainwindow.h"

MeshViewerWidget::MeshViewerWidget(QWidget* parent)
    :QOpenGLWidget(parent)
{
    setMouseTracking(true);

    mouse_pressed = false;
    wheel_pressed = false;

    wireframe_on = true;
    fill_on = true;

    smooth_on = true;

    frames = 0;
    set_frames_per_second(60);
    lap = Clock::now();

    arcball = nullptr;
    program = nullptr;
    obj = nullptr;
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

    if( obj != nullptr ){
        delete obj;
        obj = nullptr;
    }
}

/* Update the view matrix
 * i.e.: can be called after every user inputs (mouse, keyboard ...)
 *
*/
void
MeshViewerWidget::update_view()
{
    view.setToIdentity();
    view.translate(position);
    view *= rotation;
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

/* Load default values for the view matrix + update */
void
MeshViewerWidget::default_view()
{
    rotation = QMatrix4x4();
    rotation.rotate(25.0f, 1.0f, 1.0f, 0.0f);

    position = QVector3D(0.0f, 0.0f, -2.0f);
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

    program = new QOpenGLShaderProgram();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, "../shaders/simple.vert.glsl");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, "../shaders/simple.frag.glsl");
    program->link();
    program->bind();
    {
        light = new Light();
        light->set_position(0.0f, 100.0f, 200.0f, program->uniformLocation("light_position"))
             ->set_color(0.6f, 0.6f, 0.6f, program->uniformLocation("light_color"))
             ->set_ambient(0.4f, program->uniformLocation("light_ambient"))
             ->enable(program->uniformLocation("light_on"));

        axis->build(program);
        axis->update_buffers(program);

        program->setUniformValue("wireframe_color", QVector3D(1.0f, 0.0f, 0.0f));
    }
    program->release();

    glClearColor(1.0f, 191.0f/255.0f, 179.0f/255.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    glPolygonMode(GL_FRONT, GL_FILL);

    default_ModelViewPosition();

    // IDLE Function
    timer_id_0 = startTimer(0);
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
            std::chrono::microseconds((frequency-100) - mcs)
        );
    }

    update_lap();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program->bind();
    {
        program->setUniformValue("smooth_on", smooth_on);

        // in case user has modified light pos
        light->to_gpu(program);

        // TODO: DrawableObject method to compute directly transposed(inverted) matrix once.
        program->setUniformValue("projection", projection);
        program->setUniformValue("view", view);
        program->setUniformValue("view_inverse", view.transposed().inverted());

        // draw obj
        if( obj != nullptr ){
            program->setUniformValue("model", obj->model_matrix());
            program->setUniformValue("model_inverse", obj->model_matrix().transposed().inverted());

            if( fill_on )
                obj->show(GL_TRIANGLES);

            if( wireframe_on ){
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                program->setUniformValue("wireframe_on", 1);
                obj->show(GL_TRIANGLES);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glDepthFunc(GL_LESS);
                program->setUniformValue("wireframe_on", 0);
            }
        }


        // draw axis
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

    if( mouse_pressed ){
        // The new rotation equals arcball rotation * the last rotation.
        rotation = arcball->get_rotation_matrix(
            pos.x(), pos.y(), mouse.x(), mouse.y()
        ) * rotation;

        update_view();
        update();
    }
    else
    if( wheel_pressed ){
        float step = position.z() / (zFar - zNear);

        position.setX(position.x() - ((pos.x() - mouse.x())*step));
        position.setY(position.y() + ((pos.y() - mouse.y())*step));

        update_view();
        update();
    }

    mouse = pos;
}

void
MeshViewerWidget::mousePressEvent(QMouseEvent* event)
{
    if( event->button() == Qt::MouseButton::LeftButton ){
        mouse_pressed = true;
        mouse = event->pos();
    }
    else
    if( event->button() == Qt::MouseButton::MiddleButton ){
        wheel_pressed = true;
        mouse = event->pos();
    }
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

    if( id == timer_id_0 ){
        update();
    }
}

/*
 * Make sure that position.z()
 * stay between ]zNear+step & zFar-step[
*/
void
MeshViewerWidget::wheelEvent(QWheelEvent* event)
{
    float z = position.z();
    float step = 0.1f;

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
    float step = 0.1f;

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
            glPolygonMode(GL_FRONT, GL_LINE);
        else
            glPolygonMode(GL_FRONT, GL_FILL);

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

size_t
MeshViewerWidget::get_computed_frames() const
{
    return frames;
}

void
MeshViewerWidget::reset_computed_frames()
{
    frames = 0;
}

void
MeshViewerWidget::update_lap()
{
    ++frames;
    lap = Clock::now();
}

void
MeshViewerWidget::smooth_render(bool on)
{
    smooth_on = on;
    update();
}

// The delay between two time in microseconds
long
MeshViewerWidget::microseconds_diff(
        Clock::time_point t1,
        Clock::time_point t2)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(t1-t2).count();
}

void
MeshViewerWidget::get_obj_from_filesystem(const std::string& filename)
{
    MeshObject* new_obj = new MeshObject(filename);
    if( new_obj != nullptr ){
        if( obj != nullptr ){
            delete obj;
            obj = nullptr;
        }

        makeCurrent();
        program->bind();
        {
           new_obj->build(program);
           new_obj->use_unique_color(0.0f, 1.0f, 1.0f);
           new_obj->update_buffers(program);
           obj = new_obj;
        }
        program->release();
        doneCurrent();
    }
    else
        std::cerr << "Failed to load mesh file." << std::endl;
}

void
MeshViewerWidget::display_wireframe(bool mode)
{
    wireframe_on = mode;
    update();
}

void
MeshViewerWidget::display_fill(bool mode)
{
    fill_on = mode;
    update();
}

void
MeshViewerWidget::reset_view()
{
    default_view();
    update_view();
    update();
}

void
MeshViewerWidget::set_scale_factor(float factor)
{
    if( obj != nullptr ){
        obj->reset_model_matrix();
        obj->scale(factor, factor, factor);
        update();
    }
}

void
MeshViewerWidget::set_light_position(int x, int y, int z)
{
    if( light != nullptr ){
        if( program != nullptr ){
            makeCurrent();
            program->bind();
            light->update_position(float(x), float(y), float(z))->to_gpu(program);
            program->release();
            doneCurrent();
        }
    }
}

QString
MeshViewerWidget::status_message()
{
    QString msg;
    if( obj != nullptr ){
        msg.append("Filename: ");
        msg.append(QString::fromStdString(obj->get_filename()));
        msg.append(" | Vertices: ");
        msg.append(QString::number(obj->nb_vertices()));
        msg.append(" | Faces: ");
        msg.append(QString::number(obj->nb_faces()));
        msg.append(" | Mean valence: ");
        msg.append(QString::number(double(obj->mean_valence())));
        msg.append(" | Mean dihedral angle: ");
        msg.append(QString::number(qRadiansToDegrees(double(obj->mean_dihedral()))));
    }
    return msg;
}

void
MeshViewerWidget::apply_Laplace_Beltrami()
{
    int r = QInputDialog::getInt(this, "Laplace-Beltrami", "number of iterations:", 1);
    if( r < 0 )
        r = 0;

    for(int i=0; i < r; ++i){
        obj->apply_Laplace_Beltrami(1.0f, 0.00009f);
    }

    makeCurrent();
    program->bind();
    {
        obj->update_normals();
        obj->update_dihedral_angles();
        obj->build(program);
        obj->use_unique_color(0.0f, 1.0f, 1.0f);
        obj->update_buffers(program);
    }
    program->release();
    doneCurrent();
}
