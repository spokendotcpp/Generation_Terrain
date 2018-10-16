#ifndef MESHVIEWERWIDGET_HPP
#define MESHVIEWERWIDGET_HPP

#include <QGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

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

/* Public methods */
public:
    MeshViewerWidget(QWidget *parent=nullptr);
    ~MeshViewerWidget() override;

    /* Qt OpenGL override functions */
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

/* Private methods */
private:

};

#endif // MESHVIEWERWIDGET_HPP
