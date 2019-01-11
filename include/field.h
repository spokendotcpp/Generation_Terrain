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
    float height; // wanted range (-height, height) of your field (Oy)
    float length; // wanted length of your field (Oz)
    size_t size; // paving size of your field (number of segments)
    float** map;

    size_t _vertices;
    size_t _faces;

// Public methods
public:
    Field(float width, float height, float length, size_t power);
    ~Field() override;

    inline size_t nb_vertices() const { return _vertices; }
    inline size_t nb_faces() const{ return _faces; }

    void diamond_square();
    bool build(QOpenGLShaderProgram* program) override;
};

#endif // FIELD_H
