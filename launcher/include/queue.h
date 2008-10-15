#ifndef LEGNALEURC_QUEUE_H
#define LEGNALEURC_QUEUE_H

#include <tchar.h>
#include <stdlib.h>
#include <string.h>

typedef struct PQ_node {
	TCHAR path[_MAX_PATH];
	struct PQ_node * next;
} PQ_node;

typedef struct PathQueue {
	PQ_node * front;
	PQ_node * last;
} PathQueue;

PathQueue newPathQueue();

void freePathQueue( PathQueue * );

void PQ_push( PathQueue *, const TCHAR * );

void PQ_pop( PathQueue * );

int PQ_empty( PathQueue );

#endif
