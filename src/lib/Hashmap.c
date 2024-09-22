#include "Hashmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

// Simple hash function (djb2 by Dan Bernstein)
unsigned int hash(const char* key) {
  unsigned long hash = 5381;
  int c;
  while ((c = *key++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash % TABLE_SIZE;
}

// Create a new entry
Entry* create_entry(const char* key, const char* value) {
  Entry* entry = malloc(sizeof(Entry));
  entry->key = _strdup(key);      // Duplicate the key
  entry->value = _strdup(value);  // Duplicate the value
  entry->next = NULL;             // No collision yet
  return entry;
}

// Create a new hashmap
HashMap* create_hashmap() {
  HashMap* hashmap = malloc(sizeof(HashMap));
  hashmap->entries = malloc(TABLE_SIZE * sizeof(Entry*));
  for (int i = 0; i < TABLE_SIZE; i++) {
    hashmap->entries[i] = NULL;  // Initialize all entries to NULL
  }
  return hashmap;
}

// Insert a key-value pair into the hashmap
void hashmap_insert(HashMap* hashmap, const char* key, const char* value) {
  unsigned int slot = hash(key);
  Entry* entry = hashmap->entries[slot];

  if (entry == NULL) {
    // No collision, create a new entry
    hashmap->entries[slot] = create_entry(key, value);
  } else {
    // Collision handling with chaining (linked list)
    Entry* prev = NULL;
    while (entry != NULL) {
      // If key already exists, update the value
      if (strcmp(entry->key, key) == 0) {
        free(entry->value);             // Free old value
        entry->value = _strdup(value);  // Set new value
        return;
      }
      prev = entry;
      entry = entry->next;
    }
    // Add new entry to the chain
    prev->next = create_entry(key, value);
  }
}

// Retrieve a value by key
char* hashmap_get(HashMap* hashmap, const char* key) {
  unsigned int slot = hash(key);
  Entry* entry = hashmap->entries[slot];

  while (entry != NULL) {
    if (strcmp(entry->key, key) == 0) {
      return entry->value;
    }
    entry = entry->next;  // Move to the next entry in case of a collision
  }

  return NULL;  // Key not found
}

// Free the hashmap and its contents
void free_hashmap(HashMap* hashmap) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    Entry* entry = hashmap->entries[i];
    while (entry != NULL) {
      Entry* temp = entry;
      entry = entry->next;
      free(temp->key);
      free(temp->value);
      free(temp);
    }
  }
  free(hashmap->entries);
  free(hashmap);
}
