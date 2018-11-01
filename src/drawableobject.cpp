#include "../include/drawableobject.h"

#include <iostream>

DrawableObject::DrawableObject():
    nb_elements(0),
    vao(new QOpenGLVertexArrayObject()),
    ebo(new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer)),
    vbo(new QOpenGLBuffer())
{
   vao->create();
   ebo->create();
   vbo->create();
}


DrawableObject::~DrawableObject()
{
    // In case user didn't called it into overloaded `init()` function.
    free_buffers();

    if( vao != nullptr ){
        vao->destroy();
        delete vao;
        vao = nullptr;
    }

    nb_elements = 0;
}


void
DrawableObject::show(GLenum mode) const
{
    vao->bind();
    glDrawElements(mode, nb_elements, GL_UNSIGNED_INT, nullptr);
    vao->release();
}

void
DrawableObject::drawable_elements(GLsizei amount)
{
    nb_elements = amount;
}

GLsizei
DrawableObject::drawable_elements() const
{
    return nb_elements;
}

void
DrawableObject::free_buffers()
{
    if( ebo != nullptr ){
        ebo->destroy();
        delete ebo;
        ebo = nullptr;
    }

    if( vbo != nullptr ){
        vbo->destroy();
        delete vbo;
        vbo = nullptr;
    }
}
