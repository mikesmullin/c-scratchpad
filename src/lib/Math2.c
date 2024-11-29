#include "Math2.h"

#include "Base.h"

static const f64 pi = 3.1415926535897932384626433;  // pi
static const f64 twopi = 2.0 * pi;  // pi times 2
static const f64 two_over_pi = 2.0 / pi;  // 2/pi
static const f64 halfpi = pi / 2.0;  // pi divided by 2
static const f64 threehalfpi = 3.0 * pi / 2.0;  // pi times 3/2, used in tan routines
static const f64 four_over_pi = 4.0 / pi;  // 4/pi, used in tan routines
static const f64 qtrpi = pi / 4.0;  // pi/4.0, used in tan routines
static const f64 sixthpi = pi / 6.0;  // pi/6.0, used in atan routines
static const f64 tansixthpi = 0.57735026918962576451;  // tan(pi/6), used in atan routines
static const f64 twelfthpi = pi / 12.0;  // pi/12.0, used in atan routines
static const f64 tantwelfthpi = 0.26794919243112270647;  // tan(pi/12), used in atan routines

static const f32 NaN32 = 0.0f / 0.0f;
static const f32 Infinity32 = +1.0f / 0.0f;
static const f32 NInfinity32 = -1.0f / 0.0f;

// partial copy from: https://www.ganssle.com/item/approximations-for-trig-c-code.htm
// see also: https://developer.download.nvidia.com/cg/index_stdlib.html
// NOTICE: these approximation fns are:
// - less precise, but
// - faster than stdlib, and
// - perform in constant time, and
// - are cross-platform compatible

f32 Math__fmodf(f32 n, f32 d) {
  if (n == 0.0f || d == 0.0f) {
    return 0.0f;  // 0 mod anything is 0
  }
  // handle division by zero
  if (d == 0.0f) {
    // undefined behavior
    return NaN32;
  }
  // handle infinity inputs
  if (n > Infinity32 || n < NInfinity32 || d > Infinity32 || d < NInfinity32) {
    return NaN32;  // NaN
  }

  u32 w = (u32)(n / d);  // truncate division result
  return n - (f32)w * d;  // calculate the remainder
}

// wrap-around: clamps x to range [n,m] inclusive by step s
f32 Math__wrapaf(f32 n, f32 x, f32 m, f32 s) {
  while (x < n) x += s;
  while (x > m) x -= s;
  return x;
}

// 6-digit approx
f32 Math__sqrtf(f32 n) {
  if (n <= 0.0f) {
    if (n == 0.0f) {
      return 0.0f;  // sqrt(0) is 0
    }
    return -1.0f;  // Return error for negative input (sqrt is not defined for negative numbers in real numbers)
  }

  // Initial guess for Newton's method: Start with n / 2 (or any positive guess)
  f32 x = n / 2.0f;

  // Newton's method
  // 10 iterations
  x = 0.5f * (x + n / x);
  x = 0.5f * (x + n / x);
  x = 0.5f * (x + n / x);
  x = 0.5f * (x + n / x);
  x = 0.5f * (x + n / x);
  x = 0.5f * (x + n / x);
  x = 0.5f * (x + n / x);
  x = 0.5f * (x + n / x);
  x = 0.5f * (x + n / x);
  x = 0.5f * (x + n / x);

  return x;
}

u32 Math__ceil(f32 n) {
  u32 i = (u32)n;
  if (n == (f32)i) {
    return i;
  }
  return i + 1;
}

u32 Math__floor(f32 n) {
  u32 i = (u32)n;
  if (n < 0 && n != (f32)i) {
    return i - 1;
  }
  return i;
}

static f64 _cos(f64 x) {
  static const f64 c1 = 0.999999953464;
  static const f64 c2 = -0.499999053455;
  static const f64 c3 = 0.0416635846769;
  static const f64 c4 = -0.0013853704264;
  static const f64 c5 = 0.00002315393167;

  f64 x2 = x * x;
  return (c1 + x2 * (c2 + x2 * (c3 + x2 * (c4 + c5 * x2))));
}

// 7-digit approx
f32 Math__cosf(f32 rad) {
  rad = Math__fmodf(rad, twopi);  // Get rid of values > 2* pi
  if (rad < 0) rad = -rad;  // cos(-x) = cos(x)
  u8 quad = rad * two_over_pi;  // Get quadrant # (0 to 3) we're in
  if (0 == quad) return _cos(rad);
  if (1 == quad) return -_cos(pi - rad);
  if (2 == quad) return -_cos(rad - pi);
  if (3 == quad) return _cos(twopi - rad);
  ASSERT_CONTEXT(false, "Math__tanf() quad must be in range 0-3. n: %u", quad);
}

f32 Math__sinf(f32 rad) {
  return Math__cosf(halfpi - rad);
}

