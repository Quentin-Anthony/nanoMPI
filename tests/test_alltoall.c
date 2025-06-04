#include "util.h"
#include "mpi.h"

#define ARRAY_SIZE 16

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int send_data[ARRAY_SIZE];
    int recv_data[ARRAY_SIZE];

    for (int i = 0; i < ARRAY_SIZE; i++) {
        send_data[i] = rank * 100 + i;
    }

    PRINT_STDOUT("Rank %d: Send data: ", rank);
    for (int i = 0; i < ARRAY_SIZE; i++) {
        PRINT_STDOUT("%d ", send_data[i]);
    }
    PRINT_STDOUT("\n");

    MPI_Alltoall(send_data, ARRAY_SIZE / size, MPI_INT, recv_data, ARRAY_SIZE / size, MPI_INT, MPI_COMM_WORLD);

    PRINT_STDOUT("Rank %d: Received data: ", rank);
    for (int i = 0; i < ARRAY_SIZE; i++) {
        PRINT_STDOUT("%d ", recv_data[i]);
    }
    PRINT_STDOUT("\n");

    MPI_Finalize();
    return 0;
}