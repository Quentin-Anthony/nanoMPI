#include <stddef.h>

#include "mpi.h"
#include "util.h"

int MPI_Alltoall_basic(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf,
                       int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    for (int i = 0; i < size; i++) {
        MPI_Send((char *)sendbuf + ((ptrdiff_t)(i * sendcount * nanompi_get_dtype_size(sendtype))), sendcount,
                 sendtype, i, 0, comm);
        MPI_Recv((char *)recvbuf + ((ptrdiff_t)(i * recvcount * nanompi_get_dtype_size(recvtype))), recvcount,
                 recvtype, i, 0, comm, MPI_STATUS_IGNORE);
    }

    return MPI_SUCCESS;
}
