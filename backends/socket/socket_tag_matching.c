#include "socket_tag_matching.h"
#include <sys/ioctl.h>
#include "util.h"

static struct pollfd *fds = NULL;

struct pollfd *mpi_poll_fd_init(MPI_Comm comm, int size)
{
    if (fds == NULL && size > 0) {
        fds = calloc(size, sizeof(struct pollfd));
        if (!fds) {
            PRINT_STDERR("calloc failed in mpi_poll_fd_init\n");
            return NULL;
        }
        for (size_t i = 0; i < (size_t)size; ++i) {
            fds[i].fd     = comm->socket_info.client_fds[i];
            fds[i].events = POLLIN;
        }
    }
    return fds;
}

// Free and null‑out the static pointer so later calls are safe
void mpi_poll_fd_destroy(void)
{
    free(fds);
    fds = NULL;
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
