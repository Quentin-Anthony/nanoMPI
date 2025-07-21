#ifndef NANOMPI_TAGMATCHING_H
#define NANOMPI_TAGMATCHING_H
#include "comm.h"
#include "socket_backend.h"
#include <poll.h>
#define MPI_ANY_SOURCE -1
typedef struct nanompi_message_envelope {
  size_t sizeof_buffer;
  int tag;
  int dest; 
  nanompi_communicator_t *comm;
} nanompi_message_envelope;

// lets use a queue data structure because mpi 
// needs a last in first out (LIFO) in order to process the most recent sends
//
typedef struct nanompi_queue_node {
	nanompi_message_envelope envelope; 
	char * buffer; 
	struct nanompi_queue_node* next; 
	struct nanompi_queue_node * prev; 
}nanompi_queue_node; 
typedef struct nanompi_queue {
	size_t size;
	nanompi_queue_node * head; 
	nanompi_queue_node * tail; 
}nanompi_queue;
void enqueue(nanompi_message_envelope env, char * buffer);
nanompi_queue_node * find_match(nanompi_message_envelope env); 
struct pollfd *mpi_poll_fd_init(MPI_Comm comm, int size);
int mpi_poll_source(MPI_Comm comm);
void mpi_poll_fd_destroy(void);

#endif
