#include <iostream>
#include "../include/meshviewerwidget.h"

MeshViewerWidget::MeshViewerWidget(QWidget* parent)
    :QGLWidget(parent)
{
    setMouseTracking(true);
    _mouse_moving = false;
    lap = HRClock::now();
}

/*
 * Destructor:
 *
 * Destroy every Buffers (Vertex Buffer Object: ARRAY_BUFFER, INDEX_BUFFER AND Vertex Array Object).
 * Free memory.
*/
MeshViewerWidget::~MeshViewerWidget()
{
    _vao->destroy();
    _vbo->destroy();
    _ebo->destroy();

    delete _vao;
    delete _ebo;
    delete _vbo;
    delete _program;
}

/* Update the model matrix
 * -> no specific actions yet.
*/
void
MeshViewerWidget::update_model()
{
    _model.setToIdentity();
}

/* Update the view matrix
 * i.e.: can be called after every user inputs (mouse, keyboard ...)
*/
void
MeshViewerWidget::update_view()
{
    _view.setToIdentity();
    _view.translate(_position);
    _view.rotate(_angle.x(), 1.0f, 0.0f, 0.0f);
    _view.rotate(_angle.y(), 0.0f, 1.0f, 0.0f);
}

/* Update the projection matrix
 * -> used when changing window size.
*/
void
MeshViewerWidget::update_projection()
{
    _projection.setToIdentity();
    _projection.perspective(
        45.0f,
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
    _MVP = _projection * _view * _model;
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
    _angle = QVector3D(-45.0f, 45.0f, 0.0f);
    _position = QVector3D(0.0f, 0.0f, -2.0f);
    update_view();
}

/* Load default values for the projection matrix + update */
void
MeshViewerWidget::default_projection()
{
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

    GLfloat vertices[] = {
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5, 0.0f,
        -0.5f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
    };

    GLuint indices[] = {
        0, 2, 1,
        2, 3, 1
    };

    default_positions();

    _program = new QOpenGLShaderProgram();
    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert.glsl");
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag.glsl");
    _program->link();
    _program->bind();

    _vao = new QOpenGLVertexArrayObject();
    _vao->create();
    _vao->bind();

    _vbo = new QOpenGLBuffer();
    _vbo->create();
    _vbo->bind();
    _vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    _vbo->allocate(vertices, sizeof(vertices));

    _ebo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    _ebo->create();
    _ebo->bind();
    _ebo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    _ebo->allocate(indices, sizeof(indices));

    _program->enableAttributeArray(0);
    _program->setAttributeBuffer(0, GL_FLOAT, 0, 3);

    // bind the location of our futur data into shader
    _loc_MVP = _program->uniformLocation("MVP");

    _vao->release();
    _vbo->release();
    _ebo->release();

    _program->release();

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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

    _program->bind();
    _vao->bind();

    // Send matrix_world value to Vertex Shader
    _program->setUniformValue(_loc_MVP, _MVP);

    // With Element Buffer Object :
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    _vao->release();
    _program->release();
}

/* When mouse is moving inside the widget */
void
MeshViewerWidget::mouseMoveEvent(QMouseEvent* event)
{
    if( _mouse_moving ){
        QPoint pos = event->pos();

        _angle.setX(_angle.x() + pos.y() - _mouse.y());
        _angle.setY(_angle.y() + pos.x() - _mouse.x());
        _mouse = pos;

        update_view();
        updateGL();
    }
}

void
MeshViewerWidget::mousePressEvent(QMouseEvent* event)
{
    if( event->button() == Qt::MouseButton::LeftButton )
       _mouse_moving = true;
}

void
MeshViewerWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if( event->button() == Qt::MouseButton::LeftButton )
        _mouse_moving = false;
}

void
MeshViewerWidget::wheelEvent(QWheelEvent* event)
{
    float step = 0.25f; // TODO? change this by something more efficient
    // i.e.: more the object go far, more we slow speed step.

    /* TODO:
     * Test if object goes outside of zNear & zFar positions.
     */

    if( event->delta() < 0 ) _position.setZ(_position.z()+step);  /* Wheel go down */
    else                     _position.setZ(_position.z()-step);  /* Wheel go up */

    update_view();
    updateGL();
}

void
MeshViewerWidget::handle_key_events(QKeyEvent* event)
{
    float step = 0.25f;
    int key = event->key();

    if( key == Qt::Key_Up )
        _position.setZ(_position.z()-step);
    else
    if( key == Qt::Key_Down )
        _position.setZ(_position.z()+step);
    else
    if( key == Qt::Key_Left )
        _position.setX(_position.x()+step);
    else
    if( key == Qt::Key_Right )
        _position.setX(_position.x()-step);

    update_view();
    updateGL();
}
