#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAP_SIZE 256  // fixed size, since key is uint8_t (0-255), you can map all keys if you want

typedef struct {
    bool occupied;
    uint8_t key;
    uint8_t value;
} HashEntry;

typedef struct {
    HashEntry entries[MAP_SIZE];
} HashMap;

// Simple hash: identity hash for uint8_t keys
static inline uint8_t hash(uint8_t key) {
    return key; // identity hash is fine since keys are uint8_t
}

void hashmap_init(HashMap* map) {
    memset(map->entries, 0, sizeof(map->entries));
}

// Insert or update key-value pair
void hashmap_put(HashMap* map, uint8_t key, uint8_t value) {
    uint8_t idx = hash(key);
    for (int i = 0; i < MAP_SIZE; i++) {
        uint8_t entry = (idx + i) % MAP_SIZE;
        if (!map->entries[entry].occupied || map->entries[entry].key == key) {
            map->entries[entry].occupied = true;
            map->entries[entry].key = key;
            map->entries[entry].value = value;
            return;
        }
    }
    // Map full (unlikely for 256 size), handle as needed
    fprintf(stderr, "HashMap full, cannot insert key %d\n", key);
}

// Returns true if found, and outputs value to *out_value
bool hashmap_get(HashMap* map, uint8_t key, uint8_t* out_value) {
    uint8_t idx = hash(key);
    for (int i = 0; i < MAP_SIZE; i++) {
        uint8_t entry = (idx + i) % MAP_SIZE;
        if (!map->entries[entry].occupied) {
            return false; // not found
        }
        if (map->entries[entry].key == key) {
            *out_value = map->entries[entry].value;
            return true;
        }
    }
    return false;
}

// Example usage
/*
int main() {
    HashMap map;
    hashmap_init(&map);

    hashmap_put(&map, 10, 42);
    hashmap_put(&map, 255, 99);

    uint8_t val;
    if (hashmap_get(&map, 10, &val)) {
        printf("Key 10 => Value %d\n", val);
    } else {
        printf("Key 10 not found\n");
    }

    if (hashmap_get(&map, 255, &val)) {
        printf("Key 255 => Value %d\n", val);
    } else {
        printf("Key 255 not found\n");
    }

    if (hashmap_get(&map, 100, &val)) {
        printf("Key 100 => Value %d\n", val);
    } else {
        printf("Key 100 not found\n");
    }

    return 0;
}
*/

#endif