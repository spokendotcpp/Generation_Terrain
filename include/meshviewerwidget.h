#ifndef MESHVIEWERWIDGET_H
#define MESHVIEWERWIDGET_H

#include <vector>
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

#include "../include/axis.h"
#include "../include/meshobject.h"

typedef std::chrono::high_resolution_clock HRClock;

/*
 * QGLWidget child class.
 */

class MeshViewerWidget: public QGLWidget
{
/* Private members */
private:
    //ShaderProgram* program; // Shader program
    QOpenGLShaderProgram* program;


    /* Matrix which compose our Model View Projection Matrix */
    QMatrix4x4 model;
    QMatrix4x4 view;
    QMatrix4x4 projection;

    std::chrono::time_point<HRClock> lap;

    bool mouse_pressed;
    bool wheel_pressed;
    QPoint mouse;

    QVector3D angle;
    QVector3D position;

    float fov;
    float zNear;
    float zFar;

    Axis* axis;
    MeshObject* bunny;

/* Public methods */
public:
    MeshViewerWidget(QWidget *parent=nullptr);
    ~MeshViewerWidget() override;

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

    /* Since we don't want our QMainWindow to lose Focus,
     * we handle key events with our own function.
     * See mainwindow.cpp > keyPressEvent method.
     */
    void handle_key_events(QKeyEvent*);

    float delay();

/* Private methods */
private:
    void default_model();
    void default_view();
    void default_projection();
    void default_ModelViewPosition();

    void update_model();
    void update_view();
    void update_projection();
};

#endif // MESHVIEWERWIDGET_H
