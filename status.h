#ifndef NANOMPI_STATUS_H
#define NANOMPI_STATUS_H

/*
 * MPI_Status
 */
typedef struct nanompi_status_t {
  int MPI_SOURCE;
  int MPI_TAG;
  int MPI_ERROR;
} nanompi_status_t;

typedef nanompi_status_t MPI_Status;

#define MPI_STATUS_IGNORE ((MPI_Status *)0)

#endif
