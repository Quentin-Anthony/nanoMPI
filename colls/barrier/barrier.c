#include "mpi.h"
#include "util.h"

int MPI_Barrier_basic(MPI_Comm comm)
{
    int rank, size;
    char tmp;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    // Phase 1: Gather
    if (rank != 0) {
        // All non-root processes send a message to root
        MPI_Send(&tmp, 1, MPI_BYTE, 0, 0, comm);
    } else {
        // Root receives a message from all other processes
        for (int i = 1; i < size; i++) {
            MPI_Recv(&tmp, 1, MPI_BYTE, i, 0, comm, MPI_STATUS_IGNORE);
        }
    }

    // Phase 2: Broadcast
    if (rank == 0) {
        // Root sends a message to all other processes
        for (int i = 1; i < size; i++) {
            MPI_Send(&tmp, 1, MPI_BYTE, i, 0, comm);
        }
    } else {
        // All non-root processes receive a message from root
        MPI_Recv(&tmp, 1, MPI_BYTE, 0, 0, comm, MPI_STATUS_IGNORE);
    }

    return MPI_SUCCESS;
}
