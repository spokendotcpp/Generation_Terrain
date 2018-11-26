#ifndef MESHVIEWERWIDGET_H
#define MESHVIEWERWIDGET_H

#include <chrono>

#include <QApplication>
#include <QScreen>
#include <QSize>

#include <QTimerEvent>

#include <QMouseEvent>
#include <QWheelEvent>

#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

#include <QMatrix4x4>

#include "axis.h"
#include "meshobject.h"
#include "light.h"
#include "arcball.h"

typedef std::chrono::steady_clock Clock;

/*
 * QGLWidget child class.
 */

class MeshViewerWidget: public QOpenGLWidget
{
/* Private members */
private:
    //ShaderProgram* program; // Shader program
    QOpenGLShaderProgram* program;

    /* Matrix which compose our Model View Projection Matrix -- uniform values */
    QMatrix4x4 view;
    QMatrix4x4 projection;

    // FPS frequency as microseconds
    long frequency;
    Clock::time_point lap;

    bool mouse_pressed;
    bool wheel_pressed;
    QPoint mouse;
    QVector3D position;
    QMatrix4x4 rotation;

    float fov;
    float zNear;
    float zFar;
    float window_ratio;

    size_t frames;

    ArcBall* arcball;
    MeshObject* bunny;
    Light* light;
    Axis* axis;

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

    /* QTimerEvent */
    void timerEvent(QTimerEvent*) override;

    /* Wheel */
    void wheelEvent(QWheelEvent*) override;

    /* Since we don't want our QMainWindow to lose Focus,
     * we handle key events with our own function.
     * See mainwindow.cpp > keyPressEvent method.
     */
    void handle_key_events(QKeyEvent*);

    /* caps frames per second */
    void set_frames_per_second(size_t fps);

    /* Difference between two high resolution clock time point as microseconds */
    static long microseconds_diff(
            Clock::time_point t1,
            Clock::time_point t2
    );

/* Private methods */
private:
    void default_view();
    void default_projection();
    void default_ModelViewPosition();

    void update_view();
    void update_projection();

    void update_lap();
};

#endif // MESHVIEWERWIDGET_H
