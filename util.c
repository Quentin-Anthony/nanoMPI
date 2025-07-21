#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct timespec start;

void nanompi_init_clock() { clock_gettime(CLOCK_MONOTONIC, &start); }

double MPI_Wtime(void) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (double)(now.tv_sec - start.tv_sec) + ((double)(now.tv_nsec - start.tv_nsec) / 1e9);
}

int MPI_Abort(MPI_Comm comm, int errorcode) {
    int rank;
    MPI_Comm_rank(comm, &rank);
    printf("rank %d called MPI_Abort, exiting...\n", rank);
    exit(errorcode);
}
