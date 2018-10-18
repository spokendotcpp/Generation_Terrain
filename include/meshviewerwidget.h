#ifndef MESHVIEWERWIDGET_HPP
#define MESHVIEWERWIDGET_HPP

#include <chrono>

#include <QApplication>
#include <QScreen>
#include <QSize>

#include <QMouseEvent>
#include <QWheelEvent>

#include <QGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

typedef std::chrono::high_resolution_clock HRClock;

/*
 * QGLWidget child class.
 */

class MeshViewerWidget: public QGLWidget
{
/* Private members */
private:
    QOpenGLVertexArrayObject* _vao;
    QOpenGLBuffer* _vbo;
    QOpenGLShaderProgram* _program;
    QOpenGLBuffer* _ebo; // Element Buffer Object

    /* Matrix which compose our Model View Projection Matrix */
    QMatrix4x4 _model;
    QMatrix4x4 _view;
    QMatrix4x4 _projection;

    /* The Model <-> View <-> Projection Matrix */
    QMatrix4x4 _MVP;
    GLint _loc_MVP; // ID which link variable between CPU & GPU

    std::chrono::time_point<HRClock> lap;

    bool _mouse_moving;
    QPoint _mouse;

/* Public methods */
public:
    MeshViewerWidget(QWidget *parent=nullptr);
    ~MeshViewerWidget() override;

    void update_ModelViewProjection();

    /* Qt OpenGL override functions */
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

    /* Qt override functions */
    /* Mouse */
    void mouseMoveEvent(QMouseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

    /* Wheel */
    void wheelEvent(QWheelEvent*) override;

/* Private methods */
private:

};

#endif // MESHVIEWERWIDGET_HPP
