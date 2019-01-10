#include "../include/field.h"
#include <iostream>

Field::Field(float _width, float _length, size_t power)
    :width(_width), length(_length), size(0), map(nullptr)
{
    if( power >= 16 )
        power = 8;  // default

    //    pow(2, power) + 1
    size = (1 << power) + 1;

    map = new float*[size];
    for(size_t i=0; i < size; ++i){
        map[i] = new float[size];
        for(size_t j=0; j < size; ++j)
            map[i][j] = 0;
    }
}

Field::~Field()
{
    if( map != nullptr ){
        for(size_t i=0; i < size; ++i){
            delete [] map[i];
            map[i] = nullptr;
        }
        delete [] map;
        map = nullptr;
    }
}

//template<typename T>
void median_filter(float** data, int width, int height, int window_width)
{
    if( window_width % 2 == 0 )
        window_width = 3;

    int x, y, k_idx;
    int window_size = window_width*window_width;
    int offset = window_width/2;

    std::cerr << offset << std::endl;

    float* window = new float[window_size];

    float** map = new float*[height];
    for(int i=0; i < height; ++i)
        map[i] = new float[width];

    for(int j=0; j < height; ++j){
        for(int i=0; i < width; ++i){

            k_idx = 0;

            for(int l=-offset; l < offset; ++l){
                y = j+l;
                if( y < 0 || y >= height )
                    y = j-l;

                for(int k=-offset; k < offset; ++k){
                    x = i+k;
                    if( x < 0 || x >= height )
                        x = i-k;

                    window[k_idx++] = data[y][x];
                }
            }

            // Bubble sort
            for(int m=0; m < window_size; ++m){
                for(int n=m+1; n < window_size; ++n){
                    if( window[m] > window[n] ){
                        float tmp = window[m];
                        window[m] = window[n];
                        window[n] = tmp;
                    }
                }
            }

            // Get median value
            map[j][i] = window[window_size/2];
        }
    }

    for(int j=0; j < height; ++j){
        for(int i=0; i < width; ++i){
            data[j][i] = map[j][i];
        }
        delete map[j];
        map[j] = nullptr;
    }
    delete [] map;
    map = nullptr;

    delete [] window;
    window = nullptr;
}

void
Field::diamond_square()
{
    if( map == nullptr )
        return;

    float yMax = 20.0f;
    float yMin = -20.0f;

    std::random_device random_device;
    std::mt19937 engine { random_device() };
    std::uniform_real_distribution<float> random_number(yMin, yMax);

    // Init first 4 corners
    size_t last = size-1;
    map[0][0]       = random_number(engine);
    map[0][last]    = random_number(engine);
    map[last][0]    = random_number(engine);
    map[last][last] = random_number(engine);

    size_t step = last;
    while( step > 1 ){
        size_t half = step/2;
        yMax /= 2.0f;
        yMin /= 2.0f;

        // Modify the random range (make it smaller)
        random_number.param(
            std::uniform_real_distribution<float>::param_type(yMin, yMax)
        );

        // Diamond step
        for(size_t j=half; j < size; j+=step){
            for(size_t i=half; i < size; i+=step){

                // Compute the current value of the map (j, i)
                // mean of neighbours + random value
                float mean = (
                    map[j-half][i-half] // top-left
                  + map[j-half][i+half] // top-right
                  + map[j+half][i-half] // bot-left
                  + map[j+half][i+half] // bot-right
                ) / 4;

                map[j][i] = mean + random_number(engine);
            }
        }

        // Square step
        size_t offset = 0;
        for(size_t j=0; j < size; j+=half){

            // Departure of our row
            if( offset == 0 )
                offset = half;
            else
                offset = 0;

            for(size_t i=offset; i < size; i+=step){
                float sum = 0;
                int n = 0;

                // edge test to ensure
                // we stay into the range of map[][].

                if( i >= half ){
                    sum += map[j][i-half];
                    ++n;
                }

                if( i + half < size ){
                    sum += map[j][i+half];
                    ++n;
                }

                if( j >= half ){
                    sum += map[j-half][i];
                    ++n;
                }

                if( j + half < size ){
                    sum += map[j+half][i];
                    ++n;
                }

                if( n == 0 ) n = 1;
                map[j][i] = (sum/n) + random_number(engine);
            }
        }

        step = half;
    }

    median_filter(map, int(size), int(size), 5);
}

bool
Field::build(QOpenGLShaderProgram* program)
{
    diamond_square();

    MyMesh mesh;
    mesh.request_face_normals();
    mesh.request_vertex_normals();

    size_t nb_vertices = size * size;
    GLfloat* positions = new GLfloat[nb_vertices*3];

    // Square mesh = (n-1)² squares
    // Triangle mesh = (n-1)² * 2
    // One triangle has 3 indices
    // R = (n-1)² * 2 * 3
    size_t nb_indices = (((size-1) * (size-1)) * 2) * 3;
    size_t it = 0;
    GLuint* indices = new GLuint[nb_indices];

    float init_x = -(width/2.0f);
    float init_z = -(length/2.0f);

    float step_x = width/size;
    float step_z = length/size;

    float x = init_x;
    float z = init_z;

    for(size_t j=0; j < size; ++j){
        for(size_t i=0; i < size; ++i){
            size_t idx = (j*size + i);
            positions[(idx*3)+0] = x;
            positions[(idx*3)+1] = map[j][i];
            positions[(idx*3)+2] = z;
            x += step_x;

            mesh.add_vertex(
                MyMesh::Point(
                    positions[(idx*3)+0],
                    positions[(idx*3)+1],
                    positions[(idx*3)+2])
            );

            if( j < size-1 && i < size-1 ){
                indices[it++] = GLuint(idx);
                indices[it++] = GLuint(idx+size);
                indices[it++] = GLuint(idx+1);

                indices[it++] = GLuint(idx+size);
                indices[it++] = GLuint(idx+size+1);
                indices[it++] = GLuint(idx+1);
            }
        }
        z += step_z;
        x = init_x;
    }

    // Add faces to OpenMesh struct so we can compute faces normals
    // (and then vertices normals).
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

    // Tells OpenMesh to release memory
    mesh.release_vertex_normals();
    mesh.release_face_normals();

    set_vertices_geometry(program->attributeLocation("position"), positions, indices);
    set_vertices_colors(program->attributeLocation("color"), new GLfloat[nb_vertices*3]);
    set_vertices_normals(program->attributeLocation("normal"), normals);

    return initialize(nb_vertices, nb_indices, 3);
}
