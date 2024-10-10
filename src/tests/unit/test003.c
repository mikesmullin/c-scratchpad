#include "test003.h"

#include <stdbool.h>
#include <stdio.h>

#include "../../lib/Base.h"
#include "../../lib/Breakpoint.h"

#define MAX_LISTENERS 10

// Enum for event types and listeners
typedef enum { EVENT_HELLO, EVENT_GOODBYE } EventType;

typedef enum { BLOCK__HELLO, ENTITY__HELLO, CAT_ENTITY__GOODBYE, LISTENER_FN_COUNT } ListenerFnId;

// EventEmitter structure
typedef struct {
  EventType event[MAX_LISTENERS];
  ListenerFnId listeners[MAX_LISTENERS];
  int count;
} EventEmitter_t;

// Static dispatch table
static bool Block__OnHello(void* self, void* data);
static bool Entity__OnHello(void* self, void* data);
static bool CatEntity__OnGoodbye(void* self, void* data);
static bool (*VTABLE_LISTENERS[LISTENER_FN_COUNT])(void* self, void* data) = {
    Block__OnHello,  // BLOCK__HELLO
    Entity__OnHello,  // ENTITY__HELLO
    CatEntity__OnGoodbye  // CAT_ENTITY__GOODBYE
};

// Example of event handling functions (static dispatch)
static bool Block__OnHello(void* self, void* data) {
  printf("Block says Hello! Data: %s\n", (char*)data);
  return true;
}

static bool Entity__OnHello(void* self, void* data) {
  printf("Entity says Hello! Data: %s\n", (char*)data);
  return true;
}

static bool CatEntity__OnGoodbye(void* self, void* data) {
  printf("CatEntity says Goodbye! Data: %s\n", (char*)data);
  return true;
}

// Initialize the EventEmitter
static void EventEmitter__init(EventEmitter_t* emitter) {
  emitter->count = 0;
}

// Register a listener for an event using ListenerFnId
static void EventEmitter__on(EventEmitter_t* emitter, EventType event, ListenerFnId listener) {
  if (emitter->count < MAX_LISTENERS) {
    emitter->event[emitter->count] = event;
    emitter->listeners[emitter->count] = listener;
    emitter->count++;
  }
}

// Emit an event using the listener enum dispatch
// TODO: pass vtable here
static void EventEmitter__emit(EventEmitter_t* emitter, EventType event, void* self, void* data) {
  for (int i = 0; i < emitter->count; i++) {
    EventType e = emitter->event[i];
    if (e == event) {
      ListenerFnId listener = emitter->listeners[i];
      VTABLE_LISTENERS[listener](self, data);  // Dispatch event to correct listener
    }
  }
}

void Test003__Test() {
  LOG_DEBUGF("Test003 EventEmitter System Demo");

  EventEmitter_t emitter;
  EventEmitter__init(&emitter);

  // Register listeners using enum IDs
  EventEmitter__on(&emitter, EVENT_HELLO, BLOCK__HELLO);
  EventEmitter__on(&emitter, EVENT_HELLO, ENTITY__HELLO);
  EventEmitter__on(&emitter, EVENT_GOODBYE, CAT_ENTITY__GOODBYE);

  // Emit events
  EventEmitter__emit(&emitter, EVENT_HELLO, NULL, "World");
  EventEmitter__emit(&emitter, EVENT_GOODBYE, NULL, "Friend");
}
