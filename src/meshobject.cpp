#include "../include/meshobject.h"

MeshObject::MeshObject(const std::string& filename)
    :DrawableObject()
{
    MyMesh* mesh = new MyMesh();
    OpenMesh::IO::read_mesh(mesh, filename);
    /* LOAD EVERYTHING NEEDED INTO THE MESH */
    mesh->request_vertex_colors();
    /* ------------------------------------ */

    nb_vertices = mesh->n_vertices() * 3;
    nb_indices = mesh->n_faces() * 3;

    // COPY INDICES FROM MESH STRUCT TO OURS
    raw_indices = new GLuint[nb_indices];
    MyMesh::ConstFaceVertexIter cfv_It;

    size_t i = 0;
    size_t j = 0;
    for( const auto& cf_It: mesh->faces() ){
        cfv_It = mesh->cfv_iter(cf_It);
        for(j=0; j < 3; ++j, ++i, ++cfv_It){
            raw_indices[i] = static_cast<GLuint>(cfv_It->idx());
        }
    }

    // COPY VERTICES COORDINATES FROM MESH STRUCT TO OURS
    i = 0;
    MyMesh::Point p;

    raw_vertices = new GLfloat[nb_vertices];
    for( const auto& cv_It: mesh->vertices() ){
        p = mesh->point(cv_It);
        for(j=0; j < 3; ++j, ++i){
            raw_vertices[i] = p[j];
        }
    }

    // COPY COLORS FROM MESH STRUCT TO OURS
    i = 0;
    MyMesh::Color c;

    raw_colors = new GLfloat[nb_vertices];
    for( const auto& cv_It: mesh->vertices() ){
        c = mesh->color(cv_It);
        for(j=0; j < 3; ++j, ++i){
            raw_colors[i] = c[j];
        }
    }
}

MeshObject::~MeshObject()
{

}

void MeshObject::init(QOpenGLShaderProgram* program)
{

}
