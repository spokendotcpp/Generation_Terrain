#ifndef PLANE_H
#define PLANE_H

#include <QVector3D>
#include "drawableobject.h"

class Plane : public DrawableObject
{
private:

public:
    Plane();
    bool build(QOpenGLShaderProgram* program) override;
};

#endif // PLANE_H
