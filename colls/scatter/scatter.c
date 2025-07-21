#include "mpi.h"
#include "util.h"

#include <stddef.h>
#include <string.h>

int MPI_Scatter_basic(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf,
                      int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    if (rank == root) {
        for (int i = 0; i < size; i++) {
            if (i != root) {
                MPI_Send((char *)sendbuf + ((ptrdiff_t)(i * sendcount * nanompi_get_dtype_size(sendtype))),
                         sendcount, sendtype, i, 0, comm);
            } else {
                memcpy(recvbuf, (char *)sendbuf + ((ptrdiff_t)(i * sendcount * nanompi_get_dtype_size(sendtype))),
                       recvcount * nanompi_get_dtype_size(recvtype));
            }
        }
    } else {
        MPI_Recv(recvbuf, recvcount, recvtype, root, 0, comm, MPI_STATUS_IGNORE);
    }

    return MPI_SUCCESS;
}
