// Copyright (c) 2023, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#ifndef SHARING_MAP_H
#define SHARING_MAP_H

#include <lib/hash.h>

struct term *map_get(hash_t hash);
void map_set(struct term *term, hash_t hash);
void map_delete(struct term *term);
void map_initialize(void);
void map_destroy(void);
void map_foreach(void (*func)(struct term *));

#endif
