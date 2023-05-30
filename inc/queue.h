// Copyright (c) 2023, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#ifndef SHARING_QUEUE_H
#define SHARING_QUEUE_H

#include <stddef.h>

struct queue_node {
	void *data;
	struct queue_node *next;
};

struct queue {
	struct queue_node *head;
	struct queue_node *tail;
};

struct queue *queue_new(void);
void queue_free(struct queue *queue);
void queue_push(struct queue *queue, void *data);
void *queue_pop(struct queue *queue);

#endif
