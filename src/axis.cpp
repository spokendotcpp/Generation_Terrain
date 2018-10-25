#include "../include/axis.h"
#include <iostream>

Axis::Axis(float _x, float _y, float _z, float _length)
    :DrawAbleObject(18, 6),
     x(_x), y(_y), z(_z),
     length(_length),
     raw_colors(nullptr),
     raw_vertices(nullptr),
     raw_indices(nullptr)
{
    float half = length/2.0f;

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

Axis::~Axis()
{
    if( raw_colors != nullptr ){
        delete [] raw_colors;
        raw_colors = nullptr;
    }

    if( raw_vertices != nullptr ){
        delete [] raw_vertices;
        raw_vertices = nullptr;
    }

    if( raw_indices != nullptr ){
        delete [] raw_indices;
        raw_indices = nullptr;
    }
}

void Axis::init(QOpenGLShaderProgram* program)
{
    buffers = new QOpenGLVertexArrayObject();
    if( buffers->create() ){
        buffers->bind();

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

        buffers->release();
        indices->release();
        vertices->release();
        colors->release();
    }
}

void Axis::show(GLenum mode) const
{
    buffers->bind();
    glDrawElements(mode, static_cast<GLsizei>(nb_indices), GL_UNSIGNED_INT, nullptr);
    buffers->release();
}
