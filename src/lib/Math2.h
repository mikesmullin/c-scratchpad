#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
typedef int32_t s32;
typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

#define Math__PI32 (3.14159265359f)
#define Math__PI64 (3.14159265358979323846)
#define Math__DEG2RAD32 (Math__PI32 / 180.0f)
#define Math__DEG2RAD64 (Math__PI64 / 180.0)
#define Math__RAD2DEG32 (180.0f / Math__PI32)
#define Math__RAD2DEG64 (180.0 / Math__PI64)

// min, max, clamp
#define Math__min(a, b) (((a) < (b)) ? (a) : (b))
#define Math__max(a, b) (((a) > (b)) ? (a) : (b))
#define Math__clamp(min, n, max) (((n) < (min)) ? (min) : ((max) < (n)) ? (max) : (n))
// f32 Math__map(f32 n, f32 input_start, f32 input_end, f32 output_start, f32 output_end);
f32 Math__fmodf(f32 n, f32 d);
f32 Math__wrapaf(f32 n, f32 x, f32 m, f32 s);
u32 Math__ceil(f32 n);
u32 Math__floor(f32 n);

extern inline f32 Math__sign(f32 n) {
  return n < 0 ? -1.0f : +1.0f;
}

extern inline f32 Math__fabsf(f32 x) {
  return (x < 0.0f) ? -x : x;
}

extern inline f32 Math__pow4(f32 n) {
  return n * n * n * n;
}

// sqrt
f32 Math__sqrtf(f32 n);

// trig
f32 Math__cosf(f32 rad);
f32 Math__sinf(f32 rad);
f32 Math__tanf(f32 rad);
f32 Math__acosf(f32 rad);
f32 Math__atanf(f32 rad);
f32 Math__atan2f(f32 y, f32 x);

// prng
f32 Math__randomf(f32 min, f32 max, u64* state);

// vector structures (1D, 2D, 3D, and 4D)

typedef struct v1 {
  f32 x;
} v1;

typedef struct v2 {
  f32 x, y;
} v2;

typedef struct v3 {
  f32 x, y, z;
} v3;

typedef struct v4 {
  f32 x, y, z, w;
} v4;

typedef struct m4 {
  v4 a, b, c, d;
} m4;

// vector constants

static const v3 V3_ZERO = (v3){0.0f, 0.0f, 0.0f};
static const v3 V3_ONE = (v3){1.0f, 1.0f, 1.0f};
static const v3 V3_UP = (v3){0.0f, 1.0f, 0.0f};  // +Y_UP
static const v3 V3_FWD = (v3){0.0f, 0.0f, -1.0f};  // -Z_FWD
static const v3 V3_RIGHT = (v3){1.0f, 0.0f, 0.0f};  // +X_RIGHT
// clang-format off
static const m4 M4_IDENTITY = (m4){
  {1.0f, 0.0f, 0.0f, 0.0f},
  {0.0f, 1.0f, 0.0f, 0.0f},
  {0.0f, 0.0f, 1.0f, 0.0f},
  {0.0f, 0.0f, 0.0f, 1.0f},
};
// clang-format on

// constructors

extern inline void v3_new(v3* dst, f32 x, f32 y, f32 z) {
  dst->x = x;
  dst->y = y;
  dst->z = z;
}

// clone

extern inline v1 v1_cp(v1* a) {
  return *a;
}

extern inline v2 v2_cp(v2* a) {
  return *a;
}

extern inline v3 v3_cp(v3* a) {
  return *a;
}

extern inline v4 v4_cp(v4* a) {
  return *a;
}

// set

extern inline void v3_set(v3* dst, const v3* src) {
  dst->x = src->x;
  dst->y = src->y;
  dst->z = src->z;
}

extern inline void v3_set3(v3* dst, const f32 x, const f32 y, const f32 z) {
  dst->x = x;
  dst->y = y;
  dst->z = z;
}

// comparison

extern inline bool v3_equal(const v3* a, const v3* b) {  // ==
  return a->x == b->x && a->y == b->y && a->z == b->z;
}

// serialization

extern inline void v3_str(const v3* a, char* dst, const u32 len) {  // toString
  sprintf_s(dst, len, "%f %f %f", a->x, a->y, a->z);
}

// dot product (a⋅b)

extern inline f32 v1_dot(const v1* a, const v1* b) {
  return a->x * b->x;
}

extern inline f32 v2_dot(const v2* a, const v2* b) {
  return v1_dot((v1*)a, (v1*)b) + a->y * b->y;
}

// can think of it as the length of an (opposite) side of a triangle
extern inline f32 v3_dot(const v3* a, const v3* b) {
  return v2_dot((v2*)a, (v2*)b) + a->z * b->z;
}

