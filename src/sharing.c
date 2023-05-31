// Copyright (c) 2023, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#include <stdlib.h>

#include <lib/list.h>
#include <log.h>
#include <sharing.h>
#include <map.h>

// Procedure EnqueueAndPropagate()
static void enqueue_and_propagate(struct term *neighbour, struct term *term)
{
	if (neighbour->type == ABS && term->type == ABS) {
		// m' ~ c'
		neighbour->u.abs.term->neighbours = list_add(
			neighbour->u.abs.term->neighbours, term->u.abs.term);
		term->u.abs.term->neighbours = list_add(
			term->u.abs.term->neighbours, neighbour->u.abs.term);
	} else if (neighbour->type == APP && term->type == APP) {
		// m1 ~ c1
		neighbour->u.app.lhs->neighbours = list_add(
			neighbour->u.app.lhs->neighbours, term->u.app.lhs);
		term->u.app.lhs->neighbours = list_add(
			term->u.app.lhs->neighbours, neighbour->u.app.lhs);

		// m2 ~ c2
		neighbour->u.app.rhs->neighbours = list_add(
			neighbour->u.app.rhs->neighbours, term->u.app.rhs);
		term->u.app.rhs->neighbours = list_add(
			term->u.app.rhs->neighbours, neighbour->u.app.rhs);
	} else if (neighbour->type == VAR && term->type == VAR) {
	} else {
		fatal("query failed\n");
	}
	neighbour->canonic = term;
	queue_push(term->queue, neighbour);
}

// Procedure BuildEquivalenceClass()
static void build_equivalence_class(struct term *term)
{
	term->canonic = term;
	term->building = 1;
	term->queue = queue_new();
	queue_push(term->queue, term);

	while (!queue_empty(term->queue)) {
		struct term *cur = queue_pop(term->queue);

		struct list *iterator = cur->parents;
		while (iterator) {
			struct term *parent = iterator->data;
			if (!parent->canonic) {
				build_equivalence_class(parent);
			} else if (parent->canonic->building) {
				fatal("query failed\n");
			}
			iterator = iterator->next;
		}

		iterator = cur->neighbours;
		while (iterator) {
			struct term *neighbour = iterator->data;
			if (!neighbour->canonic) {
				enqueue_and_propagate(neighbour, term);
			} else if (neighbour->canonic != term) {
				fatal("query failed\n");
			}
		}
	}

	term->building = 0;
}

static void blind_check_callback(struct term *term)
{
	if (term->canonic)
		return;
	build_equivalence_class(term);
}

// Procedure BlindCheck()
static void blind_check(void)
{
	map_foreach(blind_check_callback);
}

static void var_check_callback(struct term *term)
{
	if (term->type != VAR)
		return;
	struct term *canonic = term->canonic;
	if (canonic != term) {
		// you should compare the binders as stated in the paper.
		// due to the implementation I just compare the indices directly
		if (canonic->u.var.index != term->u.var.index) {
			fatal("query failed\n");
		}
	}
}

// Procedure VarCheck()
static void var_check(void)
{
	map_foreach(var_check_callback);
}

// this doesn't really make sense for this specific implementation of
// λ-graphs using hashes. It's a reference implementation after all.
void sharing_query(struct term *a, struct term *b)
{
	a->neighbours = list_add(a->neighbours, b);
	b->neighbours = list_add(b->neighbours, a);

	blind_check();
	var_check();
}
