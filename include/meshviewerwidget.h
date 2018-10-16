#ifndef MESHVIEWERWIDGET_HPP
#define MESHVIEWERWIDGET_HPP

#include <QtOpenGL>
#include <QGLWidget>

/*
 * QGLWidget child class.
 */

class MeshViewerWidget: public QGLWidget
{
public:
    MeshViewerWidget(QWidget *parent=nullptr);
    ~MeshViewerWidget();

    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
};

#endif // MESHVIEWERWIDGET_HPP
