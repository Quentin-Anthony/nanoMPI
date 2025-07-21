#include "mpi.h"
#include "util.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

int MPI_Allgatherv_basic(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf,
                         const int *recvcounts, const int *displs, MPI_Datatype recvtype,
                         MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    // First, gather all data to root (rank 0)
    // MPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcounts, recvtype, 0, comm); //
    // recvcounts incorrect here

    // Then, broadcast the gathered data to all processes
    int total_recv_count = 0;
    for (int i = 0; i < size; i++) {
        total_recv_count += recvcounts[i];
    }
    assert(total_recv_count > 0);

    MPI_Bcast(recvbuf, total_recv_count, recvtype, 0, comm);

    // If the receiving buffer is not properly displaced, we need to rearrange it
    if (rank != 0) {
        // NOLINT
        void *temp_buf = malloc(total_recv_count * nanompi_get_dtype_size(recvtype));
        memcpy(temp_buf, recvbuf, total_recv_count * nanompi_get_dtype_size(recvtype));

        for (int i = 0; i < size; i++) {
            memcpy((char *)recvbuf + ((ptrdiff_t)(displs[i] * nanompi_get_dtype_size(recvtype))),
                   (char *)temp_buf + ((ptrdiff_t)(displs[i] * nanompi_get_dtype_size(recvtype))),
                   recvcounts[i] * nanompi_get_dtype_size(recvtype));
        }

        free(temp_buf);
    }

    return MPI_SUCCESS;
}
