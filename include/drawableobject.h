#ifndef DRAWABLEOBJECT_H
#define DRAWABLEOBJECT_H

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class DrawableObject {

private:
    GLsizei nb_elements;

// (protected) -> only usable by inheritance
protected:
    QOpenGLVertexArrayObject* vao;
    QOpenGLBuffer* ebo;
    QOpenGLBuffer* vbo;

public:
    DrawableObject();
    virtual ~DrawableObject();

    virtual void init(QOpenGLShaderProgram* program) =0;
    virtual void show(GLenum mode) const;

    void drawable_elements(GLsizei amount);
    GLsizei drawable_elements() const;

    void free_buffers();
};

#endif // DRAWABLEOBJECT_H
