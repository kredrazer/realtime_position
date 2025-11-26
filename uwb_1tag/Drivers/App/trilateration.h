#ifndef TRILATERATION_H
#define TRILATERATION_H

#include <math.h>

typedef struct {
    float x;
    float y;
} vec2d_t;

// Hàm tính tọa độ từ 3 anchor và 3 khoảng cách
// Trả về 1 nếu thành công, 0 nếu lỗi (chia cho 0)
int trilateration_2d(vec2d_t p1, float r1,
                     vec2d_t p2, float r2,
                     vec2d_t p3, float r3,
                     vec2d_t *result);

#endif