#include <errno.h>
#include <regex.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "clanchat.h"
#include "clanchatapi.h"

#define BUFSIZE 4096    /* Should be enough to capture a whole line of output from "arp -a" */

pid_t bg_server_pid;

volatile sig_atomic_t done = 0;
 
void term(int signum) {
    kill(bg_server_pid, SIGTERM);
}

void register_sigterm_handler() {
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
}

int run_server_in_child_process(ClanchatConfig opts) {
    bg_server_pid = fork();
    if (bg_server_pid == 0) {
        /* Child process */
        ClanchatConfig server_opts = { opts.name, opts.id, SERVER_MODE };
        return clanchat(server_opts);
    } else if (bg_server_pid < 0) {
        /* Failure */
        fprintf(stderr, "Fork failure.");
        return EXIT_FAILURE;
    }

    register_sigterm_handler();
    return EXIT_SUCCESS;
}

int load_arp_output(char ** arp_output) {
    FILE *fp;
    fp = popen("arp -a", "r");
    if (fp == NULL) {
        fprintf(stderr, "Call to \"arp -a\" failed.");
        return EXIT_FAILURE;
    }

    char buf[BUFSIZE];
    while (fgets(buf, BUFSIZE, fp) != NULL) {
        /* Lines look like this (without surrouding quotation marks):
         * "? (192.168.0.1) at 14:cf:e2:ab:a7:97 on en0 ifscope [ethernet]"
         * "? (192.168.33.255) at (incomplete) on vboxnet0 ifscope [ethernet]"
         */
        *arp_output = realloc(*arp_output, sizeof(char) * (strlen(*arp_output) + strlen(buf)));
        strcat(*arp_output, buf);
    }
    
    if(pclose(fp))  {
        fprintf(stderr, "Error closing pipe to shell.");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int extract_ip_addresses_from_arp_output(char * arp_output, char ** ip_address_list) {
    regex_t regex;
    const char * ipv4_regex_pattern = "([0-9]{1,3}(\\.[0-9]{1,3}){3})";
    int reti = regcomp(&regex, ipv4_regex_pattern, REG_EXTENDED);
    if (reti != 0) {
        fprintf(stderr, "Could not compile regex\n");
        return EXIT_FAILURE;
    }

    /* Use capturing group */
    size_t ngroups = regex.re_nsub + 1;
    regmatch_t *groups = malloc(ngroups * sizeof(regmatch_t));

    char * arp_output_line = malloc(sizeof(arp_output));
    char * arp_remaining = malloc(sizeof(arp_output));

    int ret_status = EXIT_SUCCESS;
    while (1) {
        if (strlen(arp_output) == 0) {
            break;
        }

        char * result;
        result = strstr(arp_output, "\n");
        result = result + sizeof(char); /* Move forward one character: "\n" */
        if (result == NULL || strlen(result) == 0) {
            break;
        }
      
        /* Copy result substring into separate memory space to avoid conflicts */
        arp_remaining = realloc(arp_remaining, sizeof(char) * strlen(result));
        strcpy(arp_remaining, result);

        /* Reallocate and clear memory for next output line. Clearing memory is necessary for strlen to work properly. */
        arp_output_line = realloc(arp_output_line, sizeof(char) * (strlen(arp_output) - strlen(arp_remaining)));
        memset(arp_output_line, 0, sizeof(char) * strlen(arp_output_line));
        strncpy(arp_output_line, arp_output, strlen(arp_output) - strlen(arp_remaining));
        
        /* Update arp_output for next iteration. Clear memory before copying so strlen works properly. */
        arp_output = realloc(arp_output, sizeof(char) * strlen(arp_remaining));
        memset(arp_output, 0, sizeof(char) * strlen(arp_output));
        strcpy(arp_output, arp_remaining);
        
        reti = regexec(&regex, arp_output_line, ngroups, groups, 0);
        if (reti != 0) {
            continue;
        }
        
        char ip[15]; /* IPv4 string is max 15 characters */
        strncpy(ip, arp_output_line + groups[1].rm_so, groups[1].rm_eo - groups[1].rm_so);
        
        if (strlen(*ip_address_list) == 0) {
            *ip_address_list = realloc(*ip_address_list, sizeof(char) * strlen(ip));
            strcpy(*ip_address_list, ip);
        } else {
            *ip_address_list = realloc(*ip_address_list, (sizeof(char) * strlen(*ip_address_list)) + (sizeof(char) * (strlen(ip)+1)));
            strcat(*ip_address_list, ",");
            strcat(*ip_address_list, ip);
        }
    }

    free(arp_remaining);
    free(arp_output_line);
    free(groups);
    regfree(&regex);

    return ret_status;
}

int register_lan_clients() {
    /* Allocate everything up here so we can safely clean up everything using goto statement */
    int ret_status = EXIT_SUCCESS;
    char * arp_output = malloc(0);
    char * ip_address_list = malloc(0);
    
    if ((load_arp_output(&arp_output)) != 0) {
        fprintf(stderr, "Failed to load output for command \"arp -a\"");
        ret_status = EXIT_FAILURE;
        goto cleanup_register_lan_clients;
    }

    if ((extract_ip_addresses_from_arp_output(arp_output, &ip_address_list)) != 0) {
        fprintf(stderr, "Failed to extract IP addresses from arp output.");
        ret_status = EXIT_FAILURE;
        goto cleanup_register_lan_clients;
    }

    /* TODO: Connect to clients with IP addresses and register them */

    cleanup_register_lan_clients:
        free(arp_output);
        free(ip_address_list);

    return ret_status;
}

int listen_for_user_commands(ClanchatConfig opts) {
    printf("clanchat: %s> ", opts.name);
    fflush(stdout);

    char * raw_cmd = malloc(0);
    char * cmd = malloc(0);
    int raw_cmd_is_empty = 1;
    char buffer[4 * sizeof(char)];
    memset(buffer, 0, sizeof(buffer));
    
    while (read(STDIN_FILENO, buffer, sizeof(buffer)) > 0) {
        /* Add buffer contents to raw_cmd */
        if (raw_cmd_is_empty) {
            raw_cmd = realloc(raw_cmd, sizeof(char) * strlen(buffer));
            strcpy(raw_cmd, buffer);
            raw_cmd_is_empty = 0;
        } else {
            raw_cmd = realloc(raw_cmd, sizeof(char) * (strlen(raw_cmd) + strlen(buffer)));
            strcat(raw_cmd, buffer);
        }
       
        /* Clear buffer for next iteration */
        memset(buffer, 0, sizeof(buffer));

        /* Remove trailing EOT from raw_cmd */
        if (raw_cmd[strlen(raw_cmd)-1] == '\x04') {
            /* Copy raw_cmd to cmd without EOT */
            memset(cmd, 0, sizeof(char) * strlen(cmd));
            cmd = realloc(cmd, sizeof(char) * (strlen(raw_cmd) - 1));
            strncpy(cmd, raw_cmd, strlen(raw_cmd) - 1);
            /* Copy cmd back to raw_cmd */
            memset(raw_cmd, 0, sizeof(char) * strlen(raw_cmd));
            raw_cmd = realloc(raw_cmd, sizeof(char) * strlen(cmd));
            strncpy(raw_cmd, cmd, strlen(cmd));
        }

        /* If raw_cmd does not end with new line, keep reading from stdin */
        int new_line_position = strcspn(raw_cmd, "\r\n");
        if (new_line_position == strlen(raw_cmd)) {
            continue;
        }
        
        /* Copy everything except last character (\n) from raw_cmd to cmd */
        memset(cmd, 0, sizeof(char) * strlen(cmd));
        cmd = realloc(cmd, sizeof(char) * new_line_position);
        strncpy(cmd, raw_cmd, new_line_position);

        /* Clear raw_cmd for next iteration */
        memset(raw_cmd, 0, sizeof(char) * strlen(raw_cmd));
        raw_cmd_is_empty = 1;

        /* Parse cmd */
        if (strcmp(cmd, "help") == 0) {
            printf(
                    "Usage: command [args...]\n"
                    "Commands:\n"
                    "\thelp: List commands and descriptions\n"
                    "\tusers: List available users\n"
                    "\tmsg USER: Send a message to USER\n"
                    "\texit: End clanchat session\n"
                    );
            printf("clanchat: %s> ", opts.name);
            fflush(stdout);
        } else if (strcmp(cmd, "users") == 0) {
            /* TODO */
            printf("Command \"users\" not yet implemented...");
            fflush(stdout);
        } else if (strncmp(cmd, "msg ", 4) == 0) {
            /* TODO */
            printf("Command \"msg\" not yet implemented...");
            fflush(stdout);
        } else if (strcmp(cmd, "exit") == 0) {
            printf("Shutting down clanchat server and exiting...");
            fflush(stdout);
            kill(bg_server_pid, SIGTERM);
            goto cleanup_before_exit;
        } else {
            printf("Invalid command: \"%s\". For usage, type \"help\".\n", cmd);
            printf("clanchat: %s> ", opts.name);
            fflush(stdout);
        }
        
        /* Clear cmd for next iteration */
        memset(cmd, 0, sizeof(char) * strlen(cmd));
    }

    cleanup_before_exit:
        free(raw_cmd);
        free(cmd);

    return EXIT_SUCCESS;
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
    
    if (opts.mode == SERVER_MODE) {
        return clanchat_api_serve(opts);
    } else if (opts.mode != CLIENT_MODE) {
        fprintf(stderr, "Invalid mode: \"%d\"\n", opts.mode);
        return EXIT_FAILURE;
    }
    
    /* CLIENT_MODE */

    run_server_in_child_process(opts);
    if (bg_server_pid == 0) {
        return EXIT_SUCCESS;
    }

    register_lan_clients();

    /* TODO: Run register_lan_clients in the background
     * in a sleeping loop and print notifications to stdout
     */
    
    return listen_for_user_commands(opts);
}

