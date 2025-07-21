.PHONY: all clean tests benchmarks

# Compiler settings
CC = gcc
CXX = g++
CPPFLAGS = -g -fPIC -O0 -I.
LDFLAGS = -shared

# Source files
SOURCES_C = mpi.c comm.c group.c proc.c op.c dtype.c util.c \
            backends/socket/socket_backend.c \
            colls/colls.c colls/allgather/allgather.c colls/allgatherv/allgatherv.c \
            colls/allreduce/allreduce.c colls/alltoall/alltoall.c colls/bcast/bcast.c \
            colls/gather/gather.c colls/reduce/reduce.c colls/reduce_scatter/reduce_scatter.c \
            colls/scatter/scatter.c colls/barrier/barrier.c

SOURCES_CPP = backends/self/self_backend.cpp

# Object files
OBJECTS_C = $(SOURCES_C:.c=.o)
OBJECTS_CPP = $(SOURCES_CPP:.cpp=.o)

# Test executables
TEST_TARGETS = tests/test_hello tests/test_pt2pt tests/test_bcast tests/test_reduce \
               tests/test_allreduce tests/test_allreduce_ring tests/test_scatter_gather \
               tests/test_alltoall tests/test_self tests/test_wtime tests/test_barrier

# Benchmark executables
BENCHMARK_TARGETS = benchmarks/benchmark_allreduce

# Main targets
all: libmpi.so mpirun benchmarks tests

clean:
	rm -f $(OBJECTS_C) $(OBJECTS_CPP) libmpi.so mpirun $(TEST_TARGETS) $(BENCHMARK_TARGETS)

# Library
libmpi.so: $(OBJECTS_C) $(OBJECTS_CPP)
	$(CXX) $(CPPFLAGS) $(OBJECTS_C) $(OBJECTS_CPP) -o $@ $(LDFLAGS)

# MPI runner
mpirun: mpirun.c libmpi.so
	$(CC) -g -o $@ mpirun.c -L. -I. -lpthread

# Benchmarks
benchmarks: $(BENCHMARK_TARGETS)

benchmarks/benchmark_allreduce: benchmarks/benchmark_allreduce.c libmpi.so
	$(CC) -g -o $@ $< -L. -I. -lmpi

# Tests
tests: $(TEST_TARGETS)

tests/test_%: tests/test_%.c libmpi.so
	$(CC) -g -o $@ $< -L. -I. -lmpi

# Formatting
FMT_SRCS := $(SOURCES_C) $(SOURCES_CPP) $(wildcard tests/*.c)

.PHONY: format check-format tidy

format:      ## Rewrites code in place
	clang-format -i $(FMT_SRCS)

check-format:  ## Fails if any file would be re‑formatted
	clang-format --dry-run --Werror $(FMT_SRCS)

tidy:         ## Runs clang‑tidy using compile_commands.json
	@command -v bear >/dev/null || (echo "bear not found - sudo apt install bear"; exit 1)
	bear -- make -j$(nproc) libmpi.so
	run-clang-tidy -p . -quiet -config=.clang-tidy
