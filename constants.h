#ifndef NANOMPI_CONSTANTS_H
#define NANOMPI_CONSTANTS_H

/* 
 *  MPI Standard
 */
#define MAX_HOSTNAME_LENGTH 256
//#define NDEBUG // Comment for debug prints and asserts

/* 
 *  MPI Standard
 */
#define MPI_VERSION 3
#define MPI_SUBVERSION 1

/*
 * Error classes and codes
 */
#define MPI_SUCCESS                   0
#define MPI_ERR_OTHER                 16
#define MPI_ERR_UNSUPPORTED_OPERATION 52

/*
 * Socket info--random port
 */
#define PORT 19113


#endif
