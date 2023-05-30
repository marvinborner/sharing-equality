// Copyright (c) 2023, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#include <stdlib.h>

#include <parse.h>
#include <log.h>

struct term *parse_blc(char **term)
{
	if (!**term) {
		fatal("invalid parsing state!\n");
	} else if (**term == '0' && *(*term + 1) == '0') {
		(*term) += 2;
		struct term *new = term_new(ABS);
		new->u.abs.term = parse_blc(term);
		return new;
	} else if (**term == '0' && *(*term + 1) == '1') {
		(*term) += 2;
		struct term *new = term_new(APP);
		new->u.app.lhs = parse_blc(term);
		new->u.app.rhs = parse_blc(term);
		return new;
	} else if (**term == '1') {
		const char *cur = *term;
		while (**term == '1')
			(*term)++;
		int index = *term - cur - 1;
		(*term)++;
		struct term *new = term_new(VAR);
		new->u.var.index = index;
		return new;
	} else {
		(*term)++;
		return parse_blc(term);
	}
}
