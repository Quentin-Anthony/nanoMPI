#include <stdio.h>

#include "mpi.h"

int main(int argc, char** argv) {
    int status;
    status = MPI_Init(NULL, NULL);
    if (status) {
        printf("Error in MPI_Init\n");
        return 1;
    }

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    printf("Hello world from rank %d out of %d processors\n",
           world_rank, world_size);

    MPI_Finalize();
    return 0;
}