static f64 _tanf(f64 x) {
  static const f64 c1 = 211.849369664121;
  static const f64 c2 = -12.5288887278448;
  static const f64 c3 = 269.7350131214121;
  static const f64 c4 = -71.4145309347748;

  f64 x2 = x * x;
  return (x * (c1 + c2 * x2) / (c3 + x2 * (c4 + x2)));
}

// 8-digit approx
f32 Math__tanf(f32 rad) {
  rad = Math__fmodf(rad, twopi);  // Get rid of values >2 *pi
  // rad = Math__rclampf(0.0f, rad, twopi, twopi);
  u8 octant = rad * four_over_pi;  // Get octant # (0 to 7)
  if (0 == octant) return _tanf(rad * four_over_pi);
  if (1 == octant) return 1.0 / _tanf((halfpi - rad) * four_over_pi);
  if (2 == octant) return -1.0 / _tanf((rad - halfpi) * four_over_pi);
  if (3 == octant) return -_tanf((pi - rad) * four_over_pi);
  if (4 == octant) return _tanf((rad - pi) * four_over_pi);
  if (5 == octant) return 1.0 / _tanf((threehalfpi - rad) * four_over_pi);
  if (6 == octant) return -1.0 / _tanf((rad - threehalfpi) * four_over_pi);
  if (7 == octant) return -_tanf((twopi - rad) * four_over_pi);
  ASSERT_CONTEXT(false, "Math__tanf() octant must be in range 0-7. n: %u", octant);
}

f64 _atan(f64 x) {
  static const f64 c1 = 1.6867629106;
  static const f64 c2 = 0.4378497304;
  static const f64 c3 = 1.6867633134;

  f64 x2 = x * x;
  return (x * (c1 + x2 * c2) / (c3 + x2));
}

// 6-digit approx
f32 Math__atanf(f32 rad) {
  f64 y;  // return from atan__s function
  bool complement = false;  // true if arg was >1
  bool region = false;  // true depending on region arg is in
  bool sign = false;  // true if arg was < 0

  if (rad < 0) {
    rad = -rad;
    sign = true;  // arctan(-x)=-arctan(x)
  }
  if (rad > 1.0) {
    rad = 1.0 / rad;  // keep arg between 0 and 1
    complement = true;
  }
  if (rad > tantwelfthpi) {
    rad = (rad - tansixthpi) / (1 + tansixthpi * rad);  // reduce arg to under tan(pi/12)
    region = true;
  }

  y = _atan(rad);  // run the approximation
  if (region) y += sixthpi;  // correct for region we're in
  if (complement) y = halfpi - y;  // correct for 1/x if we did that
  if (sign) y = -y;  // correct for negative arg
  return y;
}

// 6-digit approx
f32 Math__atan2f(f32 y, f32 x) {
  if (x > 0) {
    return Math__atanf(y / x);
  } else if (x < 0 && y >= 0) {
    return Math__atanf(y / x) + pi;
  } else if (x < 0 && y < 0) {
    return Math__atanf(y / x) - pi;
  } else if (x == 0 && y > 0) {
    return pi / 2;
  } else if (x == 0 && y < 0) {
    return -pi / 2;
  } else {  // Both x and y are 0
    return 0.0f;  // or undefined
  }
}

// arccosine
// see also: https://developer.download.nvidia.com/cg/acos.html
// 5-digit approx; Absolute error <= 6.7e-5
f32 Math__acosf(f32 x) {
  // Handbook of Mathematical Functions
  // M. Abramowitz and I.A. Stegun, Ed.
  f32 negate = (f32)(x < 0);
  x = Math__fabsf(x);
  f32 ret = -0.0187293;
  ret = ret * x;
  ret = ret + 0.0742610;
  ret = ret * x;
  ret = ret - 0.2121144;
  ret = ret * x;
  ret = ret + 1.5707288;
  ret = ret * Math__sqrtf(1.0 - x);
  ret = ret - 2 * negate * ret;
  return negate * 3.14159265358979 + ret;
}

// f32 Math__acosf(f32 x) {
//   // Initial guess
//   f64 guess = halfpi;

//   // Allowable error
//   static const f64 epsilon = 1e-9;

//   // Iteration variable
//   u8 maxIterations = 100, iteration = 0;

//   // Newton-Raphson method
//   while (Math__fabsf(Math__cosf(guess) - x) > epsilon && iteration < maxIterations) {
//     guess = guess - (Math__cosf(guess) - x) / (-Math__sinf(guess));
//     iteration++;
//   }

//   return guess;
// }

