#ifndef AXIS_H
#define AXIS_H

#include "drawableobject.h"

class Axis : public DrawableObject
{
private:
    float x, y, z;
    float length;

public:
    Axis(float x=0.0f, float y=0.0f, float z=0.0f, float length=5.0f);
    void init(QOpenGLShaderProgram* program);
};

#endif // AXIS_H
