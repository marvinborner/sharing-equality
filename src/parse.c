// Copyright (c) 2023, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#include <stdlib.h>

#include <lib/list.h>
#include <parse.h>
#include <log.h>
#include <map.h>

static struct term_handle abs_blc(char **term)
{
	term_type_t res_type = ABS;
	struct term_handle inner = parse_blc(term);
	hash_t res = hash((uint8_t *)&res_type, sizeof(res_type), inner.hash);

	struct term *res_term;
	if (!(res_term = map_get(res))) {
		res_term = term_new(res_type, res);
		res_term->u.abs.term = inner.term;
		map_set(res_term, res);
	}

	res_term->u.abs.term->parents =
		list_add(res_term->u.abs.term->parents, res_term);
	return (struct term_handle){ .term = res_term, .hash = res };
}

static struct term_handle app_blc(char **term)
{
	term_type_t res_type = APP;
	struct term_handle lhs = parse_blc(term);
	struct term_handle rhs = parse_blc(term);

	hash_t res = hash((uint8_t *)&res_type, sizeof(res_type), lhs.hash);
	res = hash((uint8_t *)&res, sizeof(res), rhs.hash);

	struct term *res_term;
	if (!(res_term = map_get(res))) {
		res_term = term_new(res_type, res);
		res_term->u.app.lhs = lhs.term;
		res_term->u.app.rhs = rhs.term;
		map_set(res_term, res);
	}

	res_term->u.app.lhs->parents =
		list_add(res_term->u.app.lhs->parents, res_term);
	res_term->u.app.rhs->parents =
		list_add(res_term->u.app.rhs->parents, res_term);
	return (struct term_handle){ .term = res_term, .hash = res };
}

static struct term_handle var_blc(int index)
{
	term_type_t res_type = VAR;
	hash_t res = hash((uint8_t *)&res_type, sizeof(res_type), index);

	struct term *res_term;
	if (!(res_term = map_get(res))) {
		res_term = term_new(res_type, res);
		res_term->u.var.index = index;
		map_set(res_term, res);
	}

	return (struct term_handle){ .term = res_term, .hash = res };
}

struct term_handle parse_blc(char **term)
{
	if (!**term) {
		fatal("invalid parsing state!\n");
	} else if (**term == '0' && *(*term + 1) == '0') {
		(*term) += 2;
		return abs_blc(term);
	} else if (**term == '0' && *(*term + 1) == '1') {
		(*term) += 2;
		return app_blc(term);
	} else if (**term == '1') {
		const char *cur = *term;
		while (**term == '1')
			(*term)++;
		int index = *term - cur - 1;
		(*term)++;
		return var_blc(index);
	} else {
		(*term)++;
		return parse_blc(term);
	}
}
