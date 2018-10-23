#include "../include/axis.h"
#include <iostream>

Axis::Axis(float _x, float _y, float _z, float _length)
    :x(_x), y(_y), z(_z), length(_length)
{
    float half = length/2.0f;

    _vertices = new float[18] {
        x-half, y, z,
        x+half, y, z,
        x, y-half, z,
        x, y+half, z,
        x, y, z-half,
        x, y, z+half
    };

    _indices = new unsigned int [6] {
        0, 1,
        2, 3,
        4, 5,
    };
}

Axis::~Axis()
{
    if( _vertices != nullptr ){
        delete [] _vertices;
        _vertices = nullptr;
    }

    if( _indices != nullptr ){
        delete [] _indices;
        _vertices = nullptr;
    }
}

unsigned int*
Axis::indices() const
{
    return _indices;
}

float*
Axis::vertices() const
{
    return _vertices;
}
