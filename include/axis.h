#ifndef AXIS_H
#define AXIS_H

#include "buffer.h"

class Axis
{
private:
    float x, y, z;
    float length;
    float* _vertices;
    unsigned int* _indices;

public:
    Axis(float=0.0f, float=0.0f, float=0.0f, float=5.0f);
    ~Axis();

    float* vertices() const;
    unsigned int* indices() const;
};

#endif // AXIS_H