extern inline f32 v4_dot(const v4* a, const v4* b) {
  return v3_dot((v3*)a, (v3*)b) + a->w * b->w;
}

// magnitude squared
// because avoiding sqrt(x)) is computationally cheaper
// and often sufficient (e.g., for checking relative distances)

extern inline f32 v1_mag2(const v1* a) {
  return v1_dot(a, a);
}

extern inline f32 v2_mag2(const v2* a) {
  return v2_dot(a, a);
}

extern inline f32 v3_mag2(const v3* a) {
  return v3_dot(a, a);
}

extern inline f32 v4_mag2(const v4* a) {
  return v4_dot(a, a);
}

// magnitude

extern inline f32 v1_mag(const v1* a) {
  return Math__sqrtf(v1_mag2(a));
}

extern inline f32 v2_mag(const v2* a) {
  return Math__sqrtf(v2_mag2(a));
}

extern inline f32 v3_mag(const v3* a) {
  return Math__sqrtf(v3_mag2(a));
}

extern inline f32 v4_mag(const v4* a) {
  return Math__sqrtf(v4_mag2(a));
}

// normalize
void v1_norm(const v1* a, v1* dst);
void v2_norm(const v2* a, v2* dst);
void v3_norm(const v3* a, v3* dst);
void v4_norm(const v4* a, v4* dst);

// PEMDAS
// {mul,div,add,sub} *Scalar

// #metacode
// #macro MACRO1(T1)
//   {{~#for T1}}
//   // {{this.op}}
//   extern inline void v1_{{this.op}}(v1* a, v1* b, v1* dst) {
//     dst->x = a->x {{this.oper}} b->x;
//   }
//
//   extern inline void v2_{{this.op}}(v2* a, v2* b, v2* dst) {
//     v1_{{this.op}}((v1*)a, (v1*)b, (v1*)dst);
//     dst->y = a->y {{this.oper}} b->y;
//   }
//
//   extern inline void v3_{{this.op}}(v3* a, v3* b, v3* dst) {
//     v2_{{this.op}}((v2*)a, (v2*)b, (v2*)dst);
//     dst->z = a->z {{this.oper}} b->z;
//   }
//
//   extern inline void v4_{{this.op}}(v4* a, v4* b, v4* dst) {
//     v3_{{this.op}}((v3*)a, (v3*)b, (v3*)dst);
//     dst->w = a->w {{this.oper}} b->w;
//   }
//
//   // {{this.op}} scalar
//
//   extern inline void v1_{{this.op}}S(v1* a, f32 s, v1* dst) {
//     dst->x = a->x {{this.oper}} s;
//   }
//
//   extern inline void v2_{{this.op}}S(v2* a, f32 s, v2* dst) {
//     v1_{{this.op}}S((v1*)a, s, (v1*)dst);
//     dst->y = a->y {{this.oper}} s;
//   }
//
//   extern inline void v3_{{this.op}}S(v3* a, f32 s, v3* dst) {
//     v2_{{this.op}}S((v2*)a, s, (v2*)dst);
//     dst->z = a->z {{this.oper}} s;
//   }
//
//   extern inline void v4_{{this.op}}S(v4* a, f32 s, v4* dst) {
//     v3_{{this.op}}S((v3*)a, s, (v3*)dst);
//     dst->w = a->w {{this.oper}} s;
//   }
//   {{~/for~}}
//
// #table T_VARS
//   op  | oper |
//   mul | *    |
//   div | /    |
//   add | +    |
//   sub | -    |
//
// MACRO1(T_VARS)
// #metagen

// mul
extern inline void v1_mul(v1* a, v1* b, v1* dst) {
  dst->x = a->x * b->x;
}

extern inline void v2_mul(v2* a, v2* b, v2* dst) {
  v1_mul((v1*)a, (v1*)b, (v1*)dst);
  dst->y = a->y * b->y;
}

extern inline void v3_mul(v3* a, v3* b, v3* dst) {
  v2_mul((v2*)a, (v2*)b, (v2*)dst);
  dst->z = a->z * b->z;
}

extern inline void v4_mul(v4* a, v4* b, v4* dst) {
  v3_mul((v3*)a, (v3*)b, (v3*)dst);
  dst->w = a->w * b->w;
}

// mul scalar

extern inline void v1_mulS(v1* a, f32 s, v1* dst) {
  dst->x = a->x * s;
}

extern inline void v2_mulS(v2* a, f32 s, v2* dst) {
  v1_mulS((v1*)a, s, (v1*)dst);
  dst->y = a->y * s;
}

extern inline void v3_mulS(v3* a, f32 s, v3* dst) {
  v2_mulS((v2*)a, s, (v2*)dst);
  dst->z = a->z * s;
}

extern inline void v4_mulS(v4* a, f32 s, v4* dst) {
  v3_mulS((v3*)a, s, (v3*)dst);
  dst->w = a->w * s;
}

