#include "test001.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../lib/Base.h"
#include "../../lib/Breakpoint.h"

// Define Event types
typedef enum { EVENT_NONE, EVENT_DEATH, EVENT_ATTACKED, EVENT_ANIMOVER } EventType;

typedef struct State State;
typedef struct StateGraph StateGraph;

typedef void (*StateFunction)(StateGraph* sg);

// Define the structure for State
struct State {
  const char* name;
  // TODO: tags set in each state are what allow states to intelligently transit
  // const char* tags[];
  // bool State__HasTag(const char* tag);
  StateFunction onEnter;
  StateFunction onUpdate;
  StateFunction onExit;
  // TODO: event listeners can also transit state
  // TODO: Event instances contain closured/unique fn logic
  // Event events[];
  // TODO: each state has its own Animator state, sending events, tracking anim frames, etc.
  // TODO: anim timers/onFrameN can also transit state (like Flash keyframes)
  // Timer timeline[];
  struct State* nextState;  // Next state to transition to
};

// Define the structure for StateGraph
struct StateGraph {
  State* currentState;
  EventType currentEvent;  // Event sent to the StateGraph
};

// Function prototypes for state functions
static void idleoutEnter(StateGraph* sg);
static void idleoutUpdate(StateGraph* sg);
static void idleoutExit(StateGraph* sg);
static void idleinEnter(StateGraph* sg);
static void idleinUpdate(StateGraph* sg);
static void idleinExit(StateGraph* sg);
static void emergeEnter(StateGraph* sg);
static void emergeUpdate(StateGraph* sg);
static void emergeExit(StateGraph* sg);
static void hibernateEnter(StateGraph* sg);
static void hibernateUpdate(StateGraph* sg);
static void hibernateExit(StateGraph* sg);

// Define states
static State idleoutState, idleinState, emergeState, hibernateState;

// State definitions with transitions
static State idleoutState = {"IdleOut", idleoutEnter, idleoutUpdate, idleoutExit, &hibernateState};
static State idleinState = {"IdleIn", idleinEnter, idleinUpdate, idleinExit, &emergeState};
static State emergeState = {"Emerge", emergeEnter, emergeUpdate, emergeExit, &idleinState};
static State hibernateState = {
    "Hibernate", hibernateEnter, hibernateUpdate, hibernateExit, &idleoutState};

// StateGraph initialization
// TODO: accept params like (name, states, events, initialState)
static StateGraph* createStateGraph() {
  StateGraph* sg = (StateGraph*)malloc(sizeof(StateGraph));
  sg->currentState = &idleoutState;  // Start in the idleout state
  sg->currentEvent = EVENT_NONE;  // No event initially
  return sg;
}

// Event handling functions
static void sendEvent(StateGraph* sg, EventType event) {
  sg->currentEvent = event;
}

// Check for specific events and handle state transitions
static void handleEvents(StateGraph* sg) {
  switch (sg->currentEvent) {
    case EVENT_DEATH:
      printf("Handled 'death' event.\n");
      // Transition to hibernate immediately on death
      sg->currentState->onExit(sg);
      sg->currentState = &hibernateState;
      sg->currentState->onEnter(sg);
      break;
    case EVENT_ATTACKED:
      printf("Handled 'attacked' event.\n");
      // Transition to emerge when attacked
      sg->currentState->onExit(sg);
      sg->currentState = &emergeState;
      sg->currentState->onEnter(sg);
      break;
    case EVENT_ANIMOVER:
      printf("Handled 'animover' event.\n");
      // Use animover to move to the next state
      sg->currentState->onExit(sg);
      sg->currentState = sg->currentState->nextState;
      sg->currentState->onEnter(sg);
      break;
    default:
      break;
  }
  sg->currentEvent = EVENT_NONE;  // Reset the event after handling
}

// State functions implementation
static void idleoutEnter(StateGraph* sg) {
  printf("Entering IdleOut State.\n");
}

static void idleoutUpdate(StateGraph* sg) {
  printf("Update IdleOut State.\n");
  // For this example, after one update, we'll simulate animation over

  // TODO: the method which triggers play for all animation,
  //       is what sends out the event (when its actually done)
  // inst.AnimState:PlayAnimation("pick")
  sendEvent(sg, EVENT_ANIMOVER);
  // TODO: but states can trigger other (incl. custom events) (ie. "freshspawn")
}

static void idleoutExit(StateGraph* sg) {
  printf("Exiting IdleOut State.\n");
}

static void idleinEnter(StateGraph* sg) {
  printf("Entering IdleIn State.\n");
}

static void idleinUpdate(StateGraph* sg) {
  printf("Update IdleIn State.\n");
  if (rand() % 10 < 2) {  // 20% chance to be attacked
    sendEvent(sg, EVENT_ATTACKED);
  } else {
    sendEvent(sg, EVENT_ANIMOVER);
  }
}

static void idleinExit(StateGraph* sg) {
  printf("Exiting IdleIn State.\n");
}

static void emergeEnter(StateGraph* sg) {
  printf("Entering Emerge State.\n");
}

static void emergeUpdate(StateGraph* sg) {
  printf("Update Emerge State.\n");
  sendEvent(sg, EVENT_ANIMOVER);  // After emerge, transition to idlein
}

static void emergeExit(StateGraph* sg) {
  printf("Exiting Emerge State.\n");
}

static void hibernateEnter(StateGraph* sg) {
  printf("Entering Hibernate State.\n");
}

static void hibernateUpdate(StateGraph* sg) {
  printf("Update Hibernate State.\n");
  if (rand() % 10 < 3) {  // 30% chance to come out of hibernation
    sendEvent(sg, EVENT_ANIMOVER);
  }
}

static void hibernateExit(StateGraph* sg) {
  printf("Exiting Hibernate State.\n");
}

// Main simulation loop
static void runStateGraph(StateGraph* sg) {
  for (int i = 0; i < 20; i++) {
    printf("--TICK %d--\n", i);
    sg->currentState->onUpdate(sg);
    handleEvents(sg);  // Handle events and trigger transitions
  }
}

void Test001__Test() {
  LOG_DEBUGF("Test001 StateGraph System Demo");
  StateGraph* sg = createStateGraph();
  sg->currentState->onEnter(sg);  // Enter the initial state

  // Run the state graph simulation (in reality, this could be part of a game loop)
  runStateGraph(sg);

  // Free allocated memory
  free(sg);
}
