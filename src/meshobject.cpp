#include "../include/meshobject.h"

#include <iostream>

MeshObject::MeshObject(const std::string& _filename):
    DrawableObject(),
    filename(_filename),
    _nb_faces(0),
    _nb_vertices(0),
    _mean_vertices_valence(0),
    _mean_angles_dihedral(0)
{
    mesh.request_face_normals();
    mesh.request_vertex_normals();
    mesh.request_vertex_colors();

    OpenMesh::IO::Options opt;
    opt.set( OpenMesh::IO::Options::VertexNormal );

    OpenMesh::IO::read_mesh(mesh, filename, opt);

    if( !opt.check(OpenMesh::IO::Options::VertexNormal) ){
        std::cerr << "Vertex normal property was not found into " << filename << std::endl;
        std::cerr << "Calculating vertex normals ... ";
        mesh.update_face_normals();
        mesh.update_vertex_normals();
        std::cerr << "DONE" << std::endl;
    }

    _nb_faces = mesh.n_faces();
    _nb_vertices = mesh.n_vertices();

    compute_mean_dihedral_angles();
    compute_mean_valence_vertices();
}

MeshObject::~MeshObject()
{
    mesh.release_vertex_normals();
    mesh.release_face_normals();
    mesh.release_vertex_colors();
}

void
MeshObject::compute_mean_valence_vertices()
{
    _mean_vertices_valence = 0.0;

    for(const auto& v_it: mesh.vertices())
        _mean_vertices_valence += double(mesh.valence(v_it));

    _mean_vertices_valence /= double(_nb_vertices);
}

void
MeshObject::compute_mean_dihedral_angles()
{
    _mean_angles_dihedral = 0.0;

    for(const auto& e_it: mesh.edges())
        _mean_angles_dihedral += double(mesh.calc_dihedral_angle(e_it));

    _mean_angles_dihedral/= double(mesh.n_edges());
}

const std::string&
MeshObject::get_filename() const
{
    return filename;
}

const size_t&
MeshObject::nb_faces() const
{
    return _nb_faces;
}

const size_t&
MeshObject::nb_vertices() const
{
    return _nb_vertices;
}

const double&
MeshObject::mean_vertices_valence() const
{
    return _mean_vertices_valence;
}

const double&
MeshObject::mean_angles_dihedral() const
{
    return _mean_angles_dihedral;
}


bool
MeshObject::build(QOpenGLShaderProgram* program)
{
    size_t nb_vertices = mesh.n_vertices();
    size_t nb_elements = mesh.n_faces()*3;

    GLuint* indices = new GLuint[nb_elements];
    GLfloat* positions = new GLfloat[nb_vertices*3];
    GLfloat* normals = new GLfloat[nb_vertices*3];
    GLfloat* colors = new GLfloat[nb_vertices*3];

    MyMesh::Normal normal;
    MyMesh::Point point;
    MyMesh::Color color;
    MyMesh::ConstFaceVertexIter cfv_it;

    size_t i = 0;
    size_t j = 0;

    for(const auto& cv_it: mesh.vertices()){
        /* const vertex iterator */
        normal = mesh.normal(cv_it);
        point = mesh.point(cv_it);
        color = mesh.color(cv_it);
        for(j=0; j < 3; ++j, ++i){
            normals[i] = normal[j];
            positions[i] = point[j];
            colors[i] = colors[j];
        }
    }

    i = 0;
    for(const auto& cf_it: mesh.faces()){
        /* const face iterator */
        cfv_it = mesh.cfv_iter(cf_it);
        for(j=0; j < 3; ++j, ++i, ++cfv_it)
            indices[i] = GLuint(cfv_it->idx());
    }

    set_vertices_geometry(program->attributeLocation("position"), positions, indices);
    set_vertices_colors(program->attributeLocation("color"), colors);
    set_vertices_normals(program->attributeLocation("normal"), normals);

    return initialize(nb_vertices, nb_elements, 3);
}
