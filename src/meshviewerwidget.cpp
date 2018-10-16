#include <iostream>
#include "../include/meshviewerwidget.h"

MeshViewerWidget::MeshViewerWidget(QWidget* parent)
    :QGLWidget(parent)
{
    /* TODO ? */
}

MeshViewerWidget::~MeshViewerWidget()
{}

void
MeshViewerWidget::initializeGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Painting happens here */
}
