#include "../include/cube.h"

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>



Cube::Cube()
    :DrawableObject()
{}

bool
Cube::build(QOpenGLShaderProgram* program)
{
    size_t nb_vertices = 8;
    size_t nb_elements = 6*2*3;

    GLfloat* positions = new GLfloat[nb_vertices*3] {
        -1.0f, +1.0f, +1.0f, // 0
        -1.0f, -1.0f, +1.0f, // 1
        +1.0f, +1.0f, +1.0f, // 2
        +1.0f, -1.0f, +1.0f, // 3

        -1.0f, +1.0f, -1.0f, // 4
        -1.0f, -1.0f, -1.0f, // 5
        +1.0f, +1.0f, -1.0f, // 6
        +1.0f, -1.0f, -1.0f, // 7
    };

    // 6 square faces, each subdivided as 2 triangles = 12 triangles
    GLuint* indices = new GLuint[nb_elements] {
        0, 1, 3,
        0, 3, 2,
        0, 2, 6,
        0, 6, 4,
        0, 4, 5,
        0, 5, 1,
        3, 7, 6,
        3, 6, 2,
        3, 1, 5,
        3, 5, 7,
        6, 7, 5,
        6, 5, 4,
    };

    MyMesh cube;
    for(size_t i=0; i < nb_vertices*3; i+=3){
        cube.add_vertex(MyMesh::Point(positions+i));
    }

    for(size_t i=0; i < nb_elements; i+=3){
        cube.add_face(
            MyMesh::VertexHandle(int(indices[i])),
            MyMesh::VertexHandle(int(indices[i+1])),
            MyMesh::VertexHandle(int(indices[i+2])));
    }

    cube.request_face_normals();
    cube.request_vertex_normals();

    cube.update_face_normals();
    cube.update_vertex_normals();

    GLfloat* normals = new GLfloat[nb_vertices*3];
    size_t i = 0;
    for(const auto& v: cube.vertices()){
        MyMesh::Normal n = cube.normal(v);
        for(size_t j=0; j < 3; ++j, ++i){
            normals[i] = n[j];
        }
    }

    cube.release_face_normals();
    cube.release_vertex_normals();

    set_vertices_geometry(program->attributeLocation("position"), positions, indices);
    set_vertices_colors(program->attributeLocation("color"), new GLfloat[nb_vertices*3]);
    set_vertices_normals(program->attributeLocation("normal"), normals);

    return initialize(nb_vertices, nb_elements, 3);
}


