#ifndef MESHVIEWERWIDGET_HPP
#define MESHVIEWERWIDGET_HPP

#include <QMouseEvent>
#include <QWheelEvent>

#include <QGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

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

    QMatrix4x4 _model;
    QMatrix4x4 _view;
    QMatrix4x4 _projection;

    QMatrix4x4 _MVP;

    QMatrix4x4 _matrix_world;
    GLint _loc_matrix_world;

    bool _mouse_moving;
    int _mouse_x;
    int _mouse_y;

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
