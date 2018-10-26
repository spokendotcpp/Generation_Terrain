#include "../include/axis.h"
#include <iostream>

Axis::Axis(float _x, float _y, float _z, float _length)
    :DrawableObject(),
     x(_x), y(_y), z(_z),
     length(_length)
{
    float half = length/2.0f;

    nb_vertices = 18;
    nb_indices = 6;

    raw_vertices = new GLfloat[nb_vertices] {
        x-half, y, z,
        x+half, y, z,
        x, y-half, z,
        x, y+half, z,
        x, y, z-half,
        x, y, z+half
    };

    raw_colors = new GLfloat[nb_vertices] {
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
    };

    raw_indices = new GLuint[nb_indices] {
        0, 1,
        2, 3,
        4, 5,
    };
}

void Axis::init(QOpenGLShaderProgram* program)
{
    vao = new QOpenGLVertexArrayObject();
    if( vao->create() ){
        vao->bind();

        int loc_position = program->attributeLocation("position");
        int loc_colors = program->attributeLocation("inColor");

        indices = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
        if( indices->create() ){
            indices->bind();
            indices->setUsagePattern(QOpenGLBuffer::StaticDraw);
            indices->allocate(raw_indices, static_cast<int>(bytes_i()));
        }

        vertices = new QOpenGLBuffer();
        if( vertices->create() ){
            vertices->bind();
            vertices->setUsagePattern(QOpenGLBuffer::StaticDraw);
            vertices->allocate(raw_vertices, static_cast<int>(bytes_v()));
        }

        program->enableAttributeArray(loc_position);
        program->setAttributeArray(loc_position, GL_FLOAT, nullptr, 3);

        colors = new QOpenGLBuffer();
        if( colors->create() ){
            colors->bind();
            colors->setUsagePattern(QOpenGLBuffer::StaticDraw);
            colors->allocate(raw_colors, static_cast<int>(bytes_v()));
        }

        program->enableAttributeArray(loc_colors);
        program->setAttributeArray(loc_colors, GL_FLOAT, nullptr, 3);

        vao->release();
        indices->release();
        vertices->release();
        colors->release();

        free_raw_memory();
    }
}

void Axis::show(GLenum mode) const
{
    vao->bind();
    glDrawElements(mode, static_cast<GLsizei>(nb_indices), GL_UNSIGNED_INT, nullptr);
    vao->release();
}
