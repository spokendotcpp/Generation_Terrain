#ifndef ICOSPHERE_H
#define ICOSPHERE_H

#include <math.h>
#include <deque>

#include "drawableobject.h"

class IcoSphere : public DrawableObject {
private:
    size_t iterations;

public:
    IcoSphere(size_t iterations=0);
    ~IcoSphere() override;

    bool build(QOpenGLShaderProgram* program) override;
};

#endif // ICOSPHERE_H
