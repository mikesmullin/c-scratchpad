#ifndef HASHMAP_H
#define HASHMAP_H

#include "Base.h"

// Structure for each key-value pair (entry)
typedef struct Entry {
  char* key;
  char* value;
  struct Entry* next;  // For handling collisions via chaining
} Entry;

// Structure for the Hashmap
typedef struct HashMap {
  Entry** entries;
} HashMap;

unsigned int hash(const char* key);
Entry* create_entry(const char* key, const char* value);
HashMap* create_hashmap();
void hashmap_insert(HashMap* hashmap, const char* key, const char* value);
char* hashmap_get(HashMap* hashmap, const char* key);
void free_hashmap(HashMap* hashmap);

#endif  // HASHMAP_H