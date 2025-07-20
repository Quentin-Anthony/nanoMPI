#include "colls.h"
#include "mpi.h"
#include "util.h"

int MPI_Allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf,
                  int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
    int status;
    size_t msg_size;

    msg_size = nanompi_get_msg_size(sendtype, sendcount);

    if (msg_size < 4096) {
        status =
            MPI_Allgather_bruck(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    } else {
        status =
            MPI_Allgather_ring(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    }

    return status;
}

int MPI_Allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf,
                   const int *recvcounts, const int *displs, MPI_Datatype recvtype, MPI_Comm comm) {
    return MPI_Allgatherv_basic(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype,
                                comm);
}

int MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op,
                  MPI_Comm comm) {
    int status;
    size_t msg_size;

    msg_size = nanompi_get_msg_size(datatype, count);

    if (msg_size < 16384) {
        status = MPI_Allreduce_reduce_bcast(sendbuf, recvbuf, count, datatype, op, comm);
    } else {
        status = MPI_Allreduce_ring(sendbuf, recvbuf, count, datatype, op, comm);
    }

    return status;
}

int MPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf,
                 int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
    return MPI_Alltoall_basic(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
}

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    return MPI_Bcast_basic(buffer, count, datatype, root, comm);
}

int MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf,
               int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    return MPI_Gather_basic(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
}

int MPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op,
               int root, MPI_Comm comm) {
    return MPI_Reduce_basic(sendbuf, recvbuf, count, datatype, op, root, comm);
}

int MPI_Reduce_scatter(const void *sendbuf, void *recvbuf, const int recvcounts[],
                       MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    return MPI_Reduce_scatter_basic(sendbuf, recvbuf, recvcounts, datatype, op, comm);
}

int MPI_Scatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf,
                int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
    return MPI_Scatter_basic(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root,
                             comm);
}

int MPI_Barrier(MPI_Comm comm) { return MPI_Barrier_basic(comm); }
