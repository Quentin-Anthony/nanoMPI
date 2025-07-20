#ifndef NANOMPI_REDUCE_SCATTER_H
#define NANOMPI_REDUCE_SCATTER_H

int MPI_Reduce_scatter(const void *sendbuf, void *recvbuf,
                       const int recvcounts[], MPI_Datatype datatype, MPI_Op op,
                       MPI_Comm comm);
int MPI_Reduce_scatter_basic(const void *sendbuf, void *recvbuf,
                             const int recvcounts[], MPI_Datatype datatype,
                             MPI_Op op, MPI_Comm comm);

#endif
