#include "socket_tag_matching.h"
#include <sys/ioctl.h>
#include "util.h"
struct pollfd * mpi_poll_fd_init(MPI_Comm comm, int size)
{
	static struct pollfd * fds = NULL;
	if(fds == NULL){
	fds = calloc(size, sizeof(struct pollfd));
	for(size_t i=0; i<size; i++ ){
		fds[i].fd = comm->socket_info.client_fds[i];
		fds[i].events = POLLIN; 
	}
	}
	return fds;
}

int mpi_poll_source(MPI_Comm comm)
{
	int size =0;
	MPI_Comm_size(comm, &size);
	struct pollfd * fds = mpi_poll_fd_init(comm,size);
	while(1)
	{
	if(poll(fds, size, -1) ==-1)
		{
		PRINT_STDERR("first poll MPI_ANY_SOURCE");	
		return MPI_ERR_OTHER;
		}
	for(size_t i=0; i< size; i++)
		{
		int avaliable_bytes; 
		ioctl(fds[i].fd, FIONREAD,&avaliable_bytes);
		if(fds[i].revents == POLLIN && avaliable_bytes > 0)
		{
			return i;
		}
		}	
	}
	// lets just return the first fd which is POLLIN? 
	PRINT_STDERR("second poll MPI_ANY_SOURCE");
	return MPI_ERR_OTHER;


}
