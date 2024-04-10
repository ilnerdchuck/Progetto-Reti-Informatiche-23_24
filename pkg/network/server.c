
#define  _GNU_SOURCE

#include <arpa/inet.h>
#include <malloc.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
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
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    return bind(s->listener, (struct sockaddr*)&address, sizeof(address));
} 

int listen_server(server* s) {
    fd_set master, read_fds;

    struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};

    listen(s->listener, 10);
    s->run = 1;

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    FD_SET(STDIN_FILENO, &master);
    FD_SET(s->listener, &master);

    int fdmax = s->listener;

    while (s->run) {
        read_fds = master;

        select(fdmax + 1, &read_fds, NULL, NULL, &timeout);

        for (int fd = 0; fd <= fdmax; fd++) {
            if (!FD_ISSET(fd, &read_fds)) continue;
            
            int err = 0;
            message msg;
            
            if (fd == STDIN_FILENO) {
                char* buff = NULL;
                size_t len = 1024;
                
                len = getline(&buff, &len,stdin);
                if (len < 0 ) {
                  free(buff);
                  FD_CLR(fd, &master);
                  continue;
                }
                buff[len-1] = '\0';

                s->i(STDIN_FILENO, buff);
                free(buff);
                continue;
            }
            // add a connection in case the full sd is the listener
            if (fd == s->listener) {
                struct sockaddr_in cl_addr;
                uint32_t addrlen = sizeof(cl_addr);

                int newsd = accept(s->listener, 
                                  (struct sockaddr*)&cl_addr,
                                  (socklen_t*)&addrlen
                                  );

                FD_SET(newsd, &master);
                if (newsd > fdmax) {
                    fdmax = newsd;
                }
                // call accept callback
                s->a(fd);
                continue;
            }

            // in case of a message call the receive
            err = _receive(fd, &msg);

            // in case of an error close the socket
            
            if (err == -1) goto error;
            
            // call the message callback
            message rsp = {0};
            
            err = s->r(fd, msg, &rsp);
            
            
            if (err == -1) goto error;
            
            err = _send(fd, rsp);
            
            free(rsp.field);
            rsp.field = NULL;

            free(msg.field);
            msg.field = NULL;

            if (err == -1) goto error;
            

            continue;
error:
            FD_CLR(fd, &master);
            close(fd);
            //@TODO add callback for disconnect
            continue;
        }
    }
    return 0;
}

void stop_server(server* s){
    s->run = 0;
}

void delete_server(server* s) {
    //@TODO delete
}
