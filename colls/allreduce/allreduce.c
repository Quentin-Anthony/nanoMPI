// colls/allreduce.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../allgatherv/allgatherv.h"

#include "util.h"

// We model the total time to run the algorithms in this file using the α − β
// cost model, also called the Hockney model
// (https://doi.org/10.1016/S0167-8191(06)80021-9). The terms are:
//    α=startup latency,
//    β=pt2pt bandwidth,
//    γ=compute bandwidth, and
//    p=num processors

// Ring Allreduce
// First, do a reduce scatter in a ring fashion. Then, do a ring allgather.
// Requires the reduce op to be associative and commutative.
// T_ring = 2(p − 1)α + 2nβ + nγ − (1/p)(2nβ+nγ)
// TODO: expand on derivation
int MPI_Allreduce_ring(const void *sendbuf, void *recvbuf, int count,
                       MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
  int rank, size;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  int type_size = nanompi_get_dtype_size(datatype);
  int send_to = mod(rank + 1, size);
  int recv_from = mod(rank - 1, size);
  int chunk_count = count / size;
  int chunk_size = chunk_count * type_size;
  int chunk_rem =
      count % size; // todo: handle remainder instead of the following assert
  assert(chunk_rem == 0); // msg size too small for ring allreduce

  // Copy sendbuf to recvbuf
  memcpy(recvbuf, sendbuf, count * type_size);

  void *temp_buf = malloc(count * type_size);
  void *send_buf = malloc(count * type_size);
  memcpy(send_buf, recvbuf, count * type_size);

  // Reduce-scatter
  for (int i = 0; i < size - 1; i++) {
    ptrdiff_t send_offset = chunk_size * mod(rank - i, size);
    ptrdiff_t recv_offset = chunk_size * mod(rank - i - 1, size);

    MPI_Send(send_buf + send_offset, chunk_count, datatype, send_to, 0, comm);
    MPI_Recv(temp_buf + recv_offset, chunk_count, datatype, recv_from, 0, comm,
             MPI_STATUS_IGNORE);

    // Perform reduction operation
    op.fn(temp_buf + recv_offset, recvbuf + recv_offset, &chunk_count,
          &datatype);

    // Prepare for next iteration
    memcpy(send_buf + recv_offset, recvbuf + recv_offset, chunk_size);
  }

  // Now, each rank has one completely reduced chunk_size portion of all ranks'
  // sendbuf stored in chunk index=(rank + i) % size on the recvbuf

  // Allgather
  for (int i = 0; i < size - 1; i++) {
    ptrdiff_t send_offset = chunk_size * mod(rank - i + 1, size);
    ptrdiff_t recv_offset = chunk_size * mod(rank - i, size);

    MPI_Send(recvbuf + send_offset, chunk_count, datatype, send_to, 0, comm);
    MPI_Recv(recvbuf + recv_offset, chunk_count, datatype, recv_from, 0, comm,
             MPI_STATUS_IGNORE);
  }

  free(temp_buf);
  free(send_buf);
  return MPI_SUCCESS;
}

int MPI_Allreduce_reduce_bcast(const void *sendbuf, void *recvbuf, int count,
                               MPI_Datatype datatype, MPI_Op op,
                               MPI_Comm comm) {
  MPI_Reduce(sendbuf, recvbuf, count, datatype, op, 0, comm);
  MPI_Bcast(recvbuf, count, datatype, 0, comm);

  return MPI_SUCCESS;
}

int MPI_Allreduce_reduce_scatter_allgather(const void *sendbuf, void *recvbuf,
                                           int count, MPI_Datatype datatype,
                                           MPI_Op op, MPI_Comm comm) {
  int rank, size;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  int type_size;
  MPI_Type_size(datatype, &type_size);

  int *recvcounts = (int *)malloc(size * sizeof(int));
  int *displs = (int *)malloc(size * sizeof(int));
  int chunk_size = count / size;
  int remainder = count % size;

  for (int i = 0; i < size; i++) {
    recvcounts[i] = chunk_size + (i < remainder ? 1 : 0);
    displs[i] = (i > 0) ? (displs[i - 1] + recvcounts[i - 1]) : 0;
  }

  void *temp_buf = malloc(count * type_size);

  // Reduce-scatter
  MPI_Reduce_scatter(sendbuf, temp_buf, recvcounts, datatype, op, comm);

  // Allgather
  MPI_Allgatherv(temp_buf, recvcounts[rank], datatype, recvbuf, recvcounts,
                 displs, datatype, comm);

  free(recvcounts);
  free(displs);
  free(temp_buf);

  return MPI_SUCCESS;
}

int MPI_Allreduce_basic(const void *sendbuf, void *recvbuf, int count,
                        MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
  int rank, size;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  MPI_Reduce(sendbuf, recvbuf, count, datatype, op, 0, comm);
  MPI_Bcast(recvbuf, count, datatype, 0, comm);

  return MPI_SUCCESS;
}
