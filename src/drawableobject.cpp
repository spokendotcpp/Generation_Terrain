#include "../include/drawableobject.h"

#include <iostream>

DrawableObject::DrawableObject():
    nb_vertices(0),
    nb_indices(0),
    buffers(nullptr),
    vertices(nullptr),
    colors(nullptr),
    indices(nullptr),
    raw_vertices(nullptr),
    raw_colors(nullptr),
    raw_indices(nullptr)
{}


DrawableObject::~DrawableObject()
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

    if( raw_vertices != nullptr ){
        delete [] raw_vertices;
        raw_vertices = nullptr;
    }

    if( raw_colors != nullptr ){
        delete [] raw_colors;
        raw_colors = nullptr;
    }


    if( raw_indices != nullptr ){
        delete [] raw_indices;
        raw_indices = nullptr;
    }

    nb_vertices = 0;
    nb_indices = 0;

    std::cerr << "DrawableObject destroyed" << std::endl;
}

size_t DrawableObject::bytes_i() const
{
    return sizeof(GLuint) * nb_indices;
}

size_t DrawableObject::bytes_v() const
{
    return sizeof(GLfloat) * nb_vertices;
}
