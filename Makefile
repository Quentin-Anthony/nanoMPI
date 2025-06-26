.PHONY : clean

CC = gcc
CXX = g++
CPPFLAGS = -g -fPIC -O0 -I.
LDFLAGS = -shared

SOURCES_C = mpi.c comm.c group.c proc.c op.c dtype.c util.c backends/socket/socket_backend.c colls/colls.c colls/allgather/allgather.c colls/allgatherv/allgatherv.c colls/allreduce/allreduce.c colls/alltoall/alltoall.c colls/bcast/bcast.c colls/gather/gather.c colls/reduce/reduce.c colls/reduce_scatter/reduce_scatter.c colls/scatter/scatter.c colls/barrier/barrier.c
SOURCES_CPP = backends/self/self_backend.cpp
OBJECTS_CPP = $(SOURCES_CPP:.cpp=.o)
OBJECTS_C = $(SOURCES_C:.c=.o) 

all: libmpi.so mpirun benchmarks tests

clean:
	rm -f $(OBJECTS_C) $(OBJECTS_CPP) libmpi.so mpirun benchmarks/benchmark_allreduce tests/test_hello tests/test_pt2pt tests/test_bcast tests/test_reduce tests/test_allreduce tests/test_allreduce_ring tests/test_scatter_gather tests/test_alltoall tests/test_self tests/test_wtime tests/test_barrier 

libmpi.so: $(OBJECTS_C) $(OBJECTS_CPP)
	$(CXX) $(CPPFLAGS) $(OBJECTS_C) $(OBJECTS_CPP) -o $@ $(LDFLAGS)

mpirun: mpirun.c libmpi.so
	$(CC) -g -o $@ mpirun.c -L. -I. -lpthread

benchmarks: benchmark_allreduce

benchmark_allreduce: libmpi.so
	$(CC) -g -o benchmarks/$@ benchmarks/benchmark_allreduce.c -L. -I. -lmpi

tests: test_hello test_pt2pt test_bcast test_reduce test_allreduce test_allreduce_ring test_scatter_gather test_alltoall test_self test_wtime test_barrier

test_hello: libmpi.so
	$(CC) -g -o tests/$@ tests/test_hello.c -L. -I. -lmpi

test_pt2pt: libmpi.so
	$(CC) -g -o tests/$@ tests/test_pt2pt.c -L. -I. -lmpi

test_bcast: libmpi.so
	$(CC) -g -o tests/$@ tests/test_bcast.c -L. -I. -lmpi

test_reduce: libmpi.so
	$(CC) -g -o tests/$@ tests/test_reduce.c -L. -I. -lmpi

test_allreduce: libmpi.so
	$(CC) -g -o tests/$@ tests/test_allreduce.c -L. -I. -lmpi

test_allreduce_ring: libmpi.so
	$(CC) -g -o tests/$@ tests/test_allreduce_ring.c -L. -I. -lmpi

test_scatter_gather: libmpi.so
	$(CC) -g -o tests/$@ tests/test_scatter_gather.c -L. -I. -lmpi

test_alltoall: libmpi.so
	$(CC) -g -o tests/$@ tests/test_alltoall.c -L. -I. -lmpi

test_self: libmpi.so
	$(CC) -g -o tests/$@ tests/test_self.c -L. -I. -lmpi

test_wtime: libmpi.so
	$(CC) -g -o tests/$@ tests/test_wtime.c -L. -I. -lmpi

test_barrier: libmpi.so
	$(CC) -g -o tests/$@ tests/test_barrier.c -L. -I. -lmpi
