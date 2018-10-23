#ifndef MESHVIEWERWIDGET_HPP
#define MESHVIEWERWIDGET_HPP

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

typedef std::chrono::high_resolution_clock HRClock;

/*
 * QGLWidget child class.
 */

class MeshViewerWidget: public QGLWidget
{
/* Private members */
private:
    //std::vector<QOpenGLBuffer*> vbos;
    //std::vector<QOpenGLBuffer*> ebos;

    //QOpenGLVertexArrayObject* vao; // Vertex Array Object
    //QOpenGLBuffer* vbo; // Vertex Buffer Object
    QOpenGLShaderProgram* program; // Shader program
    //QOpenGLBuffer* ebo; // Element Buffer Object

    /* Matrix which compose our Model View Projection Matrix */
    QMatrix4x4 model;
    QMatrix4x4 view;
    QMatrix4x4 projection;

    /* The Model <-> View <-> Projection Matrix */
    QMatrix4x4 MVP;
    GLint loc_MVP; // ID which link variable between CPU & GPU

    // Time elapsed between the two last frames.
    std::chrono::time_point<HRClock> lap;

    bool mouse_pressed;
    QPoint mouse;

    QVector3D angle;
    QVector3D position;

    float fov;
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
