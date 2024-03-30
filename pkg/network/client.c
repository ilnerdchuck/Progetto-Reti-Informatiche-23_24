#include <arpa/inet.h>
#include <malloc.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "network.h"

client *new_client(const char *server_ip, const size_t server_port) {
  client *c = malloc(sizeof(client));

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

int client_connect(client *c) {
    if (c->connected) return 0;

    c->sd = socket(AF_INET, SOCK_STREAM, 0);

    if (c->sd == -1) {
        return -1;
    }

    // timeout of 100ms
    struct timeval recive_timeout = {.tv_sec = 0, .tv_usec = 100 * 1000};

    int err = setsockopt(c->sd, 
        SOL_SOCKET, 
        SO_RCVTIMEO, 
        &recive_timeout,
        sizeof(struct timeval));

    if (err == -1) {
        return -1;
    }

    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(c->server_ip);
    server_address.sin_port = htons(c->server_port);

    err = connect(c->sd, (struct sockaddr *)&server_address,sizeof(server_address));

    if (err == 0) {
        c->connected = 1;
        return 0;
    }

    return -1;
}

int request(client *c, const message payload, message* rsp) {

    int err = client_connect(c);
    if (err == -1) {
        c->connected = 0;
        close(c->sd);
        return -1;
    }

    err = _send(c->sd, payload);
    if (err != 0) {
        c->connected = 0;
        close(c->sd);
        return -1;
    }
    return _receive(c->sd, rsp);
}


