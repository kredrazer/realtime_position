#include "trilateration.h"

int trilateration_2d(vec2d_t p1, float r1,
                     vec2d_t p2, float r2,
                     vec2d_t p3, float r3,
                     vec2d_t *result)
{
    float A, B, C, D, E, F;

    // Thiết lập hệ phương trình tuyến tính Ax + By = C và Dx + Ey = F
    A = 2 * (p2.x - p1.x);
    B = 2 * (p2.y - p1.y);
    C = r1*r1 - r2*r2 - p1.x*p1.x + p2.x*p2.x - p1.y*p1.y + p2.y*p2.y;

    D = 2 * (p3.x - p1.x);
    E = 2 * (p3.y - p1.y);
    F = r1*r1 - r3*r3 - p1.x*p1.x + p3.x*p3.x - p1.y*p1.y + p3.y*p3.y;

    // Định thức (Determinant)
    float det = A * E - B * D;

    if (fabs(det) < 0.001) // Tránh chia cho 0 (3 anchor thẳng hàng)
    {
        return 0; 
    }

    result->x = (C * E - F * B) / det;
    result->y = (A * F - C * D) / det;

    return 1;
}