// Copyright (c) 2023, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#ifndef SHARING_TERM_H
#define SHARING_TERM_H

#include <stddef.h>

#include <queue.h>

typedef enum { INV, ABS, APP, VAR } term_type_t;

struct term {
	term_type_t type;
	struct term *canonic;
	char building;
	struct queue *queue;
	union {
		struct {
			struct term *term;
		} abs;
		struct {
			struct term *lhs;
			struct term *rhs;
		} app;
		struct {
			size_t index;
		} var;
	} u;
};

struct term *term_new(term_type_t type);
void term_free(struct term *term);
void term_print(struct term *term);

#endif
