#include "test001.h"

#include "../../lib/Arena.h"
#include "../../lib/Base.h"
#include "../../lib/Hashmap.h"
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
}

void Test002__Test() {
  LOG_DEBUGF("\n\nRunning test002!");

  Arena_t arena = Arena__Alloc(1024);
  void* p1 = Arena__Push(&arena, 8 * sizeof(char));

  String8* s1 = str8_alloc(&arena, "Hello, ");
  String8Node sn1 = {.string = s1};

  void* p2 = Arena__Push(&arena, 7 * sizeof(char));
  memcpy(p2, "World!", 7);

  String8 s2 = {.size = 7, .str = p2};
  String8Node sn2 = {.string = &s2};
  sn1.next = &sn2;

  LOG_DEBUGF("%s%s", sn1.string->str, sn1.next->string->str);
}

void Test003__Test() {
  HashMap* map = create_hashmap();

  // Insert some key-value pairs
  hashmap_insert(map, "name", "Alice");
  hashmap_insert(map, "age", "30");
  hashmap_insert(map, "city", "New York");

  // Retrieve values
  LOG_DEBUGF("Name: %s\n", hashmap_get(map, "name"));
  LOG_DEBUGF("Age: %s\n", hashmap_get(map, "age"));
  LOG_DEBUGF("City: %s\n", hashmap_get(map, "city"));

  // Update an existing value
  hashmap_insert(map, "city", "Los Angeles");
  LOG_DEBUGF("Updated City: %s\n", hashmap_get(map, "city"));

  // Free the hashmap
  free_hashmap(map);
}

void Test004__Test() {
  LOG_DEBUGF("Test004");
  Arena_t a = Arena__Alloc(1024 * 10);
  String8Node* sn = NULL;
  for (u32 i = 0; i < 32; i++) {
    sn = str8n__allocf(&a, sn, "%03d ", 5, i);
  }
  str8__fputs(sn, stdout);

  Arena__Free(&a);
}

void Test005__Test() {
  LOG_DEBUGF("Test005");
  Arena_t a = Arena__Alloc(100);
  Arena_t* sa = Arena__SubAlloc(&a, 10);

  u32* p1 = Arena__Push(sa, sizeof(u32));
  *p1 = 0x69000069;
  u32* p2 = Arena__Push(sa, sizeof(u32));
  *p2 = 0x70000070;

  Arena__Reset(sa);
  u32* p3 = Arena__Push(sa, sizeof(u32));
  *p3 = 0x71000071;

  char str[200];
  sprintf(str, "%8X %8X %8X ", *p1, *p2, *p3);
  puts(str);
  ASSERT(1 == strcmp(str, "71000071 70000070 71000071"));

  Arena__Free(&a);
}

void Test006__Test() {
  LOG_DEBUGF("Test006");

  u32 col = 0xffccbbaa;
  LOG_DEBUGF("col %08x", col);
  u8 b = (col >> 16) & 0xff;
  u8 g = (col >> 8) & 0xff;
  u8 r = (col) & 0xff;

  LOG_DEBUGF("r %08x", r);
  LOG_DEBUGF("g %08x", g);
  LOG_DEBUGF("b %08x", b);

  col = 0xff000000 | b << 16 | g << 8 | r;
  LOG_DEBUGF("col %08x", col);
}

void Test007__Test() {
  LOG_DEBUGF("Test007");

  u32 col = 0xffccbbaa;
  LOG_DEBUGF("col a %08x", col);
  col = ((u32)(col * 0.5f) & 0xff000000) | (col & 0x00ffffff);
  LOG_DEBUGF("col b %08x", col);

  col = 0xffffffff;
  LOG_DEBUGF("col c %08x", col);
  col = col * (f64)0.5f;
  LOG_DEBUGF("col d %08x", col);
}

void Test__Boot() {
  Test007__Test();
}
