#include "util.h"
#include "comm.h"
#include "backends/socket/socket_backend.h"

MPI_Comm nanompi_comm_world;
MPI_Comm nanompi_comm_null;

int MPI_Comm_rank(MPI_Comm comm, int *rank)
{
    *rank = comm->my_rank;
    return MPI_SUCCESS;
}

int MPI_Comm_size(MPI_Comm comm, int *size)
{
    *size = comm->local_group->grp_proc_count;
    return MPI_SUCCESS;
}

int nanompi_init_comm(nanompi_communicator_t **comm_dptr, int rank, int world_size, char *hostfile)
{
    int status = MPI_SUCCESS;

    // MPI_COMM_NULL
    nanompi_comm_null = NULL;

    // MPI_COMM_WORLD
    nanompi_comm_world = (nanompi_communicator_t*) malloc(sizeof(nanompi_communicator_t));
    if (!nanompi_comm_world) {
        PRINT_STDOUT("Error allocating nanompi_communicator_t\n");
        return MPI_ERR_OTHER;
    }

    nanompi_comm_world->my_rank = rank;

    status = nanompi_init_group(&nanompi_comm_world->local_group, rank, world_size, hostfile);
    if (status) {
        PRINT_STDOUT("error in nanompi_init_group: %d\n", status);
        goto free_comm_world;
    }

    status = nanompi_init_socket_backend(nanompi_comm_world);
    if (status) {
        PRINT_STDOUT("error in nanompi_init_socket_backend: %d\n", status);
        goto free_group;
    }

exit:
    return status;
free_group:
    status = nanompi_free_group(nanompi_comm_world->local_group);
    if (status) {
        PRINT_STDOUT("error while freeing group\n");
    }
free_comm_world:
    free(nanompi_comm_world);
    goto exit;
}

int nanompi_free_comm(nanompi_communicator_t *comm)
{
    int status = MPI_SUCCESS;

    status = nanompi_free_socket_backend(comm);
    if (status) {
        PRINT_STDOUT("Error freeing comm socket backend\n");
    }

    status = nanompi_free_group(comm->local_group);
    if (status) {
        PRINT_STDOUT("Error freeing comm->local_group\n");
    }

    free(comm);
    return status;
}
