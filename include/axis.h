#ifndef AXIS_H
#define AXIS_H

#include "buffer.h"

class Axis: public Buffer
{
private:
    float x, y, z;
    float length;
    float* segments;

public:
    Axis(float=0.0f, float=0.0f, float=0.0f, float=5.0f);
    ~Axis() override;
    void show() const override;
};

#endif // AXIS_H
