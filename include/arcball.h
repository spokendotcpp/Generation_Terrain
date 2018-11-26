#ifndef ARCBALL_H
#define ARCBALL_H

#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>

class ArcBall {
private:
    int window_w;
    int window_h;

    int last_x, last_y;
    int curr_x, curr_y;

public:
    ArcBall(int w, int h);
    ~ArcBall();

    void update_window_size(int w, int h);
    void update_window_coord(int x, int y);
    QMatrix4x4 get_rotation_matrix(const QMatrix4x4& view);

private:
    QVector3D get_screen_coord(int x, int y);
};

#endif // ARCBALL_H
