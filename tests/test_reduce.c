#include "util.h"
#include "mpi.h"

#define ARRAY_SIZE 5

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int send_data[ARRAY_SIZE];
    int recv_data[ARRAY_SIZE];

    for (int i = 0; i < ARRAY_SIZE; i++) {
        send_data[i] = rank * 10 + i;
    }

    MPI_Reduce(send_data, recv_data, ARRAY_SIZE, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        PRINT_STDOUT("Rank 0: Sum of all processes: ");
        for (int i = 0; i < ARRAY_SIZE; i++) {
            PRINT_STDOUT("%d ", recv_data[i]);
        }
        PRINT_STDOUT("\n");
    }

    MPI_Finalize();
    return 0;
}
