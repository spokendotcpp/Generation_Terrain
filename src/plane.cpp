#include "../include/plane.h"
#include <iostream>

Plane::Plane()
    :DrawableObject()
{}

bool
Plane::build(QOpenGLShaderProgram* program)
{
    size_t nb_vertices = 4;
    size_t nb_elements = 6;

    GLfloat* positions = new GLfloat[nb_vertices*3] {
        -1.0f, +1.0f, 0.0f, // 0
        -1.0f, -1.0f, 0.0f, // 1
        +1.0f, -1.0f, 0.0f, // 2
        +1.0f, +1.0f, 0.0f  // 3
    };

    GLuint* indices = new GLuint[nb_elements] {
        0, 1, 2,  2, 3, 0
    };

    GLfloat* colors = new GLfloat[nb_vertices*3];

    set_vertices_geometry(program->attributeLocation("position"), positions, indices);
    set_vertices_colors(program->attributeLocation("color"), colors);

    return initialize(nb_vertices, nb_elements, 3);
}
