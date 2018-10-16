#include <iostream>
#include "../include/meshviewerwidget.h"

MeshViewerWidget::MeshViewerWidget(QWidget* parent)
    :QGLWidget(parent)
{}

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

    unsigned int indices[] = {
        0, 2, 1
    };

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

    _vbo->release();
    _vao->release();
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* Painting happens here */

    _program->bind();
    _vao->bind();
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    _vao->release();
    _program->release();

}