// div
extern inline void v1_div(v1* a, v1* b, v1* dst) {
  dst->x = a->x / b->x;
}

extern inline void v2_div(v2* a, v2* b, v2* dst) {
  v1_div((v1*)a, (v1*)b, (v1*)dst);
  dst->y = a->y / b->y;
}

extern inline void v3_div(v3* a, v3* b, v3* dst) {
  v2_div((v2*)a, (v2*)b, (v2*)dst);
  dst->z = a->z / b->z;
}

extern inline void v4_div(v4* a, v4* b, v4* dst) {
  v3_div((v3*)a, (v3*)b, (v3*)dst);
  dst->w = a->w / b->w;
}

// div scalar

extern inline void v1_divS(v1* a, f32 s, v1* dst) {
  dst->x = a->x / s;
}

extern inline void v2_divS(v2* a, f32 s, v2* dst) {
  v1_divS((v1*)a, s, (v1*)dst);
  dst->y = a->y / s;
}

extern inline void v3_divS(v3* a, f32 s, v3* dst) {
  v2_divS((v2*)a, s, (v2*)dst);
  dst->z = a->z / s;
}

extern inline void v4_divS(v4* a, f32 s, v4* dst) {
  v3_divS((v3*)a, s, (v3*)dst);
  dst->w = a->w / s;
}

// add
extern inline void v1_add(v1* a, v1* b, v1* dst) {
  dst->x = a->x + b->x;
}

extern inline void v2_add(v2* a, v2* b, v2* dst) {
  v1_add((v1*)a, (v1*)b, (v1*)dst);
  dst->y = a->y + b->y;
}

extern inline void v3_add(v3* a, v3* b, v3* dst) {
  v2_add((v2*)a, (v2*)b, (v2*)dst);
  dst->z = a->z + b->z;
}

extern inline void v4_add(v4* a, v4* b, v4* dst) {
  v3_add((v3*)a, (v3*)b, (v3*)dst);
  dst->w = a->w + b->w;
}

// add scalar

extern inline void v1_addS(v1* a, f32 s, v1* dst) {
  dst->x = a->x + s;
}

extern inline void v2_addS(v2* a, f32 s, v2* dst) {
  v1_addS((v1*)a, s, (v1*)dst);
  dst->y = a->y + s;
}

extern inline void v3_addS(v3* a, f32 s, v3* dst) {
  v2_addS((v2*)a, s, (v2*)dst);
  dst->z = a->z + s;
}

extern inline void v4_addS(v4* a, f32 s, v4* dst) {
  v3_addS((v3*)a, s, (v3*)dst);
  dst->w = a->w + s;
}

// sub
extern inline void v1_sub(v1* a, v1* b, v1* dst) {
  dst->x = a->x - b->x;
}

extern inline void v2_sub(v2* a, v2* b, v2* dst) {
  v1_sub((v1*)a, (v1*)b, (v1*)dst);
  dst->y = a->y - b->y;
}

extern inline void v3_sub(v3* a, v3* b, v3* dst) {
  v2_sub((v2*)a, (v2*)b, (v2*)dst);
  dst->z = a->z - b->z;
}

extern inline void v4_sub(v4* a, v4* b, v4* dst) {
  v3_sub((v3*)a, (v3*)b, (v3*)dst);
  dst->w = a->w - b->w;
}

// sub scalar

extern inline void v1_subS(v1* a, f32 s, v1* dst) {
  dst->x = a->x - s;
}

extern inline void v2_subS(v2* a, f32 s, v2* dst) {
  v1_subS((v1*)a, s, (v1*)dst);
  dst->y = a->y - s;
}

extern inline void v3_subS(v3* a, f32 s, v3* dst) {
  v2_subS((v2*)a, s, (v2*)dst);
  dst->z = a->z - s;
}

extern inline void v4_subS(v4* a, f32 s, v4* dst) {
  v3_subS((v3*)a, s, (v3*)dst);
  dst->w = a->w - s;
}

// #metaend

// invert [XYZ]

extern inline void v2_invertX(v2* a, v2* dst) {
  dst->x = a->y;
}

extern inline void v2_invertY(v2* a, v2* dst) {
  dst->y = a->x;
}

extern inline void v2_invert(v2* a, v2* dst) {
  v2_invertX(a, dst);
  v2_invertY(a, dst);
}

// cross product (a×b)

// 2D cross differs in that it returns a scalar,
// representing the perpendicular vector's magnitude
// if extended into 3D space.
// We can also think of it as the area of a parallelogram.
extern inline f32 v2_cross(v2* a, v2* b) {
  return a->x * b->y - a->y * b->x;
}

