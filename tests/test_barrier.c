#include "mpi.h"
#include "util.h"
#include <unistd.h>

int main(int argc, char **argv) {
    int rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        sleep(5);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    PRINT_STDOUT("Hacky barrier test--see if everyone prints this message at the same time\n");

    MPI_Finalize();
    return 0;
}
