#include "mpi.h"
#include "util.h"

#define ARRAY_SIZE 16

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int *send_data = NULL;
  int recv_data[ARRAY_SIZE / size];
  int *gather_data = NULL;

  if (rank == 0) {
    send_data = (int *)malloc(ARRAY_SIZE * sizeof(int));
    gather_data = (int *)malloc(ARRAY_SIZE * sizeof(int));
    for (int i = 0; i < ARRAY_SIZE; i++) {
      send_data[i] = i;
    }
    PRINT_STDOUT("Rank 0: Original data: ");
    for (int i = 0; i < ARRAY_SIZE; i++) {
      PRINT_STDOUT("%d ", send_data[i]);
    }
    PRINT_STDOUT("\n");
  }

  MPI_Scatter(send_data, ARRAY_SIZE / size, MPI_INT, recv_data,
              ARRAY_SIZE / size, MPI_INT, 0, MPI_COMM_WORLD);

  PRINT_STDOUT("Rank %d: Received data: ", rank);
  for (int i = 0; i < ARRAY_SIZE / size; i++) {
    PRINT_STDOUT("%d ", recv_data[i]);
  }
  PRINT_STDOUT("\n");

  MPI_Gather(recv_data, ARRAY_SIZE / size, MPI_INT, gather_data,
             ARRAY_SIZE / size, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    PRINT_STDOUT("Rank 0: Gathered data: ");
    for (int i = 0; i < ARRAY_SIZE; i++) {
      PRINT_STDOUT("%d ", gather_data[i]);
    }
    PRINT_STDOUT("\n");
    free(send_data);
    free(gather_data);
  }

  MPI_Finalize();
  return 0;
}