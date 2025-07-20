// colls/allgather.c

#include "mpi.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int MPI_Allgather_bruck(const void *sendbuf, int sendcount,
                        MPI_Datatype sendtype, void *recvbuf, int recvcount,
                        MPI_Datatype recvtype, MPI_Comm comm) {
  int rank, size;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  int type_size = nanompi_get_dtype_size(sendtype);
  int recv_size = recvcount * type_size;
  int total_size = recv_size * size;

  // Copy local data to receive buffer
  memcpy((char *)recvbuf + rank * recv_size, sendbuf, recv_size);

  // Allocate temporary buffer
  void *temp_buf = malloc(total_size);
  memcpy(temp_buf, recvbuf, total_size);

  // Bruck algorithm
  for (int i = 1; i < size; i <<= 1) {
    int send_to = (rank + i) % size;
    int recv_from = (rank - i + size) % size;

    // MPI_Send(send_data, send_count, sendtype, dest, tag, comm);
    // MPI_Recv(recv_data, recv_count, recvtype, source, tag, comm,
    // MPI_STATUS_IGNORE);

    memcpy(temp_buf, recvbuf, total_size);
  }

  // Rearrange data in the correct order
  for (int i = 0; i < size; i++) {
    int src = (rank - i + size) % size;
    memcpy((char *)recvbuf + i * recv_size, (char *)temp_buf + src * recv_size,
           recv_size);
  }

  free(temp_buf);
  return MPI_SUCCESS;
}

int MPI_Allgather_ring(const void *sendbuf, int sendcount,
                       MPI_Datatype sendtype, void *recvbuf, int recvcount,
                       MPI_Datatype recvtype, MPI_Comm comm) {
  int rank, size;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  int type_size = nanompi_get_dtype_size(sendtype);
  int recv_size = recvcount * type_size;

  // Copy local data to receive buffer
  memcpy((char *)recvbuf + rank * recv_size, sendbuf, recv_size);

  // Ring algorithm
  for (int i = 0; i < size - 1; i++) {
    int send_to = (rank + 1) % size;
    int recv_from = (rank - 1 + size) % size;
    int send_offset = ((rank - i + size) % size) * recv_size;
    int recv_offset = ((rank - i - 1 + size) % size) * recv_size;

    // MPI_Send(send_data, send_count, send_type, dest, tag, comm);
    // MPI_Recv(recv_data, recv_count, recv_type, source, tag, comm,
    // MPI_STATUS_IGNORE);
  }

  return MPI_SUCCESS;
}

int MPI_Allgather_basic(const void *sendbuf, int sendcount,
                        MPI_Datatype sendtype, void *recvbuf, int recvcount,
                        MPI_Datatype recvtype, MPI_Comm comm) {
  int rank, size;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  for (int root = 0; root < size; root++) {
    MPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root,
               comm);
  }

  return MPI_SUCCESS;
}
