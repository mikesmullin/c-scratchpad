#pragma once

#include <stdint.h>
typedef int32_t s32;
typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

#define Math__min(a, b) (((a) < (b)) ? (a) : (b))
#define Math__max(a, b) (((a) > (b)) ? (a) : (b))
#define Math__clamp(min, n, max) (((n) < (min)) ? (min) : ((max) < (n)) ? (max) : (n))

#define Math__PI32 (3.14159265359f)
#define Math__PI64 (3.14159265358979323846)
#define Math__DEG2RAD32 (Math__PI32 / 180.0f)
#define Math__DEG2RAD64 (Math__PI64 / 180.0)
#define Math__RAD2DEG32 (180.0f / Math__PI32)
#define Math__RAD2DEG64 (180.0 / Math__PI64)

typedef struct v3 {
  f32 x, y, z;
} v3;

// min, max, clamp
f32 Math__rclampf(f32 min, f32 n, f32 max, f32 step);
f32 Math__fmodf(f32 n, f32 d);
u32 Math__ceil(f32 n);
u32 Math__floor(f32 n);
f32 Math__fabsf(f32 x);
f32 Math__pow4(f32 n);

// sqrt
f32 Math__sqrtf(f32 n);

// sin, cos, tan
// atan2 (can be impl using dot-product)
f32 Math__cosf(f32 rad);
f32 Math__sinf(f32 rad);
// f32 Math__tanf(f32 rad);
f32 Math__tanf(f32 x);
f32 Math__atanf(f32 rad);
f32 Math__atan2f(f32 y, f32 x);
f32 Math__atan2f2(f32 y, f32 x);
f32 Math__atan2f3(f32 y, f32 x);
f32 Math__acosf(f32 x);

f32 Math__randomf(f32 min, f32 max, u64* state);

extern inline v3 v3_cp(v3* a) {
  return *a;
}

extern inline f32 v3_dot(v3* a, v3* b) {
  return a->x * b->x + a->y * b->y + a->z * b->z;
}

extern inline f32 v3_len2(v3* a) {
  return v3_dot(a, a);
}

extern inline f32 v3_len(v3* a) {
  return Math__sqrtf(v3_len2(a));
}

// lerp/mix, slerp
f32 lerp(f32 v0, f32 v1, f32 t);
v3 v3_slerp(v3 v0, v3 v1, f32 t);

// {div,mul,add,sub} *Scalar [XYZ]
// invert [XYZ]
// mix/lerp [XYZ]
// norm
// dot, cross
// clamp, limit/dampOver
// zero, one, id
// len, dist [Abs] [2|Sqrt] [XYZ]
// rot [*|To|By] (LH) [Deg|Rad] [XYZ]
// isZero, isEqual
// rand

// toStr, toFixed
// unfloat
// projectOnto
// vAng, hAng [Deg|Rad]

// struct m4
// q_fromAxis
// m4_ortho, m4_persp (LH) (NO)
// m4_trans, m4_scale, m4_rot (LH)
// m4_fromQ