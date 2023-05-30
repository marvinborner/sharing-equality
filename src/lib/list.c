// Copyright (c) 2023, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#include <stdlib.h>

#include <log.h>
#include <lib/list.h>

static struct list *list_new(void)
{
	struct list *list = malloc(sizeof(*list));
	if (!list)
		fatal("out of memory!\n");
	return list;
}

struct list *list_add(struct list *list, void *data)
{
	struct list *new = list_new();
	new->data = data;
	new->next = list;
	return new;
}

void list_free(struct list *list)
{
	while (list) {
		struct list *next = list->next;
		free(list);
		list = next;
	}
}
