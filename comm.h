#ifndef NANOMPI_COMM_H
#define NANOMPI_COMM_H

#include "group.h"

typedef struct nanompi_communicator_t {
    int              my_rank;
    nanompi_group_t *local_group;
    nanompi_socket_info_t socket_info;
} nanompi_communicator_t;

typedef nanompi_communicator_t *MPI_Comm;

/* Comm world */
extern MPI_Comm nanompi_comm_world;
#define MPI_COMM_WORLD nanompi_comm_world

/* Comm NULL */
extern MPI_Comm nanompi_comm_null;
#define MPI_COMM_NULL nanompi_comm_null

int MPI_Comm_rank(MPI_Comm comm, int *rank);
int MPI_Comm_size(MPI_Comm comm, int *size);

int nanompi_init_comm(nanompi_communicator_t **comm_dptr, int rank, int world_size, char *hostfile);
int nanompi_free_comm(nanompi_communicator_t *comm);

#endif
