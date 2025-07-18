#ifndef NANOMPI_TAGMATCHING_H
#define NANOMPI_TAGMATCHING_H
#include "comm.h"
#include <poll.h>
#define MPI_ANY_SOURCE -1 
static struct pollfd * mpi_poll_fd(MPI_Comm comm, int size);
int mpi_poll_source(MPI_Comm comm);


#endif 
