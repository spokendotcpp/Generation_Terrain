#include <iostream>
#include "../include/meshviewerwidget.h"

MeshViewerWidget::MeshViewerWidget(QWidget* parent)
    :QGLWidget(parent)
{
    setMouseTracking(true);
    setFocus();

    // TODO
    _mouse_moving = false;
    _mouse = QPoint(0, 0);

    lap = HRClock::now();
}

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

void
MeshViewerWidget::update_ModelViewProjection()
{
    _MVP = _projection * _view * _model;
}

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
    };

    GLuint indices[] = {
        0, 2, 1
    };

    QSize ssize = QApplication::primaryScreen()->size();

    _model.setToIdentity();
    _view.setToIdentity();
    _projection.setToIdentity();

    // _model.rotate(15, QVector3D(0.0f, 0.0f, 1.0f));
    _view.translate(QVector3D(0.0f, 0.0f, -20.0f));
    _projection.perspective(45.0f, ssize.width()/ssize.height(), 0.1f, 100.0f);

    update_ModelViewProjection();

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
    glEnable(GL_CULL_FACE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void
MeshViewerWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    updateGL();
}

void
MeshViewerWidget::paintGL()
{
    auto current_time = HRClock::now();
    std::cerr << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(current_time-lap).count() << " ms" << std::endl;
    lap = current_time;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* Painting happens here */

    _program->bind();
    _vao->bind();

    // Send matrix_world value to Vertex Shader
    _program->setUniformValue(_loc_MVP, _MVP);

    // With Element Buffer Object :
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

    _vao->release();
    _program->release();
}

void
MeshViewerWidget::mouseMoveEvent(QMouseEvent* event)
{
    if( _mouse_moving ){
        float xoffset = event->pos().x() - _mouse.x();
        float yoffset = _mouse.y() - event->pos().y();

        _mouse = event->pos();

        float sensivity = 0.05f;
        xoffset *= sensivity;
        yoffset *= sensivity;


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
    float step = 1.0f;

    if( event->delta() < 0 ) _view.translate(0.0f, 0.0f, step);  /* Wheel go down */
    else                     _view.translate(0.0f, 0.0f, -step);  /* Wheel go up */

    update_ModelViewProjection();
    updateGL();
}
