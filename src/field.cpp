#include "../include/field.h"

Field::Field(size_t l)
    :length(l), map(nullptr)
{
    if( length%2 == 0 )
        ++length;

    map = new uchar* [length];
    for(size_t i=0; i < length; ++i){
        map[i] = new uchar[length];
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
    std::default_random_engine random_engine(random_device());
    std::uniform_int_distribution<uchar> corner{0, 255};

    map[0][0]               = corner(random_engine);
    map[0][length-1]        = corner(random_engine);
    map[length-1][0]        = corner(random_engine);
    map[length-1][length-1] = corner(random_engine);

    size_t i = length-1;

    while( i > 1 ){
        size_t half = i/2;

        for(size_t x=half; x < length; x+=i){
            for(size_t y=half; y < length; y+=i){

                uchar mean = (
                    map[x-half][y-half]
                  + map[x-half][y+half]
                  + map[x+half][y+half]
                  + map[x+half][y-half]
                )/4;

                map[x][y] = mean + corner(random_engine);
            }
        }

        size_t offset = 0;
        for(size_t x=0; x < length; x+=half){
            if( offset == 0 )
                offset = half;
            else
                offset = 0;

            for(size_t y=offset; y < length; y+=half){
                uchar mean = 0;
                size_t n = 0;

                if( x >= half ){
                    mean += map[x - half][y];
                    ++n;
                }

                if( x + half < length ){
                    mean += map[x + half][y];
                    ++n;
                }

                if( y >= half ){
                    mean += map[x][y-half];
                    ++n;
                }

                if( y + half < length ){
                    mean += map[x][y+half];
                    ++n;
                }

                map[x][y] = mean / (n + corner(random_engine));
            }
        }

        i = half;
    }
}

#include <iostream>

bool
Field::build(QOpenGLShaderProgram* program)
{
    diamond_square();

    for(size_t i=0; i < length; ++i){
        for(size_t j=0; j < length; ++j){
            std::cerr << int(map[i][j]) << ", ";
        }
        std::cerr << std::endl;
    }

    return true;
}
