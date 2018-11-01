#include "../include/meshobject.h"

#include <iostream>

MeshObject::MeshObject(const std::string& filename)
    :DrawableObject()
{
    MyMesh mesh;

    OpenMesh::IO::Options opt;
    OpenMesh::IO::read_mesh(mesh, filename, opt);

    // If the file did not provide vertex normals, then calculate them
    if ( !opt.check( OpenMesh::IO::Options::VertexNormal ) )
    {

        std::cerr << "Aie" << std::endl;

      // we need face normals to update the vertex normals
      mesh.request_face_normals();
      mesh.request_vertex_normals();
      // let the mesh update the normals
      mesh.update_normals();
      mesh.update_vertex_normals();

      // dispose the face normals, as we don't need them anymore
      mesh.release_face_normals();
    }


    /* LOAD EVERYTHING NEEDED INTO THE MESH */
    mesh.request_vertex_colors();
    mesh.request_vertex_normals();

    if( !mesh.has_vertex_colors() ){
        std::cerr << "Failed to request vertex colors" << std::endl;
    }

    if( !mesh.has_vertex_normals() ){
        std::cerr << "Failed to request vertex normals" << std::endl;
    }
    /* ------------------------------------ */

    /*
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
    MyMesh::Normal n;

    raw_vertices = new GLfloat[nb_vertices];
    raw_colors = new GLfloat[nb_vertices];
    raw_normals = new GLfloat[nb_vertices];

    for( const auto& cv_It: mesh.vertices() ){
        n = mesh.normal(cv_It);
        p = mesh.point(cv_It);
        c = mesh.color(cv_It);
        for(j=0; j < 3; ++j, ++i){
            raw_vertices[i] = p[j];
            raw_colors[i] = c[j]/255.0f;
            //raw_normals[i] = p[j];
        }
    }*/

    mesh.release_vertex_colors();
    mesh.release_vertex_normals();
}
