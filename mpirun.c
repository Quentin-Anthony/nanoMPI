#include "util.h"
#include <string.h>
#include <pthread.h>

#include "constants.h"

typedef struct execute_args {
    const char *hostname;
    const char *cmd;
    int rank;
    int size;
    const char *hostfile_path;
} execute_args_t;

void* execute_command_on_host(void *args) {
    char ssh_command[MAX_HOSTNAME_LENGTH] = {0};

    execute_args_t *exe_args = (execute_args_t *) args;

    snprintf(ssh_command,
             sizeof(ssh_command),
             "ssh %s 'export NANOMPI_WORLD_RANK=%d ; export NANOMPI_WORLD_SIZE=%d ; export NANOMPI_HOSTFILE=%s ; export LD_LIBRARY_PATH=%s ; cd %s ; %s'",
             exe_args->hostname,
             exe_args->rank,
             exe_args->size,
             exe_args->hostfile_path,
             getenv("LD_LIBRARY_PATH"),
             getenv("PWD"),
             exe_args->cmd);

#ifndef NDEBUG
    PRINT_STDOUT("%s\n", ssh_command);
#endif

    if (system(ssh_command) == -1) {
        perror("system");
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        PRINT_STDERR("Usage: %s <hostname_file> <command>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    char line[MAX_HOSTNAME_LENGTH] = {0};
    int rank = 0;
    int size = 0;

    // Count how many ranks total
    while (fgets(line, sizeof(line), file)) {
        size++;
    }
    fseek(file, 0, SEEK_SET);

    pthread_t *threads = malloc(size * sizeof(pthread_t));
    if (!threads) {
        PRINT_STDERR("Error allocating threads\n");
        goto close;
    }

    execute_args_t *args = malloc(size * sizeof(execute_args_t));
    if (!args) {
        PRINT_STDERR("Error allocating args\n");
        goto free_threads;
    }

    while (fgets(line, sizeof(line), file)) {
        // Remove newline character from the end of the line
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // Skip empty lines and comment lines
        if (strlen(line) == 0 || line[0] == '#') {
            continue;
        }

        args[rank].hostname = line;
        args[rank].cmd = argv[2];
        args[rank].rank = rank;
        args[rank].size = size;
        args[rank].hostfile_path = filename;

        // Execute the command on the hostname in a thread
        if (pthread_create(&threads[rank], NULL, execute_command_on_host, &args[rank]) != 0) {
            perror("pthread_create");
            goto free_args;
        }

        rank++;
    }

    // Wait for all threads
    for (rank = 0; rank < size; rank++) {
        pthread_join(threads[rank], NULL);
    }

free_args:
    free(args);
free_threads:
    free(threads);
close:
    fclose(file);
    return EXIT_SUCCESS;
}
