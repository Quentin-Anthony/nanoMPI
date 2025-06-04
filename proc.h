#ifndef NANOMPI_PROC_H
#define NANOMPI_PROC_H

#include "backends/socket/socket_backend.h"
#include "constants.h"

typedef struct nanompi_proc_t {
    char hostname[MAX_HOSTNAME_LENGTH];
    int port;
} nanompi_proc_t;

#endif