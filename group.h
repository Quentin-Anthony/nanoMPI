#ifndef NANOMPI_GROUP_H
#define NANOMPI_GROUP_H

#include "proc.h"

typedef struct nanompi_group_t {
    int grp_proc_count;     /**< number of processes in group */
    int grp_my_rank;        /**< rank in group */
    nanompi_proc_t **grp_proc_pointers;
    struct nanompi_group_t *grp_parent_group_ptr;
} nanompi_group_t;

int nanompi_init_group(nanompi_group_t **group_dptr, int rank, int world_size, char *hostfile);
int nanompi_free_group(nanompi_group_t *group);

#endif
