#ifndef NANOMPI_MPI_H
#define NANOMPI_MPI_H

#include <stddef.h>
#include <stdint.h>

#include "backends/socket/socket_tag_matching.h"
#include "comm.h"
#include "constants.h"
#include "dtype.h"
#include "group.h"
#include "op.h"
#include "proc.h"
#include "status.h"
#include "colls/colls.h"
#include "util.h"


int MPI_Init(int *argc, char ***argv);
int MPI_Finalize(void);

int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest,
             int tag, MPI_Comm comm);
int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag,
             MPI_Comm comm, MPI_Status *st);

#endif
