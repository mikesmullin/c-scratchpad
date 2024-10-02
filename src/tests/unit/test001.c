#include "test001.h"

#include "../../lib/Base.h"
#include "../../lib/Breakpoint.h"

static void Test001__Test() {
  LOG_DEBUGF("Test001");
  LOG_DEBUGF("a");
  DEBUGGER();
  LOG_DEBUGF("b");
  LOG_DEBUGF("c");
}

void Test__Boot() {
  Test001__Test();
}
