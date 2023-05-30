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
		fatal("huh3?\n");
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
				fatal("huh?\n");
			}
			iterator = iterator->next;
		}

		iterator = cur->neighbours;
		while (iterator) {
			struct term *neighbour = iterator->data;
			if (!neighbour->canonic) {
				enqueue_and_propagate(neighbour, term);
			} else if (neighbour->canonic != term) {
				fatal("huh2?\n");
			}
		}
	}

	term->building = 0;
}

static void iter_callback(struct term *term)
{
	if (term->canonic)
		return;
	build_equivalence_class(term);
}

// Procedure BlindCheck()
void blind_check(void)
{
	map_foreach(iter_callback);
}
