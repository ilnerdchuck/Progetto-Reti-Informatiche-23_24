#include <arpa/inet.h>
#include <errno.h>
#include <malloc.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "network.h"

server* new_server(AcceptFunction a, InputFunction i, ResponseFunction r) {
    // TODO: check that functions are not null
    server* s = malloc(sizeof(server));
    if (s == NULL) {
        goto error;
    }

    s->a = a;
    s->i = i;
    s->r = r;

    s->listener = socket(AF_INET, SOCK_STREAM, 0);
    if (s->listener == -1) {
        goto error;
    }

    return s;
error:
    return NULL;
}

int bind_server(server* s, int port) {
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    return bind(s->listener, (struct sockaddr*)&address, sizeof(address));
}

int listen_server(server* s) {
    fd_set master, read_fds;

    struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};

    listen(s->listener, 10);

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    FD_SET(STDIN_FILENO, &master);
    FD_SET(s->listener, &master);

    int fdmax = s->listener;

    while (1) {
        read_fds = master;

        select(fdmax + 1, &read_fds, NULL, NULL, &timeout);

        for (int fd = 0; fd <= fdmax; fd++) {
            if (!FD_ISSET(fd, &read_fds)) continue;

            // TODO: add input callback and fd
            if (fd == STDIN_FILENO) {
                // TODO:
                // - receive the message from stdin
                // - s->i(sd, msg)
                continue;
            }
            // add a connection in case the full sd is the listener
            if (fd == s->listener) {
                struct sockaddr_in cl_addr;
                uint32_t addrlen = sizeof(cl_addr);

                int newsd = accept(s->listener, (struct sockaddr*)&cl_addr,
                                   (socklen_t*)&addrlen);

                FD_SET(newsd, &master);
                if (newsd > fdmax) {
                    fdmax = newsd;
                }
                // call accept callback
                s->a(fd);
                continue;
            }

            // in case of a message call the proto receive
            char* msg;
            int err = _receive(fd, &msg);

            // in case of an error close the socket
            if (err == -1) {
                FD_CLR(fd, &master);
                close(fd);
                // TODO: add callback for disconnect
                continue;
            }

            // call the message callback
            char* rsp = NULL;
            err = s->r(fd, msg, &rsp);
            if (err == -1) {
                FD_CLR(fd, &master);
                close(fd);
                // TODO: add callback for disconnect
                continue;
            }

            // otherwise send the response
            err = _send(fd, rsp);

            free(rsp);
            rsp = NULL;

            free(msg);
            msg = NULL;

            if (err == -1) {
                FD_CLR(fd, &master);
                close(fd);
                // TODO: add callback for disconnect
                continue;
            }
        }
    }
}

void delete_server(server* s) {
    // TODO: delete
}
