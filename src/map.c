// Copyright (c) 2023, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#include <stdlib.h>
#include <stdio.h>

#include <lib/hashmap.h>
#include <map.h>
#include <lib/list.h>
#include <parse.h>

static struct hashmap *all_terms;

static void hashmap_free_term(void *item)
{
	struct term *term = *(struct term **)item;
	list_free(term->parents);
	list_free(term->neighbours);
	queue_free(term->queue);
	free(term);
}

struct term *map_get(hash_t hash)
{
	struct term **handle = hashmap_get(all_terms, hash);
	if (!handle)
		return 0;
	return *handle;
}

void map_set(struct term *term, hash_t hash)
{
	hashmap_set(all_terms, &term, hash);
}

void map_initialize(void)
{
	all_terms = hashmap_new(sizeof(struct term *), 0, hashmap_free_term);
}

void map_delete(struct term *term)
{
	hashmap_delete(all_terms, term->hash);
}

void map_destroy(void)
{
	hashmap_free(all_terms);
}

void map_foreach(void (*func)(struct term *))
{
	size_t iter = 0;
	void *iter_val;
	while (hashmap_iter(all_terms, &iter, &iter_val)) {
		struct term *term = *(struct term **)iter_val;
		func(term);
	}
}
