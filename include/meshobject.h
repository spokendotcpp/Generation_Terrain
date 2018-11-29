#ifndef MESHOBJECT_H
#define MESHOBJECT_H

#include <string>
#include "drawableobject.h"

class MeshObject : public DrawableObject
{
private:
    std::string filename;
    size_t _nb_faces;
    size_t _nb_vertices;

    double _mean_vertices_valence;
    double _mean_angles_dihedral;

    MyMesh mesh;

    void compute_mean_valence_vertices();
    void compute_mean_dihedral_angles();

public:
    MeshObject(const std::string&);
    ~MeshObject() override;

    const std::string& get_filename() const;
    const size_t& nb_faces() const;
    const size_t& nb_vertices() const;
    const double& mean_vertices_valence() const;
    const double& mean_angles_dihedral() const;

    void Laplace_Beltrami_operator(float h, float lambda);
    void update_normals();

    static float distance(MyMesh::Point p1, MyMesh::Point p2);

    bool build(QOpenGLShaderProgram* program) override;
};

#endif // MESHOBJECT_H
