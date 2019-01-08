#ifndef FIELD_H
#define FIELD_H

#include <random>

/* OpenMesh includes */
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

struct MyTraits : public OpenMesh::DefaultTraits {
    HalfedgeAttributes(OpenMesh::Attributes::PrevHalfedge);
};

typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits> MyMesh;
/* -- OpenMesh -- */

#include "drawableobject.h"

class Field: public DrawableObject {
// Attributes
private:
    size_t length;
    int** map;

// Public methods
public:
    Field(size_t length);
    ~Field() override;

    void diamond_square();
    bool build(QOpenGLShaderProgram* program) override;
};

#endif // FIELD_H
