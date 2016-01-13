#include <stdio.h>
#include <string.h>

int clanchat(const char *name) {
    if (strlen(name) < 1) {
        fprintf(stderr, "Name must not be empty.\n");
        return -1;
    }
    return 0;
}
