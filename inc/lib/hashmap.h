// Copyright 2020 Joshua J Baker. All rights reserved.
// Copyright 2023 Marvin Borner
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#ifndef SHARING_HASHMAP_H
#define SHARING_HASHMAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct hashmap;

struct hashmap *hashmap_new(size_t elsize, size_t cap,
			    void (*elfree)(void *item));

void hashmap_free(struct hashmap *map);
void hashmap_clear(struct hashmap *map, bool update_cap);
size_t hashmap_count(struct hashmap *map);
bool hashmap_oom(struct hashmap *map);
void *hashmap_probe(struct hashmap *map, uint64_t position);
bool hashmap_scan(struct hashmap *map, bool (*iter)(void *item));
bool hashmap_iter(struct hashmap *map, size_t *i, void **item);

void *hashmap_get(struct hashmap *map, uint64_t hash);
void *hashmap_delete(struct hashmap *map, uint64_t hash);
void *hashmap_set(struct hashmap *map, void *item, uint64_t hash);
void hashmap_set_grow_by_power(struct hashmap *map, size_t power);

#endif
