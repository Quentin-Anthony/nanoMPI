#ifndef NANOMPI_TAGMATCHING_H
#define NANOMPI_TAGMATCHING_H
#include "comm.h"
#include "socket_backend.h"
#include <poll.h>
#define MPI_ANY_SOURCE -1 
typedef struct nanompi_message_envelope {
	size_t sizeof_buffer;
	int tag; 
	nanompi_communicator_t *comm; 

} nanompi_message_envelope; 
struct pollfd * mpi_poll_fd_init(MPI_Comm comm, int size);
int mpi_poll_source(MPI_Comm comm);


#endif 
