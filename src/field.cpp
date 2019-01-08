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
                );

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

    for(size_t i=0; i < length; ++i){
        for(size_t j=0; j < length; ++j){
            std::cerr << int(map[i][j]) << ", ";
        }
        std::cerr << std::endl;
    }

    return true;
}
