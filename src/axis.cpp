#include "../include/axis.h"

Axis::Axis(float _x, float _y, float _z, float _length)
    :x(_x), y(_y), z(_z), length(_length)
{
    float half = length/2.0f;
    segments = new float[18];

    for(int i=0; i < 18; i+=3){
        segments[i] = x;
        segments[i+1] = y;
        segments[i+2] = z;
    }

    for(int i=0; i < 18; i+=7){
        segments[i] -= half;
        segments[i+3] += half;
    }
}

Axis::~Axis()
{
    if( segments != nullptr ){
        delete [] segments;
        segments = nullptr;
    }
}

void Axis::show() const
{

}
