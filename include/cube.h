#ifndef CUBE_H
#define CUBE_H

#include "drawableobject.h"

class Cube : public DrawableObject {
public:
    Cube();
    bool build(QOpenGLShaderProgram* program) override;
};

#endif // CUBE_H
