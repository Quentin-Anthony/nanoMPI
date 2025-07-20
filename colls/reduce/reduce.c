#include "mpi.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>

int MPI_Reduce_basic(const void *sendbuf, void *recvbuf, int count,
                     MPI_Datatype datatype, MPI_Op op, int root,
                     MPI_Comm comm) {
  int rank, size;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  if (rank == root) {
    memcpy(recvbuf, sendbuf, count * nanompi_get_dtype_size(datatype));
    for (int i = 0; i < size; i++) {
      if (i != root) {
        void *tempbuf = malloc(count * nanompi_get_dtype_size(datatype));
        MPI_Recv(tempbuf, count, datatype, i, 0, comm, MPI_STATUS_IGNORE);
        // TODO: other reduction ops
        for (int j = 0; j < count; j++) {
          ((int *)recvbuf)[j] += ((int *)tempbuf)[j];
        }
        free(tempbuf);
      }
    }
  } else {
    MPI_Send(sendbuf, count, datatype, root, 0, comm);
  }

  return MPI_SUCCESS;
}
