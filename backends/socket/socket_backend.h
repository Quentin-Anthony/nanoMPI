#ifndef NANOMPI_SOCKET_BACKEND_H
#define NANOMPI_SOCKET_BACKEND_H

#include <stddef.h>

typedef struct nanompi_communicator_t nanompi_communicator_t;

/*
* Somebody has to be the client and somebody has to be the server
* with sockets. So, decide who does what based on rank.
* If my rank is greater than another rank, let the lesser rank
* connect to me as the client.
*/
typedef struct nanompi_socket_info_t {
    int server_fd; // One socket for the server
    int *client_fds; // To greater ranks, this is the fd returned by socket(). To lesser ranks, this is the handle returned by accept()
} nanompi_socket_info_t;

int nanompi_init_socket_backend(nanompi_communicator_t *comm);
int nanompi_free_socket_backend(nanompi_communicator_t *comm);

int nanompi_socket_send(const void *buffer, size_t msg_size, int to_rank, nanompi_communicator_t *comm);
int nanompi_socket_recv(void *buffer, size_t msg_size, int from_rank, nanompi_communicator_t *comm);

#endif