// static f64 dot2(f64 x, f64 y) {
//   return x * x + y * y;
// }
// static f64 cross2(f64 x1, f64 y1, f64 x2, f64 y2) {
//   return x1 * x2 + y1 * y2;
// }
// static f64 f(f64 x1, f64 y1, f64 x2, f64 y2) {
//   f64 ab = dot2(x1, y1) * dot2(x2, y2);
//   f64 r = cross2(x1, y1, x2, y2) * Math__sqrtf(ab) / ab;
//   return Math__acosf(r);
// }
// f32 Math__atan2f3(f32 y, f32 x) {
//   if (y > 0) {  // I and II quadrants
//     return 0 == x ? pi / 2 :  // Positive y-axis
//                f(x, y, 1, 0);
//   } else if (y < 0) {  // III and IV quadrants
//     return 0 == x ? -pi / 2 :  // Positive y-axis
//                f(x, y, -1, 0) - pi;
//   }
//   return 0;
// };

// SplitMix64 PRNG function
// - deterministic
// - cross-platform
// - fast speed
// - long period
// - decent quality
static u64 _splitMix64(u64* state) {
  u64 z = (*state += 0x9e3779b97f4a7c15ULL);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
  z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
  return z ^ (z >> 31);
}

f32 Math__randomf(f32 min, f32 max, u64* state) {
  // Generate the next random integer using SplitMix64
  u64 rand_int = _splitMix64(state);

  // Normalize the integer to a floating-point value in the range [0, 1)
  f32 normalized = (f32)(rand_int >> 11) * (1.0f / (f32)(1ULL << 53));

  // Scale and shift the value to the desired range [min, max]
  return min + normalized * (max - min);
}

// normalize

void v1_norm(const v1* a, v1* dst) {
  f32 len = v1_mag(a);
  if (len > 0.0f) {
    dst->x /= len;
  }
}

void v2_norm(const v2* a, v2* dst) {
  f32 len = v2_mag(a);
  if (len > 0.0f) {
    dst->x /= len;
    dst->y /= len;
  }
}

void v3_norm(const v3* a, v3* dst) {
  f32 len = v3_mag(a);
  if (len > 0.0f) {
    dst->x /= len;
    dst->y /= len;
    dst->z /= len;
  }
}

void v4_norm(const v4* a, v4* dst) {
  f32 len = v4_mag(a);
  if (len > 0.0f) {
    dst->x /= len;
    dst->y /= len;
    dst->z /= len;
    dst->w /= len;
  }
}

// spherical linear interpolation
void v3_slerp(v3* a, v3* b, f32 t, v3* dst) {
  // normalize the input vectors
  v3_norm(a, a);
  v3_norm(b, b);

  // dot product
  f32 dp = v3_dot(a, b);

  // clamp the dot product to avoid floating point errors
  if (dp < -1.0) dp = -1.0;
  if (dp > 1.0) dp = 1.0;

  // angle between the vectors
  f32 theta = Math__acosf(dp) * t;

  // orthogonal vector
  dst->x = b->x - a->x * dp;
  dst->y = b->y - a->y * dp;
  dst->z = b->z - a->z * dp;
  v3_norm(dst, dst);

  // result
  dst->x = a->x * Math__cosf(theta) + dst->x * Math__sinf(theta);
  dst->y = a->y * Math__cosf(theta) + dst->y * Math__sinf(theta);
  dst->z = a->z * Math__cosf(theta) + dst->z * Math__sinf(theta);
}

// left-handed orthographic projection matrix
// with Z ranging from -1 to 1 (GL convention)
void m4_ortho(m4* dst, f32 l, f32 r, f32 b, f32 t, f32 znear, f32 zfar) {
  // TODO:
}

// left-handed perspective projection matrix
// with Z ranging from -1 to 1 (GL convention)
void m4_perspective(m4* dst, f32 fov, f32 aspect, f32 znear, f32 zfar) {
  f32 f = 1.0f / Math__tanf(fov * 0.5f);  // fov is y-axis only
  f32 fn = 1.0f / (znear - zfar);

  // clang-format off
  m4_set4x4(dst,
    f / aspect, 0.0f, 0.0f, 0.0f,
    0.0f, f, 0.0f, 0.0f,
    0.0f, 0.0f, (znear + zfar) * fn, -1.0f,
    0.0f, 0.0f, 2.0f * znear * zfar * fn, 0.0f
  );
  // clang-format on
}

// quaternions

void q_fromAxis(v3 axis, f32 angle, v4* dst) {
  // Normalize the axis vector
  f32 length = Math__sqrtf(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
  if (length < 1e-6f) {  // Avoid division by zero
    dst->x = dst->y = dst->z = 0.0f, dst->w = 1.0f;  // Identity quaternion
    return;
  }
  f32 invLength = 1.0f / length;
  axis.x *= invLength;
  axis.y *= invLength;
  axis.z *= invLength;

  // Compute sine and cosine of half the angle
  f32 halfAngle = angle * 0.5f;
  f32 sinHalfAngle = Math__sinf(halfAngle);
  f32 cosHalfAngle = Math__cosf(halfAngle);

  // Create the quaternion
  dst->x = axis.x * sinHalfAngle;
  dst->y = axis.y * sinHalfAngle;
  dst->z = axis.z * sinHalfAngle;
  dst->w = cosHalfAngle;
}
