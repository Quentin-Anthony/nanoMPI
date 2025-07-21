#ifndef NANOMPI_DTYPE_H
#define NANOMPI_DTYPE_H

#include <stddef.h>
#include <stdint.h>

enum nanompi_dtype_id {
    NANOMPI_DTYPE_INT,
    NANOMPI_DTYPE_FLOAT,
    NANOMPI_DTYPE_DOUBLE,
    NANOMPI_DTYPE_CHAR,
    NANOMPI_DTYPE_BYTE
};

typedef struct nanompi_dtype_t {
    enum nanompi_dtype_id    id;
    int                      size;
} nanompi_dtype_t;

typedef nanompi_dtype_t MPI_Datatype;

/* Predefined dtypes */
extern MPI_Datatype nanompi_dtype_int;
extern MPI_Datatype nanompi_dtype_float;
extern MPI_Datatype nanompi_dtype_double;
extern MPI_Datatype nanompi_dtype_char;
extern MPI_Datatype nanompi_dtype_byte;

#define MPI_INT    nanompi_dtype_int
#define MPI_FLOAT  nanompi_dtype_float
#define MPI_DOUBLE nanompi_dtype_double
#define MPI_CHAR   nanompi_dtype_char
#define MPI_BYTE   nanompi_dtype_byte
#define MPI_AINT   ptrdiff_t

int MPI_Type_size(MPI_Datatype datatype, int *size);

#endif
