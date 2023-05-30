// Copyright (c) 2023, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#ifndef SHARING_LIST_H
#define SHARING_LIST_H

struct list {
	void *data;
	struct list *next;
};

struct list *list_add(struct list *list, void *data);
void list_free(struct list *list);

#endif
