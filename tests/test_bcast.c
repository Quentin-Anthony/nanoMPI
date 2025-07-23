#include "mpi.h"
#include "util.h"

#define ARRAY_SIZE 5

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int data[ARRAY_SIZE];
    if (rank == 0) {
        for (int i = 0; i < ARRAY_SIZE; i++) {
            data[i] = i * 10;
        }
        PRINT_STDOUT("Rank 0: Broadcasting data\n");
    }

    MPI_Bcast(data, ARRAY_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

    PRINT_STDOUT("Rank %d received: ", rank);
    for (int i = 0; i < ARRAY_SIZE; i++) {
        PRINT_STDOUT("%d ", data[i]);
    }
    PRINT_STDOUT("\n");

    MPI_Finalize();
    return 0;
}