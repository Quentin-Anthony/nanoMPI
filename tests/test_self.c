#include <stdio.h>
#include <string.h>

#include "mpi.h"
#include "util.h"

int main(int argc, char *argv[]) {
    int rank, size, source, tag = 0;
    char message[100];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    sprintf(message, "Greetings from process %d!", rank);
    MPI_Send(message, strlen(message) + 1, MPI_CHAR, rank, tag, MPI_COMM_WORLD);
    memset(message, '\0', 100);
    MPI_Recv(message, 100, MPI_CHAR, rank, tag, MPI_COMM_WORLD, &status);
    PRINT_STDOUT("%s\n", message);

    MPI_Finalize();
    return 0;
}
