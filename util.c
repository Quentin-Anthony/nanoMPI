#include "util.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

struct timespec start;

void nanompi_init_clock()
{
    clock_gettime(CLOCK_MONOTONIC, &start);
}

double MPI_Wtime(void)
{
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    return (((long)end.tv_sec*1e9L - (long)start.tv_sec*1e9L) + end.tv_nsec - start.tv_nsec) / 1e9L;
}

int MPI_Abort(MPI_Comm comm, int errorcode)
{
    int rank;
    MPI_Comm_rank(comm, &rank);
    printf("rank %d called MPI_Abort, exiting...\n", rank);
    exit(errorcode);
}
