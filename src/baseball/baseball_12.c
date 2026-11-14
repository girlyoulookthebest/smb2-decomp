#include "baseball.h"

void fn_18_DD38(struct Ball *ball)
{
    Vec xaxis = {1.0f, 0.0f, 0.0f};
    Vec yaxis = {0.0f, 1.0f, 0.0f};
    Vec from;
    Vec to;
    s16 rotX;
    s16 rotY;

    from = balls[1].pos;
    to = ball->pos;
    ray_to_euler_xy(&from, &to, &rotX, &rotY);
    quat_from_axis_angle(&ball->ape->rot, &xaxis, rotX);
    quat_from_axis_angle(&ball->ape->rot, &yaxis, rotY - 0x8000);
}

