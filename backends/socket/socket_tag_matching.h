#ifndef NANOMPI_SOCKET_TAGMATCHING_H
#define NANOMPI_SOCKET_TAGMATCHING_H

#include "comm.h"
#include "socket_backend.h"
#include <poll.h>

#define MPI_ANY_SOURCE -1 

typedef struct nanompi_message_envelope {
    size_t sizeof_buffer;
    int tag; 
    // Note: Removed communicator pointer as it shouldn't be serialized across processes
} nanompi_message_envelope; 

// Function declarations for proper memory management
int mpi_poll_fd_init_backend(MPI_Comm comm);
void mpi_poll_fd_cleanup_backend(MPI_Comm comm);
struct pollfd * mpi_poll_fd_get(MPI_Comm comm);
int mpi_poll_source(MPI_Comm comm);

#endif