#ifndef CLANCHAT_H
#define CLANCHAT_H

typedef enum { CLIENT_MODE, SERVER_MODE } ClanchatMode;
typedef struct config {
    const char *name;
    const char *id;
    ClanchatMode mode;
} ClanchatConfig;

int clanchat(ClanchatConfig opts);

#endif /* CLANCHAT_H */

