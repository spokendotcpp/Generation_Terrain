#include "../include/arcball.h"
#include <cmath>
#include <QtMath>

ArcBall::ArcBall(int w, int h):
    window_w(w), window_h(h),
    last_x(0), last_y(0),
    curr_x(0), curr_y(0)
{}

ArcBall::~ArcBall()
{}

QVector3D
ArcBall::get_screen_coord(int x, int y)
{
    QVector3D coord = QVector3D(
        +(2*x - window_w) / window_w,
        -(2*y - window_h) / window_h,
        0.0f
    );

    // clamp values
    if( coord.x() < -1.0f )
        coord.setX(-1.0f);
    else
    if( coord.x() > 1.0f )
        coord.setX(1.0f);

    if( coord.y() < -1.0f )
        coord.setY(-1.0f);
    else
    if( coord.y() > 1.0f )
        coord.setY(1.0f);

    float squared_len = coord.x() * coord.x() + coord.y() * coord.y();
    if( squared_len <= 1.0f )
        coord.setZ(1.0f - squared_len);
    else
        coord.normalize();

    return coord;
}

void
ArcBall::update_window_size(int w, int h)
{
    window_w = w;
    window_h = h;
}

void
ArcBall::update_window_coord(int x, int y)
{
    curr_x = x;
    curr_y = y;
}

QMatrix4x4
ArcBall::get_rotation_matrix(const QMatrix4x4& view)
{
    QMatrix4x4 rotation;
    rotation.setToIdentity();

    QVector3D v0 = get_screen_coord(last_y, last_y); // last position
    QVector3D v1 = get_screen_coord(curr_x, curr_y); // current position

    float angle = std::acos(std::min(1.0f, QVector3D::dotProduct(v0, v1)));

    QVector3D axis = QVector3D::crossProduct(v0, v1);
    rotation.rotate(qRadiansToDegrees(angle) * 1.0f, axis);

    last_x = curr_x;
    last_y = curr_y;

    return rotation;
}


