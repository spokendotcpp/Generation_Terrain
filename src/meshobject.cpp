#include "../include/meshobject.h"

#include <iostream>
#include <limits>

MeshObject::MeshObject(const std::string& _filename):
    DrawableObject(),
    filename(_filename),
    _nb_faces(0),
    _nb_vertices(0),
    dihedral_angles(nullptr),
    valences(nullptr)
{
    mesh.request_face_normals();
    mesh.request_vertex_normals();
    mesh.request_vertex_colors();

    if( OpenMesh::IO::read_mesh(mesh, filename) ){
        _nb_faces = mesh.n_faces();
        _nb_vertices = mesh.n_vertices();

        dihedral_angles = new float[mesh.n_edges()];
        valences = new uint[_nb_vertices];

        normalize();
        update_valence_vertices();
        update_dihedral_angles();

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

    if( dihedral_angles != nullptr ){
        delete [] dihedral_angles;
        dihedral_angles = nullptr;
    }

    if( valences != nullptr ){
        delete [] valences;
        valences = nullptr;
    }
}

void
MeshObject::normalize()
{
    float min_x, min_y, min_z;
    float max_x, max_y, max_z;
    float scale;

    min_x = min_y = min_z = std::numeric_limits<float>::max();
    max_x = max_y = max_z = std::numeric_limits<float>::min();

    MyMesh::Point p;
    for(const auto& v_it: mesh.vertices()){
        p = mesh.point(v_it);

        min_x = std::min(min_x, p[0]);
        min_y = std::min(min_y, p[1]);
        min_z = std::min(min_z, p[2]);

        max_x = std::max(max_x, p[0]);
        max_y = std::max(max_y, p[1]);
        max_z = std::max(max_z, p[2]);
    }

    MyMesh::Point pos( min_x+max_x, min_y+max_y, min_z+max_z);
    pos *= 0.5f;

    MyMesh::Point size = MyMesh::Point(0.0f, 0.0f, 0.0f);
    MyMesh::Point bboxmin(min_x, min_y, min_z);
    MyMesh::Point bboxmax(max_x, max_y, max_z);

    for(size_t i=0; i < 3; ++i)
        size[i] = std::fabs(bboxmax[i] - bboxmin[i]);

    if( size[0] > size[1] ){
        if( size[0] > size[2] )
            scale = 1.0f / size[0];
        else
            scale = 1.0f / size[2];
    }
    else {
        if( size[1] > size[2] )
            scale = 1.0f / size[1];
        else
            scale = 1.0f / size[2];
    }

    for(auto& v_it: mesh.vertices()){
        mesh.point(v_it) = (mesh.point(v_it) - pos)*scale;
    }
}

void
MeshObject::update_valence_vertices()
{   
    if( valences != nullptr ){
        size_t i = 0;
        for(const auto& v_it: mesh.vertices()){
            valences[i++] = mesh.valence(v_it);
        }
    }
}

void
MeshObject::update_dihedral_angles()
{
    if( dihedral_angles != nullptr ){
        size_t i = 0;
        for(const auto& e_it: mesh.edges()){
            float angle = mesh.calc_dihedral_angle(e_it);
            dihedral_angles[i++] = std::sqrt(angle * angle);
        }
    }
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

float
MeshObject::mean_dihedral() const
{
    float mean = 0.0f;
    if( valences != nullptr ){
        float nb_edges = mesh.n_edges();
        if( nb_edges > 0 ){
            for(size_t i=0; i < nb_edges; ++i)
                mean += dihedral_angles[i];

            mean /= float(nb_edges);
        }
    }
    return mean;
}

float
MeshObject::mean_valence() const
{
    float mean = 0.0f;
    if( dihedral_angles != nullptr ){
        if( _nb_vertices > 0 ){
            for(size_t i=0; i < _nb_vertices; ++i)
                mean += float(valences[i]);

            mean /= float(_nb_vertices);
        }
    }

    return mean;
}

bool
MeshObject::build(QOpenGLShaderProgram* program)
{
    size_t nb_vertices = mesh.n_vertices();
    size_t nb_elements = mesh.n_faces()*3;

    GLuint* indices = new GLuint[nb_elements];
    GLfloat* positions = new GLfloat[nb_vertices*3];
    GLfloat* v_normals = new GLfloat[nb_vertices*3];
    GLfloat* f_normals = new GLfloat[nb_vertices*3];
    GLfloat* colors = new GLfloat[nb_vertices*3];

    MyMesh::Normal v_normal;
    MyMesh::Normal f_normal;
    MyMesh::Point point;
    MyMesh::Color color;
    MyMesh::ConstFaceVertexIter cfv_it;

    size_t i = 0;
    size_t j = 0;

    for(const auto& cv_it: mesh.vertices()){
        /* const vertex iterator */
        v_normal = mesh.normal(cv_it);
        point = mesh.point(cv_it);
        color = mesh.color(cv_it);
        f_normal = mesh.normal(*mesh.cvf_iter(cv_it));

        for(j=0; j < 3; ++j, ++i){
            v_normals[i] = v_normal[j];
            f_normals[i] = f_normal[j];

            positions[i] = point[j];
            colors[i] = color[j];
        }
    }

    i = 0;
    for(const auto& cf_it: mesh.faces()){
        /* const face iterator */
        cfv_it = mesh.cfv_iter(cf_it);
        for(j=0; j < 3; ++j, ++i, ++cfv_it){
            indices[i] = GLuint(cfv_it->idx());
        }
    }

    set_vertices_geometry(program->attributeLocation("position"), positions, indices);
    set_vertices_colors(program->attributeLocation("color"), colors);
    set_vertices_normals(program->attributeLocation("v_normal"), v_normals);
    set_faces_normals(program->attributeLocation("f_normal"), f_normals);

    return initialize(nb_vertices, nb_elements, 3);
}

// https://fr.wikipedia.org/wiki/Distance_entre_deux_points_sur_le_plan_cart%C3%A9sien
float
MeshObject::distance(MyMesh::Point p1, MyMesh::Point p2)
{
    return std::sqrt(
        std::pow(p2[0]-p1[0], 2.0f)+
        std::pow(p2[1]-p1[1], 2.0f)+
        std::pow(p2[2]-p1[2], 2.0f));
}

void
MeshObject::Laplace_Beltrami_operator(float h, float lambda)
{
    MyMesh::ConstVertexVertexCCWIter cvv_ccwit;
    MyMesh::Point vi_left, vi_right, vi, v;

    float area;
    float alpha;
    float beta;

    float sum_cot;
    size_t i = 0;
    MyMesh::Point* deltas = new MyMesh::Point[mesh.n_vertices()];

    // For all vertices
    for(auto& v_it: mesh.vertices()){
        /* Const Vertex Vertex Counter-Clock Wise Iterator */
        cvv_ccwit = mesh.cvv_ccwiter(v_it);
        v = mesh.point(v_it);

        area = 0.0f;
        deltas[i] = MyMesh::Point(0.0f, 0.0f, 0.0f);

        // First ring neighbors
        while( cvv_ccwit != mesh.cvv_ccwend(v_it) ){
            vi_left = mesh.point(*cvv_ccwit);
            vi = mesh.point(*(++cvv_ccwit));
            vi_right = mesh.point(*(++cvv_ccwit));

            MyMesh::Point v0_alpha = (vi_left - vi);
            MyMesh::Point v1_alpha = (vi_left - v);

            MyMesh::Point v0_beta = (vi_right - vi);
            MyMesh::Point v1_beta = (vi_right - v);

            MyMesh::Point vi_minus_v = (vi - v);

            // compute Alpha & Beta angles
            // angle = acos( dot_product / product of norms )
            alpha = std::acos(
                  ( v0_alpha|v1_alpha )
                / ( v0_alpha.norm()*v1_alpha.norm() )
            );

            beta = std::acos(
                  ( v0_beta|v1_beta )
                / ( v0_beta.norm()*v1_beta.norm() )
            );

            // Cotangente
            // https://fr.wikipedia.org/wiki/Cotangente
            float cot_alpha = std::cos(alpha)/std::sin(alpha);
            float cot_beta = std::cos(beta)/std::sin(beta);

            // Sum part
            sum_cot = (cot_alpha + cot_beta);
            if( std::isnan( sum_cot ) )
                sum_cot = 1.0f;

            deltas[i] += (sum_cot * vi_minus_v);

            // Compute area
            // https://fr.wikipedia.org/wiki/Aire_d%27un_triangle
            MyMesh::Point barycenter_l = ((vi_left + vi + v) / 3.0f);
            MyMesh::Point barycenter_r = ((vi_right + vi + v) / 3.0f);

            float a = distance(v, barycenter_l);
            float b = distance(v, barycenter_r);
            float c = distance(barycenter_l, barycenter_r);
            float p = (a+b+c)/2.0f;

            area += std::sqrt(p*(p-a)*(p-b)*(p-c));

            // Since we `++` two times, we want to fallback once for the next iteration.
            --cvv_ccwit;
        }

        // Final compute, where deltas[i] holds all the sum part
        deltas[i] = 0.5f*area * deltas[i]; // 1/2 * Area * [Sum]
        ++i;
    }

    // Applies deltas to mesh vertices
    i = 0;
    for(auto& v: mesh.vertices())
        mesh.point(v) += (h * lambda * deltas[i++]);

    delete [] deltas;
    deltas = nullptr;
}

void
MeshObject::update_normals()
{
    mesh.update_face_normals();
    mesh.update_vertex_normals();
}
