// Copyright (c) 2023, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <log.h>
#include <term.h>

struct term *term_new(term_type_t type, hash_t hash)
{
	struct term *term = calloc(sizeof(*term), 1);
	if (!term)
		fatal("out of memory!\n");
	term->type = type;
	term->hash = hash;
	return term;
}

void term_free(struct term *term)
{
	switch (term->type) {
	case ABS:
		term_free(term->u.abs.term);
		free(term);
		break;
	case APP:
		term_free(term->u.app.lhs);
		term_free(term->u.app.rhs);
		free(term);
		break;
	case VAR:
		free(term);
		break;
	default:
		fatal("invalid type %d\n", term->type);
	}
}

void term_print(struct term *term)
{
	switch (term->type) {
	case ABS:
		fprintf(stderr, "[");
		term_print(term->u.abs.term);
		fprintf(stderr, "]");
		break;
	case APP:
		fprintf(stderr, "(");
		term_print(term->u.app.lhs);
		fprintf(stderr, " ");
		term_print(term->u.app.rhs);
		fprintf(stderr, ")");
		break;
	case VAR:
		fprintf(stderr, "%ld", term->u.var.index);
		break;
	default:
		fatal("invalid type %d\n", term->type);
	}
}
