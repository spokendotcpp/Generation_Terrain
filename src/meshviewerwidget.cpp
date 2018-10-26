#include <iostream>
#include "../include/meshviewerwidget.h"
#include "../include/axis.h"

MeshViewerWidget::MeshViewerWidget(QWidget* parent)
    :QGLWidget(parent)
{
    setMouseTracking(true);
    mouse_pressed = false;
    lap = HRClock::now();

    axis = new Axis(0.0f, 0.0f, 0.0f, 1.0f);
    bunny = new MeshObject("../mesh_files/bunnyLowPoly.obj");
}

/*
 * Destructor:
 *
 * Destroy every Buffers (Vertex Buffer Object: ARRAY_BUFFER, INDEX_BUFFER AND Vertex Array Object).
 * Free memory.
*/
MeshViewerWidget::~MeshViewerWidget()
{
    if( program != nullptr ){
        delete program;
        program = nullptr;
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
        static_cast<float>(size().width()/size().height()),
        zNear, zFar
    );
}

/* Matrix computation on CPU
 * -> must be call only when needed.
 * i.e.: before rendering.
*/
void
MeshViewerWidget::update_ModelViewProjection()
{   
    MVP = projection * view * model;
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
    position = QVector3D(0.0f, 0.0f, -2.0f);
    update_view();
}

/* Load default values for the projection matrix + update */
void
MeshViewerWidget::default_projection()
{
    fov = 45.0f;
    zNear = 0.1f;
    zFar = 100.0f;
    update_projection();
}

/* Load every defaults */
void
MeshViewerWidget::default_positions()
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
    if( !context()->isValid() ){
        std::cerr << "Failed to init OpenGL context" << std::endl;
        return;
    }

    program = new QOpenGLShaderProgram();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, "../shaders/simple.vert.glsl");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, "../shaders/simple.frag.glsl");
    program->link();
    program->bind();
    {
        loc_MVP = program->uniformLocation("MVP");
        axis->init(program);
        bunny->init(program);
    }
    program->release();


    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glClearColor(1.0f, 191.0f/255.0f, 179.0f/255.0f, 1.0f);
    default_positions();
}

/* When window (this widget) is resized */
void
MeshViewerWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    update_projection();
    updateGL();
}

/* RENDER TIME */
void
MeshViewerWidget::paintGL()
{
    auto current_time = HRClock::now();
    std::cerr << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(current_time-lap).count() << " ms" << std::endl;
    lap = current_time;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* update the MVP matrix */
    update_ModelViewProjection();    

    program->bind();
    {
        // Send matrix_world value to Vertex Shader
        program->setUniformValue(loc_MVP, MVP);

        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        axis->show(GL_LINES);
        bunny->show(GL_TRIANGLES);
    }
    program->release();
}

/* When mouse is moving inside the widget */
void
MeshViewerWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pos = event->pos();

    if( mouse_pressed ){
        angle.setX(angle.x() + pos.y() - mouse.y());
        angle.setY(angle.y() + pos.x() - mouse.x());
        update_view();
        updateGL();
    }

    mouse = pos;
}

void
MeshViewerWidget::mousePressEvent(QMouseEvent* event)
{
    if( event->button() == Qt::MouseButton::LeftButton )
       mouse_pressed = true;
}

void
MeshViewerWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if( event->button() == Qt::MouseButton::LeftButton )
        mouse_pressed = false;
}

/*
 * Make sure that position.z()
 * stay between ]zNear+step & zFar-step[
*/
void
MeshViewerWidget::wheelEvent(QWheelEvent* event)
{
    float step = 0.25f;
    float z = position.z();

    /* Wheel go down */
    if( event->delta() < 0 ){
        if( -z < (zFar-step) )
            position.setZ(z-step);
    }
    /* Wheel go up */
    else {
        if( -z > (zNear+step) )
            position.setZ(z+step);
    }

    update_view();
    updateGL();
}

void
MeshViewerWidget::handle_key_events(QKeyEvent* event)
{
    float step = 0.25f;
    int key = event->key();

    if( key == Qt::Key_Up )
        position.setZ(position.z()+step);
    else
    if( key == Qt::Key_Down )
        position.setZ(position.z()-step);
    else
    if( key == Qt::Key_Left )
        position.setX(position.x()+step);
    else
    if( key == Qt::Key_Right )
        position.setX(position.x()-step);
    else
    if( key == Qt::Key_V )
        default_view();

    update_view();
    updateGL();
}
