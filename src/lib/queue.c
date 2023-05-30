// Copyright (c) 2023, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#include <stdlib.h>

#include <lib/queue.h>
#include <log.h>

struct queue *queue_new(void)
{
	struct queue *queue = malloc(sizeof(*queue));
	if (!queue)
		fatal("out of memory!\n");
	queue->head = 0;
	queue->tail = 0;
	return queue;
}

void queue_free(struct queue *queue)
{
	while (queue->head) {
		struct queue_node *node = queue->head;
		queue->head = node->next;
		free(node);
	}
	free(queue);
}

void queue_push(struct queue *queue, void *data)
{
	struct queue_node *node = malloc(sizeof(*node));
	if (!node)
		fatal("out of memory!\n");
	node->data = data;
	node->next = 0;
	if (queue->tail) {
		queue->tail->next = node;
		queue->tail = node;
	} else {
		queue->head = node;
		queue->tail = node;
	}
}

void *queue_pop(struct queue *queue)
{
	if (!queue->head)
		return 0;
	struct queue_node *node = queue->head;
	queue->head = node->next;
	if (!queue->head)
		queue->tail = 0;
	void *data = node->data;
	free(node);
	return data;
}

int queue_empty(struct queue *queue)
{
	return !queue->head;
}
