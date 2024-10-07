#pragma once

#include <stdbool.h>
#include <time.h>

// Define return statuses for behavior tree nodes
typedef enum { BT_SUCCESS, BT_FAILURE, BT_RUNNING } BTStatus;

// Basic structure for a behavior tree node
typedef struct BTNode {
  BTStatus (*tick)(struct BTNode* node);  // Function pointer to the behavior's tick function
} BTNode;

// This set of nodes provides a wide variety of behaviors and
// control flow for a behavior tree.

// 1. DecoratorNode

// This node is a basic node that wraps a single child,
// primarily used for adding additional behavior to the child.

typedef struct {
  BTNode base;
  BTNode* child;
} DecoratorNode;

BTStatus DecoratorTick(DecoratorNode* node);

// TODO: Constructor arguments: name, child
DecoratorNode* CreateDecoratorNode(BTNode* child);

// 2. NotDecorator

// This node inverts the result of its child (i.e., success becomes failure, and vice versa).

typedef struct {
  BTNode base;
  BTNode* child;
} NotDecorator;

// TODO: Constructor arguments: name, child
BTStatus NotDecoratorTick(NotDecorator* node);

NotDecorator* CreateNotDecorator(BTNode* child);

// 3. FailIfRunningDecorator

// This node behaves like DecoratorNode but fails if its child is running.

typedef struct {
  BTNode base;
  BTNode* child;
} FailIfRunningDecorator;

BTStatus FailIfRunningTick(FailIfRunningDecorator* node);

// TODO: Constructor arguments: name, child
FailIfRunningDecorator* CreateFailIfRunningDecorator(BTNode* child);

// 4. ConditionNode

// This node checks a condition function,
// succeeding if the condition is true, and failing otherwise.

typedef struct {
  BTNode base;
  bool (*condition)();
} ConditionNode;

BTStatus ConditionTick(ConditionNode* node);

// TODO: Constructor arguments: fn, name
ConditionNode* CreateConditionNode(bool (*condition)());

// 5. ConditionWaitNode

// This node runs until the condition function returns true.
// It's similar to ConditionNode, but it returns BT_RUNNING
// instead of BT_FAILURE when the condition is false.

typedef struct {
  BTNode base;
  bool (*condition)();
} ConditionWaitNode;

BTStatus ConditionWaitTick(ConditionWaitNode* node);

// TODO: Const. arg.: fn, name
ConditionWaitNode* CreateConditionWaitNode(bool (*condition)());

// 6. ActionNode

// This node executes an action and always succeeds after running it.

typedef struct {
  BTNode base;
  void (*action)();
} ActionNode;

BTStatus ActionTick(ActionNode* node);

// TODO: Const. arg.: action, name
ActionNode* CreateActionNode(void (*action)());

// 7. WaitNode

// This node waits for a specified amount of time before succeeding.

typedef struct {
  BTNode base;
  double waitTime;
  time_t startTime;
} WaitNode;

BTStatus WaitTick(WaitNode* node);

WaitNode* CreateWaitNode(double timeInSeconds);

// 8. SequenceNode

// This node goes through its children in order, succeeding if all succeed.

typedef struct {
  BTNode base;
  BTNode** children;
  int childCount;
  int currentChild;
} SequenceNode;

BTStatus SequenceTick(SequenceNode* node);

// TODO: Const. arg.: children
SequenceNode* CreateSequenceNode(BTNode** children, int count);

// 9. SelectorNode

// This node is similar to SequenceNode but succeeds
// as soon as one child succeeds.

typedef struct {
  BTNode base;
  BTNode** children;
  int childCount;
  int currentChild;
} SelectorNode;

BTStatus SelectorTick(SelectorNode* node);

// TODO: Const. arg.: children
SelectorNode* CreateSelectorNode(BTNode** children, int count);

// 10. LoopNode

// This node runs its children multiple times (up to maxreps).

typedef struct {
  BTNode base;
  BTNode** children;
  int childCount;
  int maxReps;
  int currentRep;
  int currentChild;
} LoopNode;

BTStatus LoopTick(LoopNode* node);

// TODO: Const. arg.: children, maxreps
LoopNode* CreateLoopNode(BTNode** children, int count, int maxReps);

// 11. RandomNode
// The RandomNode picks a random child to execute.
// If it fails, it will try the next child
// until all children have been executed.

typedef struct {
  BTNode base;
  BTNode** children;
  int childCount;
  int currentChild;
} RandomNode;

BTStatus RandomTick(RandomNode* node);

// TODO: Const. arg.: children
RandomNode* CreateRandomNode(BTNode** children, int count);

// 12. ParallelNode
// The ParallelNode runs all its children simultaneously and
// succeeds if all children complete successfully.
// It fails if any child fails.

typedef struct {
  BTNode base;
  BTNode** children;
  int childCount;
} ParallelNode;

BTStatus ParallelTick(ParallelNode* node);

// TODO: Const. arg.: children, name
ParallelNode* CreateParallelNode(BTNode** children, int count);

// 13. ParallelNodeAny

// The ParallelNodeAny is similar to ParallelNode,
// but it succeeds if any child stops running without failing.

typedef struct {
  BTNode base;
  BTNode** children;
  int childCount;
} ParallelNodeAny;

BTStatus ParallelAnyTick(ParallelNodeAny* node);

// TODO: Const. arg.: children, name
ParallelNodeAny* CreateParallelNodeAny(BTNode** children, int count);

// 14. IfNode

// The IfNode checks a condition before executing its child node.
// If the condition is true, it runs the child.

typedef struct {
  BTNode base;
  bool (*condition)();
  BTNode* child;
} IfNode;

BTStatus IfTick(IfNode* node);

// TODO: Const. arg.: cond, name, node
IfNode* CreateIfNode(bool (*condition)(), BTNode* child);

// 15. PriorityNode

// The PriorityNode runs its children in order
// but only executes when a specified period has passed.
// If a child succeeds or is still running, it retains that state.

// PriorityNode allows you to execute nodes based on timing,
// creating a cooldown effect before the next execution.

// TODO: add Sleep() method which can accumulate (+=) parent priorityNode period

typedef struct {
  BTNode base;
  BTNode** children;
  int childCount;
  float period;
  float lastExecutionTime;
  float currentTime;  // Current time for the node (should be updated externally)
} PriorityNode;

BTStatus PriorityTick(PriorityNode* node);

// TODO: Const. arg.: children, period
PriorityNode* CreatePriorityNode(BTNode** children, int count, float period);

// 16. WhileNode

// The WhileNode runs its child node
// as long as a specified condition is true.
// It effectively stops when the condition returns false.

// WhileNode allows for repeated execution of a child node
// based on a dynamic condition, making it useful for
// looping behaviors until a condition changes.

typedef struct {
  BTNode base;
  bool (*condition)();
  BTNode* child;
} WhileNode;

BTStatus WhileTick(WhileNode* node);

// TODO: Const. arg.: cond, name, node
WhileNode* CreateWhileNode(bool (*condition)(), BTNode* child);

// TODO: EventNode listens, wakes parent priority node, visits child
// TODO: EventLoop impl
