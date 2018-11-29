#include "../include/meshobject.h"

#include <iostream>
#include <QtMath>
#include <cmath>

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

    if( OpenMesh::IO::read_mesh(mesh, filename) ){
        _nb_faces = mesh.n_faces();
        _nb_vertices = mesh.n_vertices();

        compute_mean_dihedral_angles();
        compute_mean_valence_vertices();

        Laplace_Beltrami_operator();

        std::cout << "Calculating vertex normals ... ";
        mesh.update_face_normals();
        mesh.update_vertex_normals();
        std::cout << "DONE" << std::endl;
    }
    else {
        std::cerr << "Failed to read " << filename << " file" << std::endl;
    }
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

float distance(MyMesh::Point p1, MyMesh::Point p2)
{
    return std::sqrt(
        std::pow(p2[0]-p1[0], 2.0f)+
        std::pow(p2[1]-p1[1], 2.0f)+
        std::pow(p2[2]-p1[2], 2.0f));
}

void
MeshObject::Laplace_Beltrami_operator()
{
    MyMesh::ConstVertexVertexCCWIter cvv_ccwit;
    MyMesh::Point vi_left, vi_right, vi, v;

    float area;
    float alpha;
    float beta;

    float* deltas = new float[mesh.n_vertices()];
    size_t i=0;

    // For all vertices
    for(auto& v_it: mesh.vertices()){
        /* Const Vertex Vertex Counter-Clock Wise Iterator */
        cvv_ccwit = mesh.cvv_ccwiter(v_it);
        v = mesh.point(v_it);

        size_t n = 0;
        float sum = 0.0f;
        area = 0.0f;

        // First ring neighbors
        while( cvv_ccwit != mesh.cvv_ccwend(v_it) ){
            vi_left = mesh.point(*cvv_ccwit);
            vi = mesh.point(*(++cvv_ccwit));
            vi_right = mesh.point(*(++cvv_ccwit));

            MyMesh::Point side_vi_left1 = (vi_left - vi).normalize();
            MyMesh::Point side_vi_left2 = (vi_left - v).normalize();

            MyMesh::Point side_vi_right1 = (vi_right - vi).normalize();
            MyMesh::Point side_vi_right2 = (vi_right - v).normalize();

            float vi_minus_v = distance(vi, v);

            // compute Alpha & Beta angles
            // angle = dot_product / product of norms
            alpha = std::acos(
                  ( side_vi_left1|side_vi_left2 )
                / ( side_vi_left1.norm()*side_vi_left2.norm() )
            );

            beta = std::acos(
                  ( side_vi_right1|side_vi_right2 )
                / ( side_vi_right1.norm()*side_vi_right2.norm() )
            );

            float alpha_degree = qRadiansToDegrees(alpha);
            float beta_degree = qRadiansToDegrees(beta);

            float cot_alpha = std::cos(alpha_degree)/std::sin(alpha_degree);
            float cot_beta = std::cos(beta_degree)/std::sin(beta_degree);

            float sum_cot = cot_alpha + cot_beta;

            sum += (sum_cot * vi_minus_v);

            MyMesh::Point barycenter_l = (vi_left + vi + v) / 3;
            MyMesh::Point barycenter_r = (vi_right + vi + v) / 3;

            float a = distance(v, barycenter_l);
            float b = distance(v, barycenter_r);
            float c = distance(barycenter_l, barycenter_r);
            float p = (a+b+c)/2.0f;

            area += std::sqrt(p*(p-a)*(p-b)*(p-c));

            // Since we `++` two times, we want to fallback once for the next iteration.
            --cvv_ccwit;
            ++n;
        }

        deltas[i++] = 0.5f*area * sum;

        //mesh.point(v_it) = MyMesh::Point(v[0]+delta, v[1]+delta, v[2]+delta);
    }

    i = 0;
    for(auto& v: mesh.vertices())
    {
        MyMesh::Point p = mesh.point(v);
        mesh.point(v) = MyMesh::Point(p[0]+deltas[i], p[1]+deltas[i], p[2]+deltas[i]);
        i++;
    }
}

