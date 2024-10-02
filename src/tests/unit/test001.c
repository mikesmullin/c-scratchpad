#include "test001.h"

#include <float.h>
#include <math.h>
#include <stdio.h>

#include "../../lib/Base.h"
#include "../../lib/Breakpoint.h"

static f32 safe_division(f32 numerator, f32 denominator) {
  // Check if y is close to zero, then clamp the value of z
  if (fabs(denominator) < FLT_EPSILON) {
    return 0.0f;  // Clamp z when y is too small
  }
  return numerator / denominator;
}

static void Test001__Test() {
  LOG_DEBUGF("Test001");

  f32 steps = 10.0f;
  f32 b = 1.0f;
  f32 a = -b;
  f32 step = (b - a) / steps;
  for (f32 y = a; y <= b; y += step) {
    for (f32 x = a; x <= b; x += step) {
      f32 z = safe_division(x, y);
      if (y < 0) z = -z;

      printf("% 5.3f ", z);
    }
    printf("\n");
  }
}

void Test__Boot() {
  Test001__Test();
}
