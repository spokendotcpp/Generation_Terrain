#include "../include/axis.h"
#include <iostream>

Axis::Axis(float _x, float _y, float _z, float _length)
    :DrawableObject(),
     x(_x), y(_y), z(_z),
     length(_length)
{}

void Axis::init(QOpenGLShaderProgram* program)
{
    float half = length/2.0f;

    unsigned long nb_vertices = 18;
    unsigned long nb_indices = 6;

    GLfloat* raw_vertices = new GLfloat[nb_vertices] {
        x-half, y, z,
        x+half, y, z,
        x, y-half, z,
        x, y+half, z,
        x, y, z-half,
        x, y, z+half
    };

    GLfloat* raw_colors = new GLfloat[nb_vertices] {
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
    };

    GLuint* raw_indices = new GLuint[nb_indices] {
        0, 1,
        2, 3,
        4, 5,
    };

    vao->bind();
    {
        ebo->bind();
        ebo->setUsagePattern(QOpenGLBuffer::StaticDraw);
        ebo->allocate(raw_indices, static_cast<int>(sizeof(GLuint)*nb_indices));

        // bytes size of vertices into memory :
        int vertices_bytes = static_cast<int>(sizeof(GLfloat) * nb_vertices);

        vbo->bind();
        vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
        // how many bytes do you give to the GPU ?
        /*
         * VBO[
         *      Vertices[v0.x, v0.y, v0.z ... vn-1.x, vn-1.y, vn-1.z], <-- one vertex * 3 coordinates * sizeof(GLfloat) == vertices_bytes
         *      Colors[c0.r, c0.g, c0.b ... cn-1.r, cn-1.g, cn-1.b],
         *      Normals?,
         *      Texture coordinates?,
         *      ...
         * ]
         *
         */
        vbo->allocate(vertices_bytes * 2);
        vbo->write(0, raw_vertices, vertices_bytes);
        vbo->write(vertices_bytes, raw_colors, vertices_bytes);

        // Where does the attribute position into our vertex shader has to look into the VBO ?
        program->enableAttributeArray("position");
        program->setAttributeBuffer("position", GL_FLOAT, 0, 3, 0);

        // attribute inColor will find information (colors) into VBO after an offset of `vertices_bytes`.
        program->enableAttributeArray("inColor");
        program->setAttributeBuffer("inColor", GL_FLOAT, vertices_bytes, 3, 0);
    }
    vao->release();

    // No more needs of Buffers
    ebo->release();
    vbo->release();

    delete [] raw_vertices;
    delete [] raw_colors;
    delete [] raw_indices;

    this->free_buffers();

    this->drawable_elements(static_cast<GLsizei>(nb_vertices));
}
