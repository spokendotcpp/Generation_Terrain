#include "../include/meshobject.h"

#include <iostream>

MeshObject::MeshObject(const std::string& filename)
    :DrawableObject()
{
    MyMesh mesh;
    OpenMesh::IO::read_mesh(mesh, filename);
    /* LOAD EVERYTHING NEEDED INTO THE MESH */
    mesh.request_vertex_colors();

    if( !mesh.has_vertex_colors() ){
        std::cerr << "Failed to request vertex colors" << std::endl;
    }
    /* ------------------------------------ */

    nb_vertices = mesh.n_vertices() * 3;
    nb_indices = mesh.n_faces() * 3;

    // copy indices
    raw_indices = new GLuint[nb_indices];
    MyMesh::ConstFaceVertexIter cfv_It;

    size_t i = 0;
    size_t j = 0;
    for( const auto& cf_It: mesh.faces() ){
        cfv_It = mesh.cfv_iter(cf_It);
        for(j=0; j < 3; ++j, ++i, ++cfv_It){
            raw_indices[i] = static_cast<GLuint>(cfv_It->idx());
        }
    }

    // copy vertices & their colors
    i = 0;
    MyMesh::Point p;
    MyMesh::Color c;

    raw_vertices = new GLfloat[nb_vertices];
    raw_colors = new GLfloat[nb_vertices];

    for( const auto& cv_It: mesh.vertices() ){
        p = mesh.point(cv_It);
        c = mesh.color(cv_It);
        for(j=0; j < 3; ++j, ++i){
            raw_vertices[i] = p[j];
            raw_colors[i] = c[j];
        }
    }

    mesh.release_vertex_colors();
}

MeshObject::~MeshObject()
{}

void MeshObject::init(QOpenGLShaderProgram* program)
{   
    if( !program->isLinked() ){
        std::cerr << "Program shader not linked" << std::endl;
        return;
    }

    vao = new QOpenGLVertexArrayObject();
    if( !vao->create() ){
        std::cerr << "Failed to create VAO" << std::endl;
        return;
    }

    vao->bind();
    {
        int loc_position = program->attributeLocation("position");
        int loc_colors = program->attributeLocation("inColor");

        indices = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
        if( !indices->create() ){
            std::cerr << "Failed to create Index Buffer" << std::endl;
            return;
        }
        indices->bind();
        indices->setUsagePattern(QOpenGLBuffer::StaticDraw);
        indices->allocate(raw_indices, static_cast<int>(bytes_i()));


        vertices = new QOpenGLBuffer();
        if( !vertices->create() ){
            std::cerr << "Failed to create VBO for vertices" << std::endl;
            return;
        }
        vertices->bind();
        vertices->setUsagePattern(QOpenGLBuffer::StaticDraw);
        vertices->allocate(raw_vertices, static_cast<int>(bytes_v()));

        program->enableAttributeArray(loc_position); // TODO: move this part somewhere else (New Class)
        program->setAttributeArray(loc_position, GL_FLOAT, nullptr, 3);


        colors = new QOpenGLBuffer();
        if( !colors->create() ){
            std::cerr << "Failed to create VBO for colors" << std::endl;
            return;
        }
        colors->bind();
        colors->setUsagePattern(QOpenGLBuffer::StaticDraw);
        colors->allocate(raw_colors, static_cast<int>(bytes_v()));

        program->enableAttributeArray(loc_colors);
        program->setAttributeArray(loc_colors, GL_FLOAT, nullptr, 3);
    }
    vao->release();
    indices->release();
    vertices->release();
    colors->release();

    free_raw_memory();
}


void MeshObject::show(GLenum mode) const
{
    vao->bind();
    glDrawElements(mode, static_cast<GLsizei>(nb_indices), GL_UNSIGNED_INT, nullptr);
    vao->release();
}
