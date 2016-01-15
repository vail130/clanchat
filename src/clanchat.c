#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "clanchat.h"
#include "clanchatapi.h"

pid_t pid;

volatile sig_atomic_t done = 0;
 
void term(int signum) {
    kill(pid, SIGTERM);
}

int clanchat(ClanchatConfig opts) {
    if (strlen(opts.name) < 1) {
        fprintf(stderr, "Name must not be empty.\n");
        return EXIT_FAILURE;
    }

    if (strlen(opts.id) < 1) {
        fprintf(stderr, "ID must not be empty.\n");
        return EXIT_FAILURE;
    }

    if (opts.mode == CLIENT_MODE) {
        pid = fork();
        if (pid == 0) {
            // Child process
            ClanchatConfig server_opts = { opts.name, opts.id, SERVER_MODE };
            return clanchat(server_opts);
        } else if (pid < 0) {
            // Failure
            fprintf(stderr, "Fork failure.");
            return EXIT_FAILURE;
        } else {
            // Parent process
            
            // Register sigterm handler to cleanup child
            struct sigaction action;
            memset(&action, 0, sizeof(struct sigaction));
            action.sa_handler = term;
            sigaction(SIGTERM, &action, NULL);

            // TODO
            int status;
            wait(&status);
            return status;
        }
    } else if (opts.mode == SERVER_MODE) {
        return clanchat_api_serve(opts);
    } else {
        fprintf(stderr, "Invalid mode: \"%d\"\n", opts.mode);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

