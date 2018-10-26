#ifndef AXIS_H
#define AXIS_H

#include "drawableobject.h"

class Axis : public DrawableObject
{
private:
    float x, y, z;
    float length;

public:
    Axis(float=0.0f, float=0.0f, float=0.0f, float=5.0f);

    void init(QOpenGLShaderProgram*) override;
    void show(GLenum=GL_POINTS) const override;
};

#endif // AXIS_H
