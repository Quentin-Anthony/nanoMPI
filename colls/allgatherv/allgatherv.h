#ifndef NANOMPI_ALLGATHERV_H
#define NANOMPI_ALLGATHERV_H

#include "mpi.h"

int MPI_Allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                   void *recvbuf, const int *recvcounts, const int *displs,
                   MPI_Datatype recvtype, MPI_Comm comm);

int MPI_Allgatherv_basic(const void *sendbuf, int sendcount,
                         MPI_Datatype sendtype, void *recvbuf,
                         const int *recvcounts, const int *displs,
                         MPI_Datatype recvtype, MPI_Comm comm);

#endif
