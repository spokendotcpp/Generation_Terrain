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
    QOpenGLVertexArrayObject* _vao; // Vertex Array Object
    QOpenGLBuffer* _vbo; // Vertex Buffer Object
    QOpenGLShaderProgram* _program; // Shader program
    QOpenGLBuffer* _ebo; // Element Buffer Object

    /* Matrix which compose our Model View Projection Matrix */
    QMatrix4x4 _model;
    QMatrix4x4 _view;
    QMatrix4x4 _projection;

    /* The Model <-> View <-> Projection Matrix */
    QMatrix4x4 _MVP;
    GLint _loc_MVP; // ID which link variable between CPU & GPU

    // Time elapsed between the two last frames.
    std::chrono::time_point<HRClock> lap;

    bool _mouse_moving;
    QPoint _mouse;

    QVector3D _angle;
    QVector3D _position;

    float zNear;
    float zFar;

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

/* Private methods */
private:
    void default_model();
    void default_view();
    void default_projection();
    void default_positions();

    void update_model();
    void update_view();
    void update_projection();
    void update_ModelViewProjection();
};

#endif // MESHVIEWERWIDGET_HPP
