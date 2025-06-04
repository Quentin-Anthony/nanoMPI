#include "mpi.h"
#include "util.h"

#include <string.h>

int MPI_Gather_basic(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
               void *recvbuf, int recvcount, MPI_Datatype recvtype,
               int root, MPI_Comm comm)
{
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    if (rank == root) {
        for (int i = 0; i < size; i++) {
            if (i != root) {
                MPI_Recv((char*)recvbuf + i * recvcount * nanompi_get_dtype_size(recvtype), recvcount, recvtype, i, 0, comm, MPI_STATUS_IGNORE);
            } else {
                memcpy((char*)recvbuf + i * recvcount * nanompi_get_dtype_size(recvtype), sendbuf, sendcount * nanompi_get_dtype_size(sendtype));
            }
        }
    } else {
        MPI_Send(sendbuf, sendcount, sendtype, root, 0, comm);
    }

    return MPI_SUCCESS;
}
