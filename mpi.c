#include <string.h>

#include "mpi.h"

#include "util.h"
#include "backends/socket/socket_backend.h"
#include "backends/self/self_backend.h"

int MPI_Init(int *argc, char ***argv)
{
    int status = MPI_SUCCESS;
    int my_rank = atoi(getenv("NANOMPI_WORLD_RANK"));
    int world_size = atoi(getenv("NANOMPI_WORLD_SIZE"));
    char *hostfile = getenv("NANOMPI_HOSTFILE");

    // For MPI_Wtime
    nanompi_init_clock();

    status = nanompi_init_comm(&nanompi_comm_world, my_rank, world_size, hostfile);
    if (status) {
        PRINT_STDERR("Error in nanompi_init_comm\n");
    }

    return status;
}

int MPI_Finalize(void)
{
    int status;
    
    status = nanompi_free_comm(nanompi_comm_world);
    if (status) {
        PRINT_STDERR("Error in nanompi_free_comm\n");
    }
    
    return status;
}

// TODO: tags, generate a message envelope with match info so the recv side can match
int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
    int status = MPI_SUCCESS;

    // Send envelope first, then user buffer
    size_t msg_size = nanompi_get_msg_size(datatype, count);
    nanompi_message_envelope envelope = {
        .sizeof_buffer = msg_size,
        .tag            = tag
    };
    int rank = comm->my_rank;
    if (rank == dest) {
        status = nanompi_self_send(buf, count, datatype, dest, tag, comm);
    } else {
        // send envelope
        status = nanompi_socket_send(&envelope, sizeof(envelope), dest, comm);
        // send payload only if the envelope was sent successfully
        if (status == MPI_SUCCESS) {
            status = nanompi_socket_send(buf, msg_size, dest, comm);
        }
    }
    return status;
}

// TODO: tags, MPI_ANY_SOURCE, matching with a variable (but lesser than) count that the user passed, etc.
int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *st)
{
    int status = MPI_SUCCESS;
    size_t msg_size = nanompi_get_msg_size(datatype, count);
    int rank = comm->my_rank;
    if (rank == source) {
        status = nanompi_self_recv(buf, count, datatype, source, tag, comm);
    }
    else if(source == MPI_ANY_SOURCE)
    {
	int any_source = mpi_poll_source(comm);
	status = nanompi_socket_recv(buf, msg_size, any_source, comm); 
    }
    else {
        status = nanompi_socket_recv(buf, msg_size, source, comm);
    }
    if(st) {
        st->MPI_ERROR = status;
        st->MPI_SOURCE = source;
        st->MPI_TAG = tag;
    }
    return status;
}
