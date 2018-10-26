#ifndef MESHOBJECT_H
#define MESHOBJECT_H

#include <string>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include "drawableobject.h"

struct MyTraits : public OpenMesh::DefaultTraits {
    HalfedgeAttributes(OpenMesh::Attributes::PrevHalfedge);
};

typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits> MyMesh;

class MeshObject : public DrawableObject
{
public:
    MeshObject(const std::string&);
    ~MeshObject() override;
    void init(QOpenGLShaderProgram*) override;
    void show(GLenum) const override;
};

#endif // MESHOBJECT_H
