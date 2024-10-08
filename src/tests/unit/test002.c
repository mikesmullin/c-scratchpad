#include "test002.h"

#include <stdio.h>

#include "../../lib/Base.h"
#include "../../lib/BehaviorTree.h"
#include "../../lib/Breakpoint.h"

// -- PigEntity --

// Pig's brain states
// TODO: Only use BTStatus, otherwise use component state
typedef enum { PIG_IDLE, PIG_PANIC, PIG_FOLLOW_LEADER } PigState;

typedef struct {
  PigState state;  // Current state of the pig
  bool isThreatNear;  // Flag to indicate if a threat is nearby
  bool isLeaderNear;  // Flag to indicate if a leader is nearby
} Pig;

// #metacode
// #macro MACRO1(T1)
//   {{~#for T1~}}
//   // --- {{this.nn}}Node ---
//   typedef struct {
//     BTNode base;
//     {{this.e}};
//   } {{this.nn}}Node;
//   BTStatus {{this.nn}}Tick({{this.nn}}Node* node) {
//     {{this.e}} = node->{{this.v}};
//     printf("{{this.msg}}\n");
//     if ({{this.v}}->{{this.c1}}) {
//       return {{this.o1}};  // {{this.ex1}}
//     }
//     return {{this.o2}}; // {{this.ex2}}
//   }
//   {{this.nn}}Node* Create{{this.nn}}Node({{this.e}}) {
//     {{this.nn}}Node* node = malloc(sizeof({{this.nn}}Node));
//     node->{{this.v}} = {{this.v}};
//     node->base.tick = (BTStatus(*)(BTNode*)){{this.nn}}Tick;
//     return node;
//   }
//   {{~/for~}}
//
// #table T_VARS
//   nn              | e        | v   | msg                            | c1           | o1         | ex1                                                      | o2         | ex2                                           |
//   PigIdle         | Pig* pig | pig | Pig is idling...               | isThreatNear | BT_FAILURE | Fail and switch to another state if a threat is detected | BT_SUCCESS |                                               |
//   PigPanic        | Pig* pig | pig | Pig is panicking!              | isThreatNear | BT_RUNNING | Keep running in panic mode if the threat is still nearby | BT_SUCCESS | Calm down if the threat is gone               |
//   PigFollowLeader | Pig* pig | pig | Pig is following the leader... | isLeaderNear | BT_RUNNING | Continue following the leader                            | BT_FAILURE | If leader is gone, fail and pick a new action |
//
// MACRO1(T_VARS)
// #metagen

// --- PigIdleNode ---
typedef struct {
  BTNode base;
  Pig* pig;
} PigIdleNode;

static BTStatus PigIdleTick(PigIdleNode* node) {
  Pig* pig = node->pig;
  // printf("Pig is considering idling...\n");
  // if (!pig->isThreatNear) {
  printf("Pig is idling...\n");
  pig->state = PIG_IDLE;
  return BT_RUNNING;  // Fail and switch to another state if a threat is detected
  // }
  // return BT_SUCCESS;  // We're the last node, so don't try to do anything else
}

// TODO: maybe easier to not define all the Create*() fns, and just manually init structs
static PigIdleNode* CreatePigIdleNode(Pig* pig) {
  PigIdleNode* node = malloc(sizeof(PigIdleNode));
  node->pig = pig;
  node->base.tick = (BTStatus(*)(BTNode*))PigIdleTick;
  return node;
}

// --- PigPanicNode ---
typedef struct {
  BTNode base;
  Pig* pig;
} PigPanicNode;

static BTStatus PigPanicTick(PigPanicNode* node) {
  Pig* pig = node->pig;
  printf("Pig is thinking about panicking...\n");
  if (pig->isThreatNear) {
    printf("Pig is panicking!\n");
    pig->state = PIG_PANIC;
    return BT_RUNNING;  // Keep running in panic mode if the threat is still nearby
  }
  return BT_FAILURE;  // Do other things if the threat is gone
}

static PigPanicNode* CreatePigPanicNode(Pig* pig) {
  PigPanicNode* node = malloc(sizeof(PigPanicNode));
  node->pig = pig;
  node->base.tick = (BTStatus(*)(BTNode*))PigPanicTick;
  return node;
}

// --- PigFollowLeaderNode ---
typedef struct {
  BTNode base;
  Pig* pig;
} PigFollowLeaderNode;

static BTStatus PigFollowLeaderTick(PigFollowLeaderNode* node) {
  Pig* pig = node->pig;
  printf("Pig is considering following a leader...\n");
  if (pig->isLeaderNear) {
    printf("Pig is following the leader!\n");
    pig->state = PIG_FOLLOW_LEADER;
    return BT_RUNNING;  // Continue following the leader
  }
  return BT_FAILURE;  // Pick a new action, while leader is gone
}

static PigFollowLeaderNode* CreatePigFollowLeaderNode(Pig* pig) {
  PigFollowLeaderNode* node = malloc(sizeof(PigFollowLeaderNode));
  node->pig = pig;
  node->base.tick = (BTStatus(*)(BTNode*))PigFollowLeaderTick;
  return node;
}
// #metaend

// Simulate the Behavior Tree running for the pig
static void RunPigBrain(Pig* pig) {
  // Create leaf nodes for each behavior
  PigIdleNode* idle = CreatePigIdleNode(pig);
  PigPanicNode* panic = CreatePigPanicNode(pig);
  PigFollowLeaderNode* followLeader = CreatePigFollowLeaderNode(pig);

  // Create the selector node with behaviors
  BTNode* behaviors[] = {(BTNode*)panic, (BTNode*)followLeader, (BTNode*)idle};
  SelectorNode* root = CreateSelectorNode(behaviors, 3);

  // Run the behavior tree in a loop (for demonstration)
  for (int i = 0; i < 10; i++) {
    printf("---- TICK %d ----\n", i);
    BTStatus result = root->base.tick((BTNode*)root);

    if (result == BT_SUCCESS) {
      printf("Behavior completed successfully!\n");
    } else if (result == BT_RUNNING) {
      printf("Behavior is running...\n");
    } else {
      printf("Behavior failed!\n");
    }

    // Simulate changing world conditions
    pig->isThreatNear = (i == 3 || i == 4);  // Simulate a threat near at ticks 3 and 4
    pig->isLeaderNear = (i > 6);  // Simulate leader being near after tick 6
  }

  // Cleanup
  free(idle);
  free(panic);
  free(followLeader);
  free(root);
}

void Test002__Test() {
  LOG_DEBUGF("Test002 BehaviorTree System Demo");
  Pig pig = {PIG_IDLE, false, false};  // Initialize pig with default state (idle)
  RunPigBrain(&pig);  // Run the Pig's brain using the behavior tree
}
