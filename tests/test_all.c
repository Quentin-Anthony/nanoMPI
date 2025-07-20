/*
 * Comprehensive integration test that combines the behaviour of:
 *   - test_hello.c
 *   - test_wtime.c
 *   - test_barrier.c
 *   - test_pt2pt.c
 *   - test_self.c
 *   - test_bcast.c
 *   - test_reduce.c
 *   - test_allreduce.c
 *   - test_allreduce_ring.c
 *   - test_scatter_gather.c
 *   - test_alltoall.c
 *
 * In a single file so that we can easily perform regression checks, and keep it
 * all in a single MPI_Init.
 */

#include "mpi.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  /* ------------------------------------------------------------------ *
   * 1. “Hello world”                                                    *
   * ------------------------------------------------------------------ */
  PRINT_STDOUT("Hello world from rank %d of %d\n", rank, size);

  /* ------------------------------------------------------------------ *
   * 2. MPI_Wtime sanity‑check                                          *
   * ------------------------------------------------------------------ */
  double t0 = MPI_Wtime();
  sleep(1);
  PRINT_STDOUT("Rank %d: 1‑second sleep measured %f s\n", rank,
               MPI_Wtime() - t0);

  t0 = MPI_Wtime();
  for (size_t i = 0; i < 100000; ++i) { /* busy loop */
  }
  PRINT_STDOUT("Rank %d: 100 000 increments in %f s\n", rank, MPI_Wtime() - t0);

  /* ------------------------------------------------------------------ *
   * 3. Barrier (rank‑0 delay)                                           *
   * ------------------------------------------------------------------ */
  if (rank == 0) {
    sleep(5);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  PRINT_STDOUT("Rank %d: passed barrier\n", rank);

  /* ------------------------------------------------------------------ *
   * 4. Point‑to‑point demo                                             *
   * ------------------------------------------------------------------ */
  if (rank != 0) {
    char msg[100];
    sprintf(msg, "Greetings from process %d!", rank);
    MPI_Send(msg, strlen(msg) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
  } else {
    for (int src = 1; src < size; ++src) {
      char msg[100];
      MPI_Recv(msg, 100, MPI_CHAR, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      PRINT_STDOUT("Rank 0 received: %s\n", msg);
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);

  /* ------------------------------------------------------------------ *
   * 5. Self send/recv                                                  *
   * ------------------------------------------------------------------ */
  {
    char msg[100];
    sprintf(msg, "Greetings from myself %d!", rank);
    MPI_Send(msg, strlen(msg) + 1, MPI_CHAR, rank, 0, MPI_COMM_WORLD);
    memset(msg, 0, sizeof(msg));
    MPI_Recv(msg, 100, MPI_CHAR, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    PRINT_STDOUT("Rank %d self‑comm: %s\n", rank, msg);
  }

  /* ------------------------------------------------------------------ *
   * 6. Broadcast                                                       *
   * ------------------------------------------------------------------ */
  {
    const int N = 5;
    int buf[N];
    if (rank == 0) {
      for (int i = 0; i < N; ++i)
        buf[i] = i * 10;
      PRINT_STDOUT("Rank 0 broadcasting data\n");
    }
    MPI_Bcast(buf, N, MPI_INT, 0, MPI_COMM_WORLD);

    PRINT_STDOUT("Rank %d broadcast recv: ", rank);
    for (int i = 0; i < N; ++i)
      PRINT_STDOUT("%d ", buf[i]);
    PRINT_STDOUT("\n");
  }

  /* ------------------------------------------------------------------ *
   * 7. Reduce                                                          *
   * ------------------------------------------------------------------ */
  {
    const int N = 5;
    int send[N], recv[N];
    for (int i = 0; i < N; ++i)
      send[i] = rank * 10 + i;
    MPI_Reduce(send, recv, N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0) {
      PRINT_STDOUT("Rank 0 reduce sum: ");
      for (int i = 0; i < N; ++i)
        PRINT_STDOUT("%d ", recv[i]);
      PRINT_STDOUT("\n");
    }
  }

  /* ------------------------------------------------------------------ *
   * 8. Allreduce (default)                                             *
   * ------------------------------------------------------------------ */
  {
    const int N = 5;
    int send[N], recv[N];
    for (int i = 0; i < N; ++i)
      send[i] = rank * 10 + i;
    MPI_Allreduce(send, recv, N, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    PRINT_STDOUT("Rank %d allreduce sum: ", rank);
    for (int i = 0; i < N; ++i)
      PRINT_STDOUT("%d ", recv[i]);
    PRINT_STDOUT("\n");
  }

  /* ------------------------------------------------------------------ *
   * 9. Allreduce (ring algorithm—your custom impl)                     *
   * ------------------------------------------------------------------ */
  {
    const int N = 5;
    int send[N], recv[N];
    for (int i = 0; i < N; ++i)
      send[i] = rank * 10 + i;
    MPI_Allreduce_ring(send, recv, N, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    PRINT_STDOUT("Rank %d allreduce_ring sum: ", rank);
    for (int i = 0; i < N; ++i)
      PRINT_STDOUT("%d ", recv[i]);
    PRINT_STDOUT("\n");
  }

  /* ------------------------------------------------------------------ *
   * 10. Scatter / Gather                                               *
   * ------------------------------------------------------------------ */
  {
    const int N = 16;
    int *scatter_buf = NULL, *gather_buf = NULL;
    int recv[N / size];

    if (rank == 0) {
      scatter_buf = malloc(N * sizeof(int));
      gather_buf = malloc(N * sizeof(int));
      for (int i = 0; i < N; ++i)
        scatter_buf[i] = i;
    }

    MPI_Scatter(scatter_buf, N / size, MPI_INT, recv, N / size, MPI_INT, 0,
                MPI_COMM_WORLD);

    PRINT_STDOUT("Rank %d scatter recv: ", rank);
    for (int i = 0; i < N / size; ++i)
      PRINT_STDOUT("%d ", recv[i]);
    PRINT_STDOUT("\n");

    MPI_Gather(recv, N / size, MPI_INT, gather_buf, N / size, MPI_INT, 0,
               MPI_COMM_WORLD);

    if (rank == 0) {
      PRINT_STDOUT("Rank 0 gathered: ");
      for (int i = 0; i < N; ++i)
        PRINT_STDOUT("%d ", gather_buf[i]);
      PRINT_STDOUT("\n");
      free(scatter_buf);
      free(gather_buf);
    }
  }

  /* ------------------------------------------------------------------ *
   * 11. All‑to‑all                                                     *
   * ------------------------------------------------------------------ */
  {
    const int N = 16;
    int send[N], recv[N];
    for (int i = 0; i < N; ++i)
      send[i] = rank * 100 + i;

    MPI_Alltoall(send, N / size, MPI_INT, recv, N / size, MPI_INT,
                 MPI_COMM_WORLD);

    PRINT_STDOUT("Rank %d alltoall recv: ", rank);
    for (int i = 0; i < N; ++i)
      PRINT_STDOUT("%d ", recv[i]);
    PRINT_STDOUT("\n");
  }

  MPI_Finalize();
  return 0;
}
