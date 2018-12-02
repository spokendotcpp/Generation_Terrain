#ifndef MESHOBJECT_H
#define MESHOBJECT_H

#include <string>
#include <QtMath>
#include "drawableobject.h"

class MeshObject : public DrawableObject
{
private:
    std::string filename;
    size_t _nb_faces;
    size_t _nb_vertices;

    MyMesh mesh;

    float* dihedral_angles;
    uint* valences;

private:
    void normalize();

public:
    MeshObject(const std::string&);
    ~MeshObject() override;

    const std::string& get_filename() const;
    const size_t& nb_faces() const;
    const size_t& nb_vertices() const;
    float mean_valence() const;
    float mean_dihedral() const;

    void Laplace_Beltrami_operator(float h, float lambda);

    void update_valence_vertices();
    void update_dihedral_angles();
    void update_normals();

    static float distance(MyMesh::Point p1, MyMesh::Point p2);

    bool build(QOpenGLShaderProgram* program) override;
};

#endif // MESHOBJECT_H
