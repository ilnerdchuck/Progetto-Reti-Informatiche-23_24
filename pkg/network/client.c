#include <stdlib.h>
#include <string.h>

#include "network.h"

client *new_client(const char *server_ip, const size_t server_port) {
    struct client *c = malloc(sizeof(struct client));

    if (c == NULL) {
        goto error;
    }
    c->server_ip = malloc(strlen(server_ip) + 1);
    if (c->server_ip == NULL) {
        goto error;
    }

    memcpy(c->server_ip, server_ip, strlen(server_ip) + 1);
    c->server_port = server_port;
    return c;

error:
    delete_client(c);
    return NULL;
}

void delete_client(client *c) {
    if (c == NULL) {
        return;
    }
    if (c->server_ip != NULL) {
        free(c->server_ip);
        c->server_ip = NULL;
    }
    free(c);
    c = NULL;

    return;
}

int connect(client *c) { return 0; }

int request(client *c, const char *msg, char **rsp) {
    int err = connect(c);
    if (err == -1) {
        return -1;
    }

    err = _send(c->sd, msg);
    if (err == -1) {
        return -1;
    }

    return _receive(c->sd, rsp);
}
