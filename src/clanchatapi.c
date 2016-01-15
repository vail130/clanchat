#define PORT 37777

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>

#include <microhttpd.h>

#include "clanchat.h"

static int http_router(void * cls, struct MHD_Connection * connection, const char * url,
                       const char * method, const char * version, const char * upload_data,
                       size_t * upload_data_size, void ** ptr) {
    static int dummy;
    char * response_string = cls;
    struct MHD_Response * response;
    int ret;
    
    if (0 != strcmp(method, "GET")) {
        // Not supporting GET
        return MHD_NO;
    }

    if (&dummy != *ptr) {
      /* The first time only the headers are valid,
         do not respond in the first round... */
      *ptr = &dummy;
      return MHD_YES;
    }

    if (0 != *upload_data_size) {
        // Upload data not allowed
        return MHD_NO;
    }
    
    *ptr = NULL; // Clear context pointer
    
    response = MHD_create_response_from_buffer(strlen(response_string), (void*) response_string, MHD_RESPMEM_PERSISTENT);
    MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, "application/json");
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

int clanchat_api_serve(const char *name, ClanchatConfig opts) {
    struct MHD_Daemon *d;
    
    char response_string[28 + strlen(opts.name) + strlen(opts.id)];
    strcpy(response_string, "{\"data\":{\"name\":\"");
    strcat(response_string, opts.name);
    strcat(response_string, "\",\"id\":\"");
    strcat(response_string, opts.id);
    strcat(response_string, "\"}}");

    d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, PORT, NULL, NULL, &http_router, &response_string, MHD_OPTION_END);
    if (d == NULL) {
        return EXIT_FAILURE;
    }
    while (1) {}
    MHD_stop_daemon(d);
    return EXIT_SUCCESS;
}

