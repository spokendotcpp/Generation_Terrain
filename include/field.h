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
    float width; // wanted width of your field (Ox)
    float length; // wanted length of your field (Oz)
    size_t size; // paving size of your field (number of segments)
    float** map;

// Public methods
public:
    Field(float width, float height, size_t power);
    ~Field() override;

    void diamond_square();
    bool build(QOpenGLShaderProgram* program) override;
};

#endif // FIELD_H
