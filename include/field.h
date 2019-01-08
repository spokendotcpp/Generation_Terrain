#ifndef FIELD_H
#define FIELD_H

#include <random>
#include "drawableobject.h"

class Field: public DrawableObject {
// Attributes
private:
    size_t length;
    int** map;

// Public methods
public:
    Field(size_t length);
    ~Field() override;

    void diamond_square();
    bool build(QOpenGLShaderProgram* program) override;
};

#endif // FIELD_H
