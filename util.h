#ifndef NANOMPI_UTIL_H
#define NANOMPI_UTIL_H

#include <assert.h>

#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"

static inline int nanompi_get_dtype_size(MPI_Datatype dtype)
{
    return dtype.size;
}

static inline size_t nanompi_get_msg_size(MPI_Datatype dtype, int count)
{
    return count * nanompi_get_dtype_size(dtype);
}

// todo: make a macro and use inline for array indexing
static inline int mod(int i, int s)
{
    return (i + s) % s;
}

#define PRINT_FLUSH(stream, ...) do { fprintf(stream, __VA_ARGS__); fflush(stream); } while(0)
#define PRINT_STDOUT(...) PRINT_FLUSH(stdout, __VA_ARGS__)
#define PRINT_STDERR(...) PRINT_FLUSH(stderr, __VA_ARGS__)

void nanompi_init_clock();

double MPI_Wtime(void);
int MPI_Abort(MPI_Comm comm, int errorcode);

#endif
