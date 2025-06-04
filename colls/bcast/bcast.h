#ifndef NANOMPI_BCAST_H
#define NANOMPI_BCAST_H

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
int MPI_Bcast_basic(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);

#endif
