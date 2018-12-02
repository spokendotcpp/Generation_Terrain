#include "../include/icosphere.h"
#include <iostream>

// http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html

IcoSphere::IcoSphere(size_t _iterations)
    :iterations(_iterations)
{}

IcoSphere::~IcoSphere()
{}

// https://fr.wikipedia.org/wiki/Icosa%C3%A8dre#Construction_par_les_coordonn%C3%A9es
bool
IcoSphere::build(QOpenGLShaderProgram* program)
{
    size_t nb_vertices = 12;
    size_t nb_elements = 20;

    MyMesh mesh;

    // gold number
    float t = (1.0f + std::sqrt(5.0f)) / 2.0f;

    // setup 12 init vertices of our Icosahedron:
    mesh.add_vertex(MyMesh::Point(-1.0f, +t, 0.0f).normalize());
    mesh.add_vertex(MyMesh::Point(+1.0f, +t, 0.0f).normalize());
    mesh.add_vertex(MyMesh::Point(-1.0f, -t, 0.0f).normalize());
    mesh.add_vertex(MyMesh::Point(+1.0f, -t, 0.0f).normalize());

    mesh.add_vertex(MyMesh::Point(0.0f, -1.0f, +t).normalize());
    mesh.add_vertex(MyMesh::Point(0.0f, +1.0f, +t).normalize());
    mesh.add_vertex(MyMesh::Point(0.0f, -1.0f, -t).normalize());
    mesh.add_vertex(MyMesh::Point(0.0f, +1.0f, -t).normalize());

    mesh.add_vertex(MyMesh::Point(+t, 0.0f, -1.0f).normalize());
    mesh.add_vertex(MyMesh::Point(+t, 0.0f, +1.0f).normalize());
    mesh.add_vertex(MyMesh::Point(-t, 0.0f, -1.0f).normalize());
    mesh.add_vertex(MyMesh::Point(-t, 0.0f, +1.0f).normalize());

    std::deque<MyMesh::VertexHandle> vh(12);
    for(size_t i=0; i < 12; ++i)
        vh[i] = MyMesh::VertexHandle(int(i));

    // setup 20 init faces of our Icosahedron:
    mesh.add_face(vh[0],  vh[11], vh[5]);
    mesh.add_face(vh[0],  vh[5],  vh[1]);
    mesh.add_face(vh[0],  vh[1],  vh[7]);
    mesh.add_face(vh[0],  vh[7],  vh[10]);
    mesh.add_face(vh[0],  vh[10], vh[11]);

    mesh.add_face(vh[1],  vh[5],  vh[9]);
    mesh.add_face(vh[5],  vh[11], vh[4]);
    mesh.add_face(vh[11], vh[10], vh[2]);
    mesh.add_face(vh[10], vh[7],  vh[6]);
    mesh.add_face(vh[7],  vh[1],  vh[8]);

    mesh.add_face(vh[3],  vh[9],  vh[4]);
    mesh.add_face(vh[3],  vh[4],  vh[2]);
    mesh.add_face(vh[3],  vh[2],  vh[6]);
    mesh.add_face(vh[3],  vh[6],  vh[8]);
    mesh.add_face(vh[3],  vh[8],  vh[9]);

    mesh.add_face(vh[4],  vh[9],  vh[5]);
    mesh.add_face(vh[2],  vh[4],  vh[11]);
    mesh.add_face(vh[6],  vh[2],  vh[10]);
    mesh.add_face(vh[8],  vh[6],  vh[7]);
    mesh.add_face(vh[9],  vh[8],  vh[1]);

    // ______

    MyMesh tmp_mesh;
    MyMesh::Point point;
    MyMesh::Normal normal;
    MyMesh::ConstFaceVertexIter cfv_it;
    bool found;

    std::deque<MyMesh::Point> points(6);

    vh.clear();
    vh.resize(6);

    // Subdivision(s):
    for(size_t i=0; i < iterations; ++i){

        // For each triangle of our current mesh:
        for(const auto& cf_it: mesh.faces()){
            cfv_it = mesh.cfv_iter(cf_it);

            // 3 vertices which compose the current triangle
            // + 3 new computed vertices
            points[0] = mesh.point(*cfv_it);
            points[1] = mesh.point(*(++cfv_it));
            points[2] = mesh.point(*(++cfv_it));
            points[3] = ((points[0] + points[1]) * 0.5f).normalize();
            points[4] = ((points[1] + points[2]) * 0.5f).normalize();
            points[5] = ((points[2] + points[0]) * 0.5f).normalize();

            // Find out if we already have computed vertices:
            for(size_t i=0; i < 6; ++i){
                found = false;

                for(const auto& cv_it: tmp_mesh.vertices()){
                    point = tmp_mesh.point(cv_it);

                    // if yes, save VertexHandle
                    if( points[i] == point ){
                        vh[i] = cv_it;
                        found = true;
                        break;
                    }
                }

                // if not, add if to the mesh AND get the VertexHandle
                if( !found )
                    vh[i] = tmp_mesh.add_vertex(points[i]);
            }

            // Create 4 new faces
            tmp_mesh.add_face(vh[0], vh[3], vh[5]);
            tmp_mesh.add_face(vh[1], vh[4], vh[3]);
            tmp_mesh.add_face(vh[2], vh[5], vh[4]);
            tmp_mesh.add_face(vh[3], vh[4], vh[5]);
        }

        // Swap memory:
        mesh.clean();
        mesh = tmp_mesh;
        tmp_mesh.clean();
    }

    // Don't forget to clean up:
    tmp_mesh.clear();
    vh.clear();
    points.clear();

    // _____
    mesh.request_face_normals();
    mesh.request_vertex_normals();

    mesh.update_face_normals();
    mesh.update_vertex_normals();

    nb_vertices = mesh.n_vertices();
    nb_elements = mesh.n_faces() * 3;

    GLfloat* positions = new GLfloat[nb_vertices*3];
    GLfloat* v_normals = new GLfloat[nb_vertices*3];
    GLuint* indices = new GLuint[nb_elements];

    size_t i = 0;
    for(const auto& cf_it: mesh.faces()){
        cfv_it = mesh.cfv_begin(cf_it);
        while( cfv_it != mesh.cfv_end(cf_it) ){
            indices[i++] = GLuint(cfv_it->idx());
            ++cfv_it;
        }
    }

    i=0;
    for(const auto& cv_it: mesh.vertices()){
        point = mesh.point(cv_it);
        normal = mesh.normal(cv_it);
        for(size_t j=0; j < 3; ++j, ++i){
            positions[i] = point[j];
            v_normals[i] = normal[j];
        }
    }

    mesh.release_vertex_normals();
    mesh.release_face_normals();

    set_vertices_geometry(program->attributeLocation("position"), positions, indices);
    set_vertices_colors(program->attributeLocation("color"), new GLfloat[nb_vertices*3]);
    set_vertices_normals(program->attributeLocation("v_normal"), v_normals);

    return initialize(nb_vertices, nb_elements, 3);
}
