#include "mpi.h"
#include "util.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

int MPI_Reduce_scatter_basic(const void *sendbuf, void *recvbuf, const int recvcounts[],
                             MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    int total_count = 0;
    for (int i = 0; i < size; i++) {
        total_count += recvcounts[i];
    }

    void *tempbuf = malloc(total_count * nanompi_get_dtype_size(datatype));
    MPI_Allreduce(sendbuf, tempbuf, total_count, datatype, op, comm);

    int offset = 0;
    for (int i = 0; i < rank; i++) {
        offset += recvcounts[i];
    }

    memcpy(recvbuf, (char *)tempbuf + ((ptrdiff_t)(offset * nanompi_get_dtype_size(datatype))),
           recvcounts[rank] * nanompi_get_dtype_size(datatype));
    free(tempbuf);

    return MPI_SUCCESS;
}
