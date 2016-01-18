#include <stdio.h>
#include <stdlib.h>
#include <ossp/uuid.h>

#include "clanchat.h"

int main(int argc, char *argv[]) {
    /* TODO: Actual arg parsing */
    if (argc < 2) {
        printf("Usage: clanchat [USERNAME]\n");
        exit(EXIT_SUCCESS);
    }

    // http://manpages.ubuntu.com/manpages/dapper/man3/uuid.3ossp.html
    uuid_t * uuid;
    uuid_create(&uuid);
    uuid_make(uuid, UUID_MAKE_V1);
    
    char * uuid_str = NULL;
    uuid_export(uuid, UUID_FMT_STR, (void **)&uuid_str, NULL);
    
    ClanchatConfig opts = { argv[1], uuid_str, CLIENT_MODE };
    int retval = clanchat(opts);
    
    uuid_destroy(uuid);
    exit(retval);
}

