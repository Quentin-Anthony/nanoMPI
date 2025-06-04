#ifndef NANOMPI_MPI_H
#define NANOMPI_MPI_H

#include <stddef.h>
#include <stdint.h>

#include "op.h"
#include "comm.h"
#include "proc.h"
#include "group.h"
#include "status.h"
#include "dtype.h"
#include "constants.h"
#include "colls/colls.h"
#include "util.h"

int MPI_Init(int *argc, char ***argv);
int MPI_Finalize(void);

int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest,
                            int tag, MPI_Comm comm);
int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source,
                            int tag, MPI_Comm comm, MPI_Status *st);

#endif
