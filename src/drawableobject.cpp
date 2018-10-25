#include "../include/drawableobject.h"

#include <iostream>

DrawAbleObject::DrawAbleObject(size_t vts, size_t idx)
    :nb_vertices(vts), nb_indices(idx),
    buffers(nullptr), vertices(nullptr),
    colors(nullptr), indices(nullptr)
{}


DrawAbleObject::~DrawAbleObject()
{
    if( buffers != nullptr ){
        buffers->destroy();
        delete buffers;
        buffers = nullptr;
    }

    if( vertices != nullptr ){
        vertices->destroy();
        delete vertices;
        vertices = nullptr;
    }

    if( colors != nullptr ){
        colors->destroy();
        delete colors;
        colors = nullptr;
    }

    if( indices != nullptr ){
        indices->destroy();
        delete indices;
        indices = nullptr;
    }
}

size_t DrawAbleObject::bytes_i() const
{
    return sizeof(GLuint) * nb_indices;
}

size_t DrawAbleObject::bytes_v() const
{
    return sizeof(GLfloat) * nb_vertices;
}
