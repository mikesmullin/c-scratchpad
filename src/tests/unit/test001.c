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
void idleoutEnter(StateGraph* sg);
void idleoutUpdate(StateGraph* sg);
void idleoutExit(StateGraph* sg);
void idleinEnter(StateGraph* sg);
void idleinUpdate(StateGraph* sg);
void idleinExit(StateGraph* sg);
void emergeEnter(StateGraph* sg);
void emergeUpdate(StateGraph* sg);
void emergeExit(StateGraph* sg);
void hibernateEnter(StateGraph* sg);
void hibernateUpdate(StateGraph* sg);
void hibernateExit(StateGraph* sg);

// Define states
State idleoutState, idleinState, emergeState, hibernateState;

// State definitions with transitions
State idleoutState = {"IdleOut", idleoutEnter, idleoutUpdate, idleoutExit, &hibernateState};
State idleinState = {"IdleIn", idleinEnter, idleinUpdate, idleinExit, &emergeState};
State emergeState = {"Emerge", emergeEnter, emergeUpdate, emergeExit, &idleinState};
State hibernateState = {"Hibernate", hibernateEnter, hibernateUpdate, hibernateExit, &idleoutState};

// StateGraph initialization
// TODO: accept params like (name, states, events, initialState)
StateGraph* createStateGraph() {
  StateGraph* sg = (StateGraph*)malloc(sizeof(StateGraph));
  sg->currentState = &idleoutState;  // Start in the idleout state
  sg->currentEvent = EVENT_NONE;  // No event initially
  return sg;
}

// Event handling functions
void sendEvent(StateGraph* sg, EventType event) {
  sg->currentEvent = event;
}

// Check for specific events and handle state transitions
void handleEvents(StateGraph* sg) {
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
void idleoutEnter(StateGraph* sg) {
  printf("Entering IdleOut State.\n");
}

void idleoutUpdate(StateGraph* sg) {
  printf("Update IdleOut State.\n");
  // For this example, after one update, we'll simulate animation over

  // TODO: the method which triggers play for all animation,
  //       is what sends out the event (when its actually done)
  // inst.AnimState:PlayAnimation("pick")
  sendEvent(sg, EVENT_ANIMOVER);
  // TODO: but states can trigger other (incl. custom events) (ie. "freshspawn")
}

void idleoutExit(StateGraph* sg) {
  printf("Exiting IdleOut State.\n");
}

void idleinEnter(StateGraph* sg) {
  printf("Entering IdleIn State.\n");
}

void idleinUpdate(StateGraph* sg) {
  printf("Update IdleIn State.\n");
  if (rand() % 10 < 2) {  // 20% chance to be attacked
    sendEvent(sg, EVENT_ATTACKED);
  } else {
    sendEvent(sg, EVENT_ANIMOVER);
  }
}

void idleinExit(StateGraph* sg) {
  printf("Exiting IdleIn State.\n");
}

void emergeEnter(StateGraph* sg) {
  printf("Entering Emerge State.\n");
}

void emergeUpdate(StateGraph* sg) {
  printf("Update Emerge State.\n");
  sendEvent(sg, EVENT_ANIMOVER);  // After emerge, transition to idlein
}

void emergeExit(StateGraph* sg) {
  printf("Exiting Emerge State.\n");
}

void hibernateEnter(StateGraph* sg) {
  printf("Entering Hibernate State.\n");
}

void hibernateUpdate(StateGraph* sg) {
  printf("Update Hibernate State.\n");
  if (rand() % 10 < 3) {  // 30% chance to come out of hibernation
    sendEvent(sg, EVENT_ANIMOVER);
  }
}

void hibernateExit(StateGraph* sg) {
  printf("Exiting Hibernate State.\n");
}

// Main simulation loop
void runStateGraph(StateGraph* sg) {
  for (int i = 0; i < 20; i++) {
    printf("--TICK %d--\n", i);
    sg->currentState->onUpdate(sg);
    handleEvents(sg);  // Handle events and trigger transitions
  }
}

static void Test001__Test() {
  LOG_DEBUGF("Test001 StateGraph System Demo");
  StateGraph* sg = createStateGraph();
  sg->currentState->onEnter(sg);  // Enter the initial state

  // Run the state graph simulation (in reality, this could be part of a game loop)
  runStateGraph(sg);

  // Free allocated memory
  free(sg);
}

void Test__Boot() {
  Test001__Test();
}
