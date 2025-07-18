#include <stdio.h>
#include <string.h>
#include "util.h"
#include "mpi.h"

int main(int argc, char ** argv)
{
    MPI_Init(NULL, NULL);
    char msg[100];
    int dest, tag = 0;
    MPI_Status status;
    int rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
   if(rank ==0) 
    {

	    for(int source = 1; source < world_size; source++)
	    {
		fprintf(stderr, "hi\n");
		MPI_Recv(msg, 100, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
		fprintf(stderr, "%s\n", msg);

	    }

    }
    if(rank != 0)
    {
	    	PRINT_STDOUT("HELLO from %d\n", rank);
		sprintf(msg, "hello from %d",rank);
		dest = 0;
		MPI_Send(msg, 100, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();

	return 0; 

}
