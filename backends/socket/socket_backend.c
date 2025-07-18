#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <errno.h>

#include "socket_backend.h"
#include "constants.h"

#include "util.h"
#include "comm.h"


static int init_server(nanompi_communicator_t *comm)
{
    struct sockaddr_in address;
    int status = MPI_SUCCESS;
    int rank = comm->my_rank;
    int size = comm->local_group->grp_proc_count;
    nanompi_proc_t **grp_proc_pointers = comm->local_group->grp_proc_pointers;
    int addrlen = sizeof(address);
    int opt = 1;
    int i;

    // Create socket file descriptor
    if ((comm->socket_info.server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        status = MPI_ERR_OTHER;
        goto exit;
    }

    // Try to reuse addr and port to avoid pesky bind: address in use errors >:(
    // This is especially useful right now because the job launcher does not forward Ctrl+C signals to each process.
    // That means after every run that errors or is Ctrl+C'd out, we get more stray processes
    // TODO: It seems like this doesn't always work
    if (setsockopt(comm->socket_info.server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        status = MPI_ERR_OTHER;
        goto close;
    }

    // Turn off Nagle's algorithm
    // Nagle's algorithm is an OS-level buffering solution. The idea is for chat-like applications where a user
    // is typing in one byte at a time, the socket can coalesce (i.e., merge multiple small sends into one large send)
    // these bytes to massively increase throughput by reducing the amount of control messages going back and forth.
    // We want it off so that for small messages, it doesn't wait for the timeout before sending, messing up the
    // latency measurement.
    if (setsockopt(comm->socket_info.server_fd, IPPROTO_TCP, TCP_NODELAY, (char *) &opt, sizeof(opt))) {
        perror("setsockopt");
        status = MPI_ERR_OTHER;
        goto close;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(grp_proc_pointers[rank]->port);

    if (bind(comm->socket_info.server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        char bind_msg[1024] = {'\0'};
        snprintf( bind_msg, 1023, "function %s, file %s, line %d, rank %d, port %d, bind error", __func__, __FILE__, __LINE__, rank, grp_proc_pointers[rank]->port);
        perror(bind_msg);
        status = MPI_ERR_OTHER;
        goto close;
    }

    if (listen(comm->socket_info.server_fd, size) < 0) {
        perror("server: listen");
        status = MPI_ERR_OTHER;
        goto close;
    }

    for(i = rank - 1; i >= 0; i--) {
        // Here "address" is a wildcard--meaning accept a connection from any address. However, since connect/accept is blocking and we "connect"
        // to the next higher/lower rank, we fill out client_fds in rank order
        // e.g. ranks are 0 1 2
        // 0 connects to 1 and blocks
        // 1 connects to 2 and blocks
        // 2 accepts 1, unblocking 1. 1 is in rank order in 2's client_fds
        // 2 accepts 0, 0 is in rank order in 2's client_fds
        // 1 accepts 0, 0 is in rank order in 1's client_fds
        // TODO: is this explanation necessary?
        comm->socket_info.client_fds[i] = accept(comm->socket_info.server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    }

exit:
    return status;
close:
    close(comm->socket_info.server_fd);
    goto exit;
}

static int init_clients(nanompi_communicator_t *comm)
{
    int status = MPI_SUCCESS;
    int rank = comm->my_rank;
    int size = comm->local_group->grp_proc_count;
    nanompi_proc_t **grp_proc_pointers = comm->local_group->grp_proc_pointers;
    struct addrinfo *res = NULL;
    int i, err;
    struct addrinfo hints;
    struct sockaddr_in *sa_in;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  // IPv4

    // Make a client socket for every rank greater than mine. Last rank just returns
    // without doing anything
    for(i = rank + 1; i < size; i++) {
        // Get ip of hostname (resolve dns)
        if (err = getaddrinfo(grp_proc_pointers[i]->hostname, NULL, &hints, &res) != 0) {
            printf("getaddrinfo failed: %s\n", gai_strerror(err));
            goto free;
        }

        if ((comm->socket_info.client_fds[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("client: socket creation error");
            goto free;
        }

        // Modify the port in the resolved address
        sa_in = (struct sockaddr_in *)res->ai_addr;
        sa_in->sin_port = htons(grp_proc_pointers[i]->port);

        // This client may have reached here before the server called accept(), so just keep trying
        while (connect(comm->socket_info.client_fds[i], res->ai_addr, res->ai_addrlen));

        freeaddrinfo(res);
        res = NULL;
    }

exit:
    return status;
free:
    if (res) {
        freeaddrinfo(res);
    }
    for(i--; i >= rank + 1; i--) {
        close(comm->socket_info.client_fds[i]);
    }
    status = MPI_ERR_OTHER;
    goto exit;
}

int nanompi_init_socket_backend(nanompi_communicator_t *comm)
{
    int status = MPI_SUCCESS;
    int size = comm->local_group->grp_proc_count;

    comm->socket_info.client_fds = (int*) malloc(sizeof(int) * size);
    if (!comm->socket_info.client_fds) {
        PRINT_STDERR("Error mallocing client fds\n");
        status = MPI_ERR_OTHER;
    }

    status = init_clients(comm);
    if (status) {
        PRINT_STDERR("Error in init_clients\n");
        goto free;
    }
    status = init_server(comm);
    if (status) {
        PRINT_STDERR("Error in init_server\n");
        goto free;
    }

exit:
    return status;
free:
    free(comm->socket_info.client_fds);
    comm->socket_info.client_fds = NULL;
    goto exit;
}

int nanompi_free_socket_backend(nanompi_communicator_t *comm)
{
    int status = MPI_SUCCESS;
    int size = comm->local_group->grp_proc_count;
    int rank = comm->my_rank;
    int i;

    for (i = 0; i < size; i++) {
        close(comm->socket_info.client_fds[i]);
    }
    free(comm->socket_info.client_fds);

    close(comm->socket_info.server_fd);

    return status;
}

int nanompi_socket_send(const void *buffer, size_t msg_size, int to_rank, nanompi_communicator_t *comm)
{
    int status = MPI_SUCCESS;
    size_t sent_bytes = 0;
    
    

    while (sent_bytes != msg_size) {
        sent_bytes += send(comm->socket_info.client_fds[to_rank], buffer + sent_bytes, msg_size - sent_bytes, 0);
    }

    return status;
}

int nanompi_socket_recv(void *buffer, size_t msg_size, int from_rank, nanompi_communicator_t *comm)
{
    int status = MPI_SUCCESS;
    size_t recv_bytes = 0;
    nanompi_message_envelope message_envelope; 
/*    while (recv_bytes != msg_size) {
        recv_bytes += recv(comm->socket_info.client_fds[from_rank], buffer + recv_bytes, msg_size - recv_bytes, 0);
	if(recv_bytes > sizeof())
    }*/
    while (recv_bytes != sizeof(nanompi_message_envelope))
	    recv_bytes += recv(comm->socket_info.client_fds[from_rank], (char*)&message_envelope+ recv_bytes, sizeof(nanompi_message_envelope) - recv_bytes, 0);
    recv_bytes = 0;
    while(recv_bytes != message_envelope.sizeof_buffer)
    {
        recv_bytes += recv(comm->socket_info.client_fds[from_rank], buffer + recv_bytes, message_envelope.sizeof_buffer - recv_bytes, 0);

    }
    return status;
}
