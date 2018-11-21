#ifndef MESHOBJECT_H
#define MESHOBJECT_H

#include <string>
#include "drawableobject.h"

class MeshObject : public DrawableObject
{
private:
    MyMesh mesh;

public:
    MeshObject(const std::string&);
    ~MeshObject() override;

    bool build(QOpenGLShaderProgram* program) override;
};

#endif // MESHOBJECT_H
