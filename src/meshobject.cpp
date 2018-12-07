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

    // This property will be used in the function `find_gaps()`
    mesh.add_property(gap_id, "gap_id");
    mesh.property(gap_id).set_persistent(true);

    if( OpenMesh::IO::read_mesh(mesh, filename) ){
        _nb_faces = mesh.n_faces();
        _nb_vertices = mesh.n_vertices();

        dihedral_angles = new float[mesh.n_edges()];
        valences = new uint[_nb_vertices];
        laplace_beltrami_operators = new MyMesh::Point[_nb_vertices];

        normalize();
        update_normals();
        update_valence_vertices();
        update_dihedral_angles();
    }
    else {
        std::cerr << "Failed to read " << filename << " file" << std::endl;
    }
}

MeshObject::~MeshObject()
{
    mesh.release_vertex_normals();
    mesh.release_face_normals();

    if( dihedral_angles != nullptr ){
        delete [] dihedral_angles;
        dihedral_angles = nullptr;
    }

    if( valences != nullptr ){
        delete [] valences;
        valences = nullptr;
    }

    if( laplace_beltrami_operators != nullptr ){
        delete [] laplace_beltrami_operators;
        laplace_beltrami_operators = nullptr;
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

std::vector<std::vector<MyMesh::VertexHandle>>
MeshObject::find_gaps()
{
    std::vector<std::vector<MyMesh::VertexHandle>> gaps;

    // Init properties
    int id = -1;
    for(auto& he_it: mesh.halfedges())
        mesh.property(gap_id, he_it) = id;

    // For each Half-Edge, findout if it's boundary and not added into a previous hole.
    for(auto& he_it: mesh.halfedges()){
        if( mesh.is_boundary(he_it) && (mesh.property(gap_id, he_it) == -1) ){
            mesh.property(gap_id, he_it) = ++id;

            std::vector<MyMesh::VertexHandle> gap;
            gap.push_back(mesh.to_vertex_handle(he_it));

            // seek neighbors
            MyMesh::HalfedgeHandle he = mesh.next_halfedge_handle(he_it);
            while( he != he_it ){
                mesh.property(gap_id, he) = id;
                gap.push_back(mesh.to_vertex_handle(he));
                he = mesh.next_halfedge_handle(he);
            }

            gaps.push_back(gap);
        }
    }

    std::cerr << "Total number of gaps: " << gaps.size() << std::endl;

    /*
     * TODO PART *
    // Allocate Matrix of size gaps.size() x gaps.size()
    float** val = new float*[gaps[0].size()];
    for(size_t i=0; i < gaps[0].size(); ++i)
        val[i] = new float[gaps[0].size()];

    // Initialize
    for(size_t i=0; i < gaps[0].size(); ++i)
        for(size_t j=0; j < gaps[0].size(); ++j)
            val[i][j] = 0.0f;

    for(size_t i=0; i < gaps[0].size(); ++i){
        for(size_t j=0; j < gaps[0].size(); ++j){
            if( j - i == 2 ){
                val[i][j] =
                    // TODO
                    triangle_quality(
                        mesh.point(gaps[0][i]),
                        mesh.point(gaps[0][(i+1)%gaps[0].size()]),
                        mesh.point(gaps[0][(i+2)%gaps[0].size()])
                    );
            }
            std::cerr << val[i][j] << " , ";
        }

        std::cerr << std::endl;
    }

    // Free memory
    for(size_t i=0; i < gaps[0].size(); ++i){
        delete [] val[i];
        val[i] = nullptr;
    }

    delete [] val;
    val = nullptr;
    */

    return gaps;
}

/* Raycasting intersection with a triangle */
// http://iquilezles.org/www/articles/intersectors/intersectors.htm
OpenMesh::Vec3f
MeshObject::ray_intersect_triangle(
        OpenMesh::Vec3f origin, OpenMesh::Vec3f direction,
        OpenMesh::Vec3f p0, OpenMesh::Vec3f p1, OpenMesh::Vec3f p2)
{
    OpenMesh::Vec3f orp0 = origin - p0;
    OpenMesh::Vec3f p1p0 = p1 - p0;
    OpenMesh::Vec3f p2p0 = p2 - p0;

    OpenMesh::Vec3f n = OpenMesh::cross(p1p0, p2p0);
    OpenMesh::Vec3f q = OpenMesh::cross(orp0, direction);

    float d = 1.0f / OpenMesh::dot( direction, n );
    float u = d * OpenMesh::dot( -q, p2p0 );
    float v = d * OpenMesh::dot(  q, p1p0 );
    float t = d * OpenMesh::dot( -n, orp0 );

    t = std::min(u, std::min(v, std::min(1.0f-(u+v), t)));

    return OpenMesh::Vec3f(t, u, v);
}

/*
 * Test raycasting intersection with every faces of the current mesh
 * @ returns first face ID met (-1 if not found at all).
 */
int
MeshObject::get_face_picked(OpenMesh::Vec3f origin, OpenMesh::Vec3f direction)
{
    for(const auto& f: mesh.faces()){
        MyMesh::ConstFaceVertexIter cfv_it = mesh.cfv_iter(f);

        OpenMesh::Vec3f p0 = mesh.point(*cfv_it);
        OpenMesh::Vec3f p1 = mesh.point(*(++cfv_it));
        OpenMesh::Vec3f p2 = mesh.point(*(++cfv_it));

        OpenMesh::Vec3f inter = ray_intersect_triangle(origin, direction, p0, p1, p2);

        if( inter[0] >= 0.0f )
            return f.idx();
    }

    return -1;
}

std::vector<int>
MeshObject::get_vertices_id_from_face(int face) const
{
    std::vector<int> ids{ -1, -1, -1  };

    if( face >= 0 ){
        MyMesh::ConstFaceVertexIter cfv_iter = mesh.cfv_iter(MyMesh::FaceHandle(face));
        ids[0] = cfv_iter->idx();
        ids[1] = (++cfv_iter)->idx();
        ids[2] = (++cfv_iter)->idx();
    }

    return ids;
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

std::vector<float>
MeshObject::get_dihedral_angles() const
{
    std::vector<float> values;
    for(size_t i=0; i < mesh.n_edges(); ++i)
        values.push_back(dihedral_angles[i]);

    return values;
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
    GLfloat* colors = new GLfloat[nb_vertices*3];

    MyMesh::Normal v_normal;
    MyMesh::Point point;
    MyMesh::ConstFaceVertexIter cfv_it;

    size_t i = 0;
    size_t j = 0;
    for(const auto& cv_it: mesh.vertices()){
        /* const vertex iterator */
        v_normal = mesh.normal(cv_it);
        point = mesh.point(cv_it);
        for(j=0; j < 3; ++j, ++i){
            v_normals[i] = v_normal[j];
            positions[i] = point[j];
            colors[i] = 1.0f;
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

float
MeshObject::triangle_area(MyMesh::Point p1, MyMesh::Point p2, MyMesh::Point p3)
{
    float a = distance(p1, p2);
    float b = distance(p1, p3);
    float c = distance(p2, p3);
    float p = (a+b+c)/2.0f;

    return std::sqrt(p*(p-a)*(p-b)*(p-c));
}

float
MeshObject::triangle_quality(MyMesh::Point p1, MyMesh::Point p2, MyMesh::Point p3)
{
    return triangle_area(p1, p2, p3);
}

void
MeshObject::compute_Laplace_Beltrami_operators()
{
    MyMesh::ConstVertexVertexCCWIter cvv_ccwit;
    MyMesh::Point vi_left, vi_right, vi, v;

    float area;
    float alpha;
    float beta;

    float sum_cot;

    // For all vertices
    size_t i = 0;
    for(auto& v_it: mesh.vertices()){
        /* Const Vertex Vertex Counter-Clock Wise Iterator */
        cvv_ccwit = mesh.cvv_ccwiter(v_it);
        v = mesh.point(v_it);

        area = 0.0f;
        laplace_beltrami_operators[i] = MyMesh::Point(0.0f, 0.0f, 0.0f);

        // First ring neighbors
        while( cvv_ccwit != mesh.cvv_ccwend(v_it) ){
            vi_left = mesh.point(*cvv_ccwit);
            vi = mesh.point(*(++cvv_ccwit));
            vi_right = mesh.point(*(++cvv_ccwit));

            OpenMesh::Vec3f v0_alpha = (vi - vi_left);
            OpenMesh::Vec3f v1_alpha = (v - vi_left);

            OpenMesh::Vec3f v0_beta = (vi - vi_right);
            OpenMesh::Vec3f v1_beta = (v - vi_right);

            OpenMesh::Vec3f vi_minus_v = (vi - v);

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
            laplace_beltrami_operators[i] += (sum_cot * vi_minus_v);

            // Compute area
            // https://fr.wikipedia.org/wiki/Aire_d%27un_triangle
            MyMesh::Point barycenter_l = ((vi_left + vi + v) / 3.0f);
            MyMesh::Point barycenter_r = ((vi_right + vi + v) / 3.0f);

            area += triangle_area(barycenter_l, barycenter_r, v);

            // Since we `++` two times, we want to fallback once for the next iteration.
            --cvv_ccwit;
        }

        laplace_beltrami_operators[i++] /= (2.0f*area);
    }
}

void
MeshObject::apply_Laplace_Beltrami(float h, float lambda)
{
    compute_Laplace_Beltrami_operators();

    size_t i = 0;
    for(auto& v: mesh.vertices())
        mesh.point(v) += (h * lambda * laplace_beltrami_operators[i++]);
}

void
MeshObject::update_normals()
{
    mesh.update_face_normals();
    mesh.update_vertex_normals();
}
