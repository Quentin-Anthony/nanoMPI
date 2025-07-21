#include "socket_tag_matching.h"
#include "util.h"
#include <sys/ioctl.h>
#include <string.h>

static struct pollfd *fds = NULL;
static nanompi_queue unexpected_list = {
	.size = 0,
	.head = NULL, 
	.tail = NULL 
}; 

struct pollfd *mpi_poll_fd_init(MPI_Comm comm, int size) {
  if (fds == NULL && size > 0) {
    fds = calloc(size, sizeof(struct pollfd));
    if (!fds) {
      PRINT_STDERR("calloc failed in mpi_poll_fd_init\n");
      return NULL;
    }
    for (size_t i = 0; i < (size_t)size; ++i) {
      fds[i].fd = comm->socket_info.client_fds[i];
      fds[i].events = POLLIN;
    }
  }
  return fds;
}

// Free and null‑out the static pointer so later calls are safe
void mpi_poll_fd_destroy(void) {
  free(fds);
  fds = NULL;
}

int mpi_poll_source(MPI_Comm comm) {
  int size = 0;
  MPI_Comm_size(comm, &size);
  struct pollfd *fds = mpi_poll_fd_init(comm, size);
  while (1) {
    if (poll(fds, size, -1) == -1) {
      PRINT_STDERR("first poll MPI_ANY_SOURCE");
      return MPI_ERR_OTHER;
    }
    for (size_t i = 0; i < size; i++) {
      int avaliable_bytes;
      ioctl(fds[i].fd, FIONREAD, &avaliable_bytes);
      if (fds[i].revents == POLLIN && avaliable_bytes > 0) {
        return i;
      }
    }
  }
  // lets just return the first fd which is POLLIN?
  PRINT_STDERR("second poll MPI_ANY_SOURCE");
  return MPI_ERR_OTHER;
}
void enqueue(nanompi_message_envelope env, char * buffer)
{
	nanompi_queue_node * node = calloc(1, sizeof(nanompi_queue_node));   
	memcpy((void*)&node->envelope, (void *)&env, sizeof(nanompi_queue_node)); 
	node->buffer = buffer; 
	node->next = NULL; 
	node->prev =NULL; 
	if(unexpected_list.size == 0)
	{
	unexpected_list.head = unexpected_list.tail = node;	
	unexpected_list.size++; 
	return; 
	}
	if(unexpected_list.size == 1)
	{
	node->prev = unexpected_list.head; 
	unexpected_list.head->next = node; 
	unexpected_list.tail = node; 
	unexpected_list.size++; 
	return; 
	}
	node->prev = unexpected_list.tail; 
	unexpected_list.tail->next = node;
	unexpected_list.tail = node;
	unexpected_list.size++; 
	return;
}
// thought it would look too clutered if i added this to find match
static nanompi_queue_node * dequeue(nanompi_queue_node * remove)
{
	if(remove->next == NULL)
	{
	// at the tail  
	unexpected_list.tail = remove->prev; 
	unexpected_list.size--;
	return remove; 
	}	
	if(remove->prev ==NULL)
	{
	// at the head 
	unexpected_list.head = remove->next; 
	unexpected_list.size--; 
	return remove; 
	}
	remove->next->prev = remove->prev; 
	remove->prev->next = remove->next; 
	unexpected_list.size--; 
	return remove; 
} 
nanompi_queue_node* find_match(nanompi_message_envelope env)
{
	if(unexpected_list.size == 0)
		return NULL; 
	nanompi_queue_node * look = unexpected_list.head;
	while(look!= NULL )
	{
		if(env.tag == look->envelope.tag && env.dest == look->envelope.dest)
		{
		dequeue(look);
		return look; 	
		} 
		look = look->next;  
	}
	return NULL; 

}
