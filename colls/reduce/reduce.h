#ifndef NANOMPI_REDUCE_H
#define NANOMPI_REDUCE_H

int MPI_Reduce(const void *sendbuf, void *recvbuf, int count,
               MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);
int MPI_Reduce_basic(const void *sendbuf, void *recvbuf, int count,
                     MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);

#endif
