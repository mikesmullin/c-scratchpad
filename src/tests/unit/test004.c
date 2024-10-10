#include "test004.h"

#include <stdbool.h>
#include <stdio.h>

#include "../../lib/Arena.h"
#include "../../lib/Base.h"
#include "../../lib/Breakpoint.h"

typedef enum EntityTags1_t : u64 {
  TAG_NONE = 0,
  TAG_FARM_PLOT = 1 << 1,  //
  TAG_PROJECTILE = 1 << 2,  //
  TAG_HEAVY = 1 << 3,  //
  TAG_EDIBLE = 1 << 4,  //
  TAG_WALL_BUILDER = 1 << 5,  //
  TAG_WATER_SOURCE = 1 << 6,  //
} EntityTags1;

typedef struct Entity_t {
  u64 tags1;
} Entity;

void Test004__Test() {
  LOG_DEBUGF("Test004 Tag System Demo");

  Entity e = {};
  e.tags1 |= TAG_HEAVY;
  e.tags1 |= TAG_WATER_SOURCE;

  if (e.tags1 & TAG_HEAVY) LOG_DEBUGF("heavy");
  if (e.tags1 & TAG_WATER_SOURCE) LOG_DEBUGF("water src");
  if (e.tags1 & TAG_PROJECTILE) LOG_DEBUGF("projectile");
}
