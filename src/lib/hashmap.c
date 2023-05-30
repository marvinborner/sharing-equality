// Copyright 2020 Joshua J Baker. All rights reserved.
// Copyright 2023 Marvin Borner
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

// This is a fork of tidwall's hashmap. It's heavily reduced and adapted.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include <lib/hashmap.h>
#include <lib/hash.h>

#define GROW_AT 0.60
#define SHRINK_AT 0.10

struct bucket {
	uint64_t hash : 48;
	uint64_t dib : 16;
};

struct hashmap {
	size_t elsize;
	size_t cap;
	void (*elfree)(void *item);
	size_t bucketsz;
	size_t nbuckets;
	size_t count;
	size_t mask;
	size_t growat;
	size_t shrinkat;
	uint8_t growpower;
	bool oom;
	void *buckets;
	void *spare;
	void *edata;
};

void hashmap_set_grow_by_power(struct hashmap *map, size_t power)
{
	map->growpower = power < 1 ? 1 : power > 16 ? 16 : power;
}

static struct bucket *bucket_at0(void *buckets, size_t bucketsz, size_t i)
{
	return (struct bucket *)(((char *)buckets) + (bucketsz * i));
}

static struct bucket *bucket_at(struct hashmap *map, size_t index)
{
	return bucket_at0(map->buckets, map->bucketsz, index);
}

static void *bucket_item(struct bucket *entry)
{
	return ((char *)entry) + sizeof(struct bucket);
}

static uint64_t clip_hash(uint64_t hash)
{
	return hash & 0xFFFFFFFFFFFF;
}

struct hashmap *hashmap_new(size_t elsize, size_t cap,
			    void (*elfree)(void *item))
{
	size_t ncap = 16;
	if (cap < ncap) {
		cap = ncap;
	} else {
		while (ncap < cap) {
			ncap *= 2;
		}
		cap = ncap;
	}
	size_t bucketsz = sizeof(struct bucket) + elsize;
	while (bucketsz & (sizeof(uintptr_t) - 1)) {
		bucketsz++;
	}
	// hashmap + spare + edata
	size_t size = sizeof(struct hashmap) + bucketsz * 2;
	struct hashmap *map = malloc(size);
	if (!map) {
		return NULL;
	}
	memset(map, 0, sizeof(struct hashmap));
	map->elsize = elsize;
	map->bucketsz = bucketsz;
	map->elfree = elfree;
	map->spare = ((char *)map) + sizeof(struct hashmap);
	map->edata = (char *)map->spare + bucketsz;
	map->cap = cap;
	map->nbuckets = cap;
	map->mask = map->nbuckets - 1;
	map->buckets = malloc(map->bucketsz * map->nbuckets);
	if (!map->buckets) {
		free(map);
		return NULL;
	}
	memset(map->buckets, 0, map->bucketsz * map->nbuckets);
	map->growpower = 1;
	map->growat = map->nbuckets * GROW_AT;
	map->shrinkat = map->nbuckets * SHRINK_AT;
	return map;
}

static void free_elements(struct hashmap *map)
{
	if (!map->elfree)
		return;

	for (size_t i = 0; i < map->nbuckets; i++) {
		struct bucket *bucket = bucket_at(map, i);
		if (bucket->dib)
			map->elfree(bucket_item(bucket));
	}
}

void hashmap_clear(struct hashmap *map, bool update_cap)
{
	map->count = 0;
	free_elements(map);
	if (update_cap) {
		map->cap = map->nbuckets;
	} else if (map->nbuckets != map->cap) {
		void *new_buckets = malloc(map->bucketsz * map->cap);
		if (new_buckets) {
			free(map->buckets);
			map->buckets = new_buckets;
		}
		map->nbuckets = map->cap;
	}
	memset(map->buckets, 0, map->bucketsz * map->nbuckets);
	map->mask = map->nbuckets - 1;
	map->growat = map->nbuckets * 0.75;
	map->shrinkat = map->nbuckets * 0.10;
}

static bool resize0(struct hashmap *map, size_t new_cap)
{
	struct hashmap *map2 = hashmap_new(map->elsize, new_cap, map->elfree);
	if (!map2)
		return false;
	for (size_t i = 0; i < map->nbuckets; i++) {
		struct bucket *entry = bucket_at(map, i);
		if (!entry->dib) {
			continue;
		}
		entry->dib = 1;
		size_t j = entry->hash & map2->mask;
		while (1) {
			struct bucket *bucket = bucket_at(map2, j);
			if (bucket->dib == 0) {
				memcpy(bucket, entry, map->bucketsz);
				break;
			}
			if (bucket->dib < entry->dib) {
				memcpy(map2->spare, bucket, map->bucketsz);
				memcpy(bucket, entry, map->bucketsz);
				memcpy(entry, map2->spare, map->bucketsz);
			}
			j = (j + 1) & map2->mask;
			entry->dib += 1;
		}
	}
	free(map->buckets);
	map->buckets = map2->buckets;
	map->nbuckets = map2->nbuckets;
	map->mask = map2->mask;
	map->growat = map2->growat;
	map->shrinkat = map2->shrinkat;
	free(map2);
	return true;
}

