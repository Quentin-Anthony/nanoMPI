#include <mpi.h>
#include <math.h>

#define NUM_RUNS 1
#define MAX_MESSAGE_SIZE 1048576  // 1 MB
#define MIN_MESSAGE_SIZE 8        // 8 bytes


double benchmark_allreduce(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    int rank;
    MPI_Comm_rank(comm, &rank);

    // Warm-up run
    MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);

    // Synchronize all processes
    MPI_Barrier(comm);

    double start_time = MPI_Wtime();

    for (int i = 0; i < NUM_RUNS; i++) {
        MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
    }

    double end_time = MPI_Wtime();

    // Average time per operation in microseconds
    return (end_time - start_time) * 1e6 / NUM_RUNS;
}

// Returns 1 if buffer was valid, 0 otherwise
int data_validate(int *recvbuf, int count, int size)
{
    for (int i = 0; i < count; i++) {
        if (recvbuf[i] != size) {
            return 0;
        }
    }
    return 1;
}

void data_reset(int *recvbuf, int count)
{
    for (int i = 0; i < count; i++) {
        recvbuf[i] = 0;
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Allocate buffers for the maximum message size
    int* sendbuf = (int*)malloc(MAX_MESSAGE_SIZE);
    int* recvbuf = (int*)malloc(MAX_MESSAGE_SIZE);

    if (sendbuf == NULL || recvbuf == NULL) {
        PRINT_STDERR("Error: Could not allocate memory\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Initialize send buffer
    for (int i = 0; i < MAX_MESSAGE_SIZE / sizeof(int); i++) {
        sendbuf[i] = 1;
    }

    if (rank == 0) {
        PRINT_STDOUT("%-25s %-20s %-20s %-20s\n", "Message Size (bytes)", "Latency (us)", "Bus BW (MB/s)", "Validation");
    }

    for (int message_size = MIN_MESSAGE_SIZE; message_size <= MAX_MESSAGE_SIZE; message_size *= 2) {
        int count = message_size / sizeof(int);
        double time = benchmark_allreduce(sendbuf, recvbuf, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

        double busBW = ((message_size/1e6) / (time/1e6)) * (2*(size-1)/size); // Allreduce bus bandwidth = alg_bw * (2*(n-1)/n), from https://github.com/NVIDIA/nccl-tests/blob/master/doc/PERFORMANCE.md

        int valid = data_validate(recvbuf, count, size);
        data_reset(recvbuf, count);

        // Only the root process prints the results
        if (rank == 0) {
            PRINT_STDOUT("%-25d %-20.2f %-20.4f %-20s\n", message_size, time, busBW, valid ? "PASS" : "FAIL");
        }
    }

    free(sendbuf);
    free(recvbuf);

    MPI_Finalize();
    return 0;
}

