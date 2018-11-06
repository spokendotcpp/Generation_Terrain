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

}


void
MeshObject::show(GLenum mode) const
{

}
