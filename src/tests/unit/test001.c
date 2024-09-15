#include "test001.h"

#include "../../lib/Arena.h"
#include "../../lib/Base.h"
#include "../../lib/String.h"

void Test001__Test() {
  LOG_DEBUGF("\n\nRunning test001!");

  Arena_t arena = Arena__Alloc(1024);
  char* s1 = Arena__Push(&arena, 8 * sizeof(char));
  memcpy(s1, "Hello, \0", 8);

  char* s2 = Arena__Push(&arena, 7 * sizeof(char));
  memcpy(s2, "World!\0", 7);

  LOG_DEBUGF("rendering string parts from arena memory:")
  LOG_DEBUGF("%s%s", s1, s2);

  Arena__Free(&arena);

  // LOG_DEBUGF("rendering string parts after arena free:")
  // LOG_DEBUGF("%s%s", s1, s2);

  Test002__Test();
}

void Test002__Test() {
  LOG_DEBUGF("\n\nRunning test002!");

  Arena_t arena = Arena__Alloc(1024);
  void* p1 = Arena__Push(&arena, 8 * sizeof(char));
  String8 s1 = {.size = 8, .str = p1};
  memcpy(p1, "Hello, \0", 8);
  String8Node sn1 = {.string = &s1};

  void* p2 = Arena__Push(&arena, 7 * sizeof(char));
  memcpy(p2, "World!\0", 7);

  String8 s2 = {.size = 7, .str = p2};
  String8Node sn2 = {.string = &s2};
  sn1.next = &sn2;

  LOG_DEBUGF("%s%s", sn1.string->str, sn1.next->string->str);
}