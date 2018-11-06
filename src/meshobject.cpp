#include "../include/meshobject.h"

#include <iostream>

MeshObject::MeshObject(const std::string& filename)
    :DrawableObject()
{
    mesh.request_vertex_normals();
    mesh.request_face_normals();

    OpenMesh::IO::Options opt;
    opt.set( OpenMesh::IO::Options::VertexNormal );
    opt.set( OpenMesh::IO::Options::FaceNormal );

    OpenMesh::IO::read_mesh(mesh, filename, opt);

    if( !opt.check(OpenMesh::IO::Options::VertexNormal) )
        mesh.update_vertex_normals();

    if( !opt.check(OpenMesh::IO::Options::FaceNormal) )
        mesh.update_face_normals();
}

MeshObject::~MeshObject()
{
    mesh.release_face_colors();
    mesh.release_vertex_normals();
}

void
MeshObject::init(QOpenGLShaderProgram* program)
{
    size_t nb_vertices = mesh.n_vertices()*3;
    size_t nb_indices = mesh.n_faces()*3;

    GLuint* raw_indices = new GLuint[nb_indices];
    GLfloat* raw_vertices = new GLfloat[nb_vertices];
    GLfloat* raw_normals = new GLfloat[nb_vertices];
    GLfloat* raw_colors = new GLfloat[nb_vertices];

    MyMesh::Normal normal;
    MyMesh::Point point;
    MyMesh::ConstFaceVertexIter cfv_it;

    size_t i = 0;
    size_t j = 0;

    for(const auto& cv_it: mesh.vertices()){
        /* const vertex iterator */
        normal = mesh.normal(cv_it);
        point = mesh.point(cv_it);
        for(j=0; j < 3; ++j, ++i){
            raw_normals[i] = normal[j];
            raw_vertices[i] = point[j];
            raw_colors[i] = 0.5f;
        }
    }

    i = 0;
    for(const auto& cf_it: mesh.faces()){
        /* const face iterator */
        cfv_it = mesh.cfv_iter(cf_it);
        for(j=0; j < 3; ++j, ++i, ++cfv_it)
            raw_indices[i] = static_cast<GLuint>(cfv_it->idx());
    }

    vao->bind();
    {
        ebo->bind();
        ebo->setUsagePattern(QOpenGLBuffer::StaticDraw);
        ebo->allocate(raw_indices, static_cast<int>(sizeof(GLuint)*nb_indices));

        int vertices_bytes = static_cast<int>(sizeof(GLfloat)*nb_vertices);

        vbo->bind();
        vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
        vbo->allocate(vertices_bytes * 3);
        /* Offset                     |     data      |   data.length */
        vbo->write(0,                   raw_vertices,   vertices_bytes);
        vbo->write(vertices_bytes,      raw_normals,    vertices_bytes);
        vbo->write(2*vertices_bytes,    raw_colors,     vertices_bytes);

        program->enableAttributeArray("position");
        program->setAttributeBuffer("position", GL_FLOAT, 0, 3, 0);

        program->enableAttributeArray("inNormal");
        program->setAttributeBuffer("inNormal", GL_FLOAT, vertices_bytes, 3, 0);

        program->enableAttributeArray("inColor");
        program->setAttributeBuffer("inColor", GL_FLOAT, 2*vertices_bytes, 3, 0);
    }
    vao->release();
    ebo->release();
    vbo->release();

    this->free_buffers();

    delete [] raw_indices;
    delete [] raw_normals;
    delete [] raw_vertices;
    delete [] raw_colors;

    this->drawable_elements(static_cast<GLsizei>(nb_indices));
}
