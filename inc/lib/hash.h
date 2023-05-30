// Copyright (c) 2023, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#ifndef SHARING_HASH_H
#define SHARING_HASH_H

#include <stdint.h>
#include <stddef.h>

typedef uint64_t hash_t;

hash_t hash(void *data, size_t len, uint64_t seed);

#endif
