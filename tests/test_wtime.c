#include "mpi.h"
#include "util.h"
#include <unistd.h>

int main(int argc, char **argv) {
    double start;
    MPI_Init(&argc, &argv);
    start = MPI_Wtime();
    sleep(1);
    PRINT_STDOUT("1 second sleep measured to be %f seconds\n", MPI_Wtime() - start);

    start = MPI_Wtime();
    for (size_t i = 0; i < 100000; i++) {
    }
    PRINT_STDOUT("100000 increments measured to be %f seconds\n", MPI_Wtime() - start);

    MPI_Finalize();
    return 0;
}