#ifndef BUFFER_H
#define BUFFER_H

#include <QOpenGLBuffer>

class Buffer: public QOpenGLBuffer
{
public:
    Buffer();
    virtual ~Buffer();
    virtual void show() const =0;
};

#endif // BUFFER_H
