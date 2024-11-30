#include "test006.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../lib/Base.h"
#include "../../lib/Thread.h"

#define THREAD_POOL_SIZE 10
#define LIST_SIZE 100

typedef struct {
  int* list;
  int start_index;
  int end_index;
  int thread_id;
} Task;

// Mutex for deterministic output
Mutex output_mutex;
u8 processed_count = 0;

// Worker function
static THREAD_FN_RET worker_function(THREAD_FN_PARAM1 userdata) {
  Task* task = (Task*)userdata;

  // Process the assigned portion of the list
  for (int i = task->start_index; i < task->end_index; i++) {
    task->list[i] = task->list[i] * 2;  // Example processing: double the value
  }

  // Lock output for sequential order
  Thread__Mutex_lock(&output_mutex);
  printf(
      "Thread %d processed indices %d to %d\n",
      task->thread_id,
      task->start_index,
      task->end_index);
  processed_count++;
  Thread__Mutex_unlock(&output_mutex);

  free(task);  // Free allocated task
  return THREAD_FN_RET_VAL;
}

void Test006__Test() {
  int list[LIST_SIZE];
  Thread threads[THREAD_POOL_SIZE];
  int chunk_size = LIST_SIZE / THREAD_POOL_SIZE;

  // Initialize the list with values
  for (int i = 0; i < LIST_SIZE; i++) {
    list[i] = i + 1;
  }

  // Initialize mutex
  if (!Thread__Mutex_create(&output_mutex)) {
    ASSERT_CONTEXT(false, "Failed to initialize mutex.");
  }

  // Create threads and assign tasks
  for (int i = 0; i < THREAD_POOL_SIZE; i++) {
    Task* task = (Task*)malloc(sizeof(Task));
    task->list = list;
    task->start_index = i * chunk_size;
    task->end_index = (i == THREAD_POOL_SIZE - 1) ? LIST_SIZE : (i + 1) * chunk_size;
    task->thread_id = i;

    if (!Thread__create(&threads[i], worker_function, task)) {
      ASSERT_CONTEXT(false, "Failed to create thread %d.", i);
    }
  }

  // Wait for all threads to finish
  Thread__join(threads, THREAD_POOL_SIZE);

  // Destroy mutex
  Thread__destroy(threads, THREAD_POOL_SIZE);
  Thread__Mutex_destroy(&output_mutex);

  // Output final list
  printf("Final list:\n");
  for (int i = 0; i < LIST_SIZE; i++) {
    printf("%d ", list[i]);
  }
  printf("\n");
}