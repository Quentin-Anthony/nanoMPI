#include "mpi.h"
#include "util.h"

int MPI_Bcast_basic(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    if (rank == root) {
        for (int i = 0; i < size; i++) {
            if (i != root) {
                MPI_Send(buffer, count, datatype, i, 0, comm);
            }
        }
    } else {
        MPI_Recv(buffer, count, datatype, root, 0, comm, MPI_STATUS_IGNORE);
    }

    return MPI_SUCCESS;
}
