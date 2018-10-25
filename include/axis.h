#ifndef AXIS_H
#define AXIS_H

#include "drawableobject.h"

class Axis : public DrawableObject
{
private:
    float x, y, z;
    float length;
    GLfloat* raw_colors;
    GLfloat* raw_vertices;
    GLuint* raw_indices;

public:
    Axis(float=0.0f, float=0.0f, float=0.0f, float=5.0f);
    ~Axis() override;

    void init(QOpenGLShaderProgram*) override;
    void show(GLenum=GL_POINTS) const override;
};

#endif // AXIS_H
