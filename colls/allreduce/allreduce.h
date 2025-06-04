#ifndef NANOMPI_ALLREDUCE_H
#define NANOMPI_ALLREDUCE_H

int MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
                  MPI_Op op, MPI_Comm comm);

int MPI_Allreduce_basic(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
                  MPI_Op op, MPI_Comm comm);

int MPI_Allreduce_ring(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
                       MPI_Op op, MPI_Comm comm);

int MPI_Allreduce_reduce_bcast(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
                               MPI_Op op, MPI_Comm comm);

int MPI_Allreduce_reduce_scatter_allgather(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
                                           MPI_Op op, MPI_Comm comm);

#endif