// 3D cross returns a new vector
// orthogonal (perpendicular) to both a and b
extern inline void v3_cross(v3* a, v3* b, v3* dst) {
  dst->x = a->y * b->z - a->z * b->y;
  dst->y = a->z * b->x - a->x * b->z;
  dst->z = a->x * b->y - a->y * b->x;
}

extern inline void v3_limit(v3* a, f32 max, v3* dst) {
  v3_set(dst, a);
  if (a->x > max) dst->x = max;
  if (a->y > max) dst->y = max;
  if (a->z > max) dst->z = max;
}

extern inline void v3_clamp(v3* a, f32 n, f32 m, v3* dst) {
  v3_set(dst, a);
  if (a->x > m) dst->x = m;
  if (a->x < n) dst->x = n;
  if (a->y > m) dst->y = m;
  if (a->y < n) dst->y = n;
  if (a->z > m) dst->z = m;
  if (a->z < n) dst->z = n;
}

extern inline void v3_dampenOver(v3* a, f32 max, f32 factor, v3* dst) {
  if (a->x > max) dst->x = a->x * factor;
  if (a->y > max) dst->y = a->y * factor;
  if (a->z > max) dst->z = a->z * factor;
}

extern inline void v3_rand(v3* tl, v3* br, u64* prng, v3* dst) {
  dst->x = Math__randomf(tl->x, br->x, prng);
  dst->y = Math__randomf(tl->y, br->y, prng);
  dst->z = Math__randomf(tl->z, br->z, prng);
}

// len, dist [Abs] [2|Sqrt] [XYZ]
// rot [*|To|By] (LH) [Deg|Rad] [XYZ]
// projectOnto
// vAng, hAng [Deg|Rad]

// lerp/mix, slerp [XYZ]
extern inline f32 lerp(f32 a, f32 b, f32 t) {
  return (1 - t) * a + t * b;
}

void v3_slerp(v3* a, v3* b, f32 t, v3* dst);

// matrix 4x4

// clang-format off
extern inline void m4_set4x4(m4* dst,
  f32 c00, f32 c01, f32 c02, f32 c03,
  f32 c10, f32 c11, f32 c12, f32 c13,
  f32 c20, f32 c21, f32 c22, f32 c23,
  f32 c30, f32 c31, f32 c32, f32 c33
) {
  dst->a.x = c00, dst->a.y = c01, dst->a.z = c02, dst->a.w = c03;
  dst->b.x = c10, dst->b.y = c11, dst->b.z = c12, dst->b.w = c13;
  dst->c.x = c20, dst->c.y = c21, dst->c.z = c22, dst->c.w = c23;
  dst->d.x = c30, dst->d.y = c31, dst->d.z = c32, dst->d.w = c33;
}
// clang-format on

extern inline void m4_trans(m4* dst, f32 x, f32 y, f32 z) {
  // clang-format off
  m4_set4x4(dst,
    1.0f, 0.0f, 0.0f, x,
    0.0f, 1.0f, 0.0f, y,
    0.0f, 0.0f, 1.0f, z,
    0.0f, 0.0f, 0.0f, 1.0f
  );
  // clang-format on
}

extern inline void m4_scale(m4* dst, f32 x, f32 y, f32 z) {
  // clang-format off
  m4_set4x4(dst,
    x, 0.0f, 0.0f, 0.0f,
    0.0f, y, 0.0f, 0.0f,
    0.0f, 0.0f, z, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  );
  // clang-format on
}

extern inline void m4_rotX(m4* dst, f32 deg) {  // LH
  f32 s = Math__sinf(deg), c = Math__cosf(deg);
  // clang-format off
  m4_set4x4(dst,
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, c, s, 0.0f,
    0.0f, -s, c, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  );
  // clang-format on
}

extern inline void m4_rotY(m4* dst, f32 deg) {  // LH
  f32 s = Math__sinf(deg), c = Math__cosf(deg);
  // clang-format off
  m4_set4x4(dst,
    c, 0.0f, -s, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    s, 0.0f, c, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  );
  // clang-format on
}

extern inline void m4_rotZ(m4* dst, f32 deg) {  // LH
  f32 s = Math__sinf(deg), c = Math__cosf(deg);
  // clang-format off
  m4_set4x4(dst,
    c, s, 0.0f, 0.0f,
    -s, c, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  );
  // clang-format on
}

// left-handed orthographic projection matrix
// with Z ranging from -1 to 1 (GL convention)
void m4_ortho(m4* dst, f32 l, f32 r, f32 b, f32 t, f32 znear, f32 zfar);

// left-handed perspective projection matrix
// with Z ranging from -1 to 1 (GL convention)
void m4_perspective(m4* dst, f32 fov, f32 aspect, f32 znear, f32 zfar);

// quaternions

void q_fromAxis(v3 axis, f32 angle, v4* dst);
void m4_fromQ(v4* left, m4* dst);