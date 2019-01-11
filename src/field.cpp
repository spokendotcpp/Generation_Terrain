#include "../include/field.h"
#include <iostream>

Field::Field(size_t l)
    :length(l), map(nullptr)
{
    if( length%2 == 0 )
        ++length;

    map = new int* [length];
    for(size_t i=0; i < length; ++i){
        map[i] = new int[length];
        for(size_t j=0; j < length; ++j)
            map[i][j] = 0;
    }
}

Field::~Field()
{
    if( map != nullptr ){
        for(size_t i=0; i < length; ++i){
            delete [] map[i];
            map[i] = nullptr;
        }
        delete [] map;
        map = nullptr;
    }
}

void
Field::diamond_square()
{
    if( map == nullptr )
        return;

    std::random_device random_device;
    std::mt19937 engine { random_device() };
    std::uniform_int_distribution<int> random_number(-int(length), int(length));

    // Init first 4 corners
    map[0][0]               = random_number(engine);
    map[0][length-1]        = random_number(engine);
    map[length-1][0]        = random_number(engine);
    map[length-1][length-1] = random_number(engine);

    size_t step = length-1;
    while( step > 1 ){
        size_t half = step/2;

        // Modify the random range (make it smaller)
        random_number.param(
            std::uniform_int_distribution<>::param_type(-int(half), int(half))
        );

        // Diamond phase
        for(size_t j=half; j < length; j+=step){
            for(size_t i=half; i < length; i+=step){

                // Compute the current value of the map (j, i)
                // mean of neighbours + random value
                int mean = (
                    map[j-half][i-half] // top-left
                  + map[j-half][i+half] // top-right
                  + map[j+half][i-half] // bot-left
                  + map[j+half][i+half] // bot-right
                ) / 4;

                map[j][i] = mean + random_number(engine);
            }
        }

        // Square phase
        size_t offset = 0;
        for(size_t j=0; j < length; j+=half){

            // Departure of our row
            if( offset == 0 )
                offset = half;
            else
                offset = 0;

            for(size_t i=offset; i < length; i+=step){
                int mean = 0;
                int n = 0;

                // edge test to ensure
                // we stay into the range of map[][].

                if( i >= half ){
                    mean += map[j][i-half];
                    ++n;
                }

                if( i + half < length ){
                    mean += map[j][i+half];
                    ++n;
                }

                if( j >= half ){
                    mean += map[j-half][i];
                    ++n;
                }

                if( j + half < length ){
                    mean += map[j+half][i];
                    ++n;
                }

                map[j][i] = (mean/n) + random_number(engine);
            }
        }

        step = half;
    }
}

bool
Field::build(QOpenGLShaderProgram* program)
{
    diamond_square();

    MyMesh mesh;
    mesh.request_face_normals();
    mesh.request_vertex_normals();

    size_t nb_vertices = length * length;
    GLfloat* positions = new GLfloat[nb_vertices*3];
    GLfloat* colors = new GLfloat[nb_vertices*3];
    // Square mesh = (n-1)² squares
    // Triangle mesh = (n-1)² * 2
    // One triangle has 3 indices
    // R = (n-1)² * 2 * 3
    size_t nb_indices = (((length-1) * (length-1)) * 2) * 3;
    size_t it = 0;
    GLuint* indices = new GLuint[nb_indices];

    // TODO
    // let user choose size of field
    // nb of segments (current length)
    // ...
    float x = -(length*0.1);
    float z = x;

    for(size_t j=0; j < length; ++j){
        for(size_t i=0; i < length; ++i){
            size_t idx = (j*length + i);
            positions[(idx*3)+0] = x;
            positions[(idx*3)+1] = float(map[j][i]) * 0.05f; // TODO
            positions[(idx*3)+2] = z;
            x += 0.2f;

            mesh.add_vertex(
                MyMesh::Point(
                    positions[(idx*3)+0],
                    positions[(idx*3)+1],
                    positions[(idx*3)+2])
            );

            if( j < length-1 && i < length-1 ){
                indices[it++] = GLuint(idx);
                indices[it++] = GLuint(idx+length);
                indices[it++] = GLuint(idx+1);

                indices[it++] = GLuint(idx+length);
                indices[it++] = GLuint(idx+length+1);
                indices[it++] = GLuint(idx+1);
            }
        }
        z += 0.2;
        x = -(length*0.1f);
    }

    for(size_t i=0; i < nb_indices; i+=3){
        mesh.add_face(
            MyMesh::VertexHandle(int(indices[i+0])),
            MyMesh::VertexHandle(int(indices[i+1])),
            MyMesh::VertexHandle(int(indices[i+2]))
        );
    }

    // compute normals
    mesh.update_face_normals();
    mesh.update_vertex_normals();

    GLfloat* normals = new GLfloat[nb_vertices*3];
    size_t i = 0;
    for(const auto& v: mesh.vertices()){
        MyMesh::Normal n = mesh.normal(v);
        for(size_t j = 0; j < 3; ++j, ++i){
            normals[i] = n[j];
        }
    }

    for(size_t j=0; j < length; ++j)
    {
        for(size_t i=0; i < length; ++i)
        {
            size_t idx = (j*length + i);
            if(map[j][i] < -128)
            {
                colors[(idx*3)+0] = 0.0f;
                colors[(idx*3)+1] = 0.0f;
                colors[(idx*3)+2] = 0.0f;
            }
            else if(map[j][i] > 127)
            {
                colors[(idx*3)+0] = 1.0f;
                colors[(idx*3)+1] = 1.0f;
                colors[(idx*3)+2] = 1.0f;
            }
            else
            {
                colors[(idx*3)+0] = ((GLfloat)map[j][i]+128.0f)/255.0f;
                colors[(idx*3)+1] = ((GLfloat)map[j][i]+128.0f)/255.0f;
                colors[(idx*3)+2] = ((GLfloat)map[j][i]+128.0f)/255.0f;
            }
        }
    }

    mesh.release_vertex_normals();
    mesh.release_face_normals();

    set_vertices_geometry(program->attributeLocation("position"), positions, indices);
    set_vertices_colors(program->attributeLocation("color"), colors);//new GLfloat[nb_vertices*3]);
    set_vertices_normals(program->attributeLocation("normal"), normals);

    return initialize(nb_vertices, nb_indices, 3);
}
