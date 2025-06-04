#ifndef NANOMPI_SELF_BACKEND_H
#define NANOMPI_SELF_BACKEND_H

#include "mpi.h"

int nanompi_self_send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
int nanompi_self_recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm);

#endif