static bool resize(struct hashmap *map, size_t new_cap)
{
	return resize0(map, new_cap);
}

void *hashmap_set(struct hashmap *map, void *item, uint64_t hash)
{
	hash = clip_hash(hash);
	map->oom = false;
	if (map->count == map->growat) {
		if (!resize(map, map->nbuckets * (1 << map->growpower))) {
			map->oom = true;
			return NULL;
		}
	}

	struct bucket *entry = map->edata;
	entry->hash = hash;
	entry->dib = 1;
	void *eitem = bucket_item(entry);
	memcpy(eitem, item, map->elsize);

	void *bitem;
	size_t i = entry->hash & map->mask;
	while (1) {
		struct bucket *bucket = bucket_at(map, i);
		if (bucket->dib == 0) {
			memcpy(bucket, entry, map->bucketsz);
			map->count++;
			return NULL;
		}
		bitem = bucket_item(bucket);
		if (entry->hash == bucket->hash) {
			memcpy(map->spare, bitem, map->elsize);
			memcpy(bitem, eitem, map->elsize);
			return map->spare;
		}
		if (bucket->dib < entry->dib) {
			memcpy(map->spare, bucket, map->bucketsz);
			memcpy(bucket, entry, map->bucketsz);
			memcpy(entry, map->spare, map->bucketsz);
			eitem = bucket_item(entry);
		}
		i = (i + 1) & map->mask;
		entry->dib += 1;
	}
}

void *hashmap_get(struct hashmap *map, uint64_t hash)
{
	hash = clip_hash(hash);
	size_t i = hash & map->mask;
	while (1) {
		struct bucket *bucket = bucket_at(map, i);
		if (!bucket->dib)
			return NULL;
		if (bucket->hash == hash)
			return bucket_item(bucket);
		i = (i + 1) & map->mask;
	}
}

void *hashmap_probe(struct hashmap *map, uint64_t position)
{
	size_t i = position & map->mask;
	struct bucket *bucket = bucket_at(map, i);
	if (!bucket->dib) {
		return NULL;
	}
	return bucket_item(bucket);
}

void *hashmap_delete(struct hashmap *map, uint64_t hash)
{
	hash = clip_hash(hash);
	map->oom = false;
	size_t i = hash & map->mask;
	while (1) {
		struct bucket *bucket = bucket_at(map, i);
		if (!bucket->dib)
			return NULL;
		void *bitem = bucket_item(bucket);
		if (bucket->hash == hash) {
			memcpy(map->spare, bitem, map->elsize);
			bucket->dib = 0;
			while (1) {
				struct bucket *prev = bucket;
				i = (i + 1) & map->mask;
				bucket = bucket_at(map, i);
				if (bucket->dib <= 1) {
					prev->dib = 0;
					break;
				}
				memcpy(prev, bucket, map->bucketsz);
				prev->dib--;
			}
			map->count--;
			if (map->nbuckets > map->cap &&
			    map->count <= map->shrinkat) {
				resize(map, map->nbuckets / 2);
			}
			return map->spare;
		}
		i = (i + 1) & map->mask;
	}
}

size_t hashmap_count(struct hashmap *map)
{
	return map->count;
}

void hashmap_free(struct hashmap *map)
{
	if (!map)
		return;
	free_elements(map);
	free(map->buckets);
	free(map);
}

bool hashmap_oom(struct hashmap *map)
{
	return map->oom;
}

bool hashmap_scan(struct hashmap *map, bool (*iter)(void *item))
{
	for (size_t i = 0; i < map->nbuckets; i++) {
		struct bucket *bucket = bucket_at(map, i);
		if (bucket->dib && !iter(bucket_item(bucket))) {
			return false;
		}
	}
	return true;
}

bool hashmap_iter(struct hashmap *map, size_t *i, void **item)
{
	struct bucket *bucket;
	do {
		if (*i >= map->nbuckets)
			return false;
		bucket = bucket_at(map, *i);
		(*i)++;
	} while (!bucket->dib);
	*item = bucket_item(bucket);
	return true;
}
