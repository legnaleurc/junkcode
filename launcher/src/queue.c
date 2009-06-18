#include "queue.h"

PathQueue newPathQueue() {
	PathQueue temp = {
		NULL,
		NULL
	};
	return temp;
}

void freePathQueue( PathQueue * q ) {
	while( !PQ_empty( *q ) ) {
		PQ_pop( q );
	}
}

void PQ_push( PathQueue * q, const TCHAR * path ) {
	PQ_node * tmp = malloc( sizeof( PQ_node ) );
	_tcscpy_s( tmp->path, _MAX_PATH, path );
	tmp->next = NULL;

	if( q->front == NULL ) {
		q->front = tmp;
		q->last = q->front;
	} else {
		q->last->next = tmp;
		q->last = q->last->next;
	}
}

void PQ_pop( PathQueue * q ) {
	PQ_node * tmp = NULL;

	if( !PQ_empty( *q ) ) {
		tmp = q->front;
		q->front = q->front->next;
		if( q->last->next == q->front ) {
			q->last = q->front;
		}

		free( tmp );
	}
}

int PQ_empty( PathQueue q ) {
	return ( q.front == NULL );
}
