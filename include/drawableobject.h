#ifndef DRAWABLEOBJECT_H
#define DRAWABLEOBJECT_H

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

// TODO : ERASE FROM MEMORY RAW ARRAY BEFORE DRAWING

class DrawableObject {
public:
    size_t nb_vertices;
    size_t nb_indices;

protected:
    QOpenGLVertexArrayObject* buffers;
    QOpenGLBuffer* vertices;
    QOpenGLBuffer* colors;
    QOpenGLBuffer* indices;

    GLfloat* raw_vertices;
    GLfloat* raw_colors;
    GLuint* raw_indices;

public:
    DrawableObject();
    virtual ~DrawableObject();

    size_t bytes_v() const;
    size_t bytes_i() const;

    virtual void init(QOpenGLShaderProgram*) =0;
    virtual void show(GLenum mode) const =0;
};

#endif // DRAWABLEOBJECT_H
