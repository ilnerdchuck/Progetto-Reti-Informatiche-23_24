#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "network.h"

#define MAX_MESSAGE_SIZE 4096
#define CHUNK_SIZE 1024

int _send(int sd, const char* msg) {
    // check if the message can be sent
    uint32_t len = strlen(msg) + 1;
    if (len > MAX_MESSAGE_SIZE) {
        return -1;
    }

    // send msg size
    uint32_t web_len = htonl(len);
    int res = send(sd, &web_len, sizeof(uint32_t), 0);
    if (res <= 0) {
        return -1;
    }

    // send message
    uint32_t bytes_sent = 0;
    uint32_t tmp_len = 0;
    while (bytes_sent < len) {
        tmp_len = (len - bytes_sent > CHUNK_SIZE) ? CHUNK_SIZE : len - bytes_sent;
        web_len = htonl(tmp_len);

        res = send(sd, &web_len, sizeof(uint32_t), 0);
        if (res <= 0) return -1;

        res = send(sd, msg+bytes_sent , tmp_len, 0);
        if (res <= 0) return -1;

        bytes_sent += res;
    }

    return 0;
}

int _receive(int sd, char** rsp) {
    uint32_t rcv_len = 0;
    int res = recv(sd, &rcv_len, sizeof(uint32_t), 0);
    if(res <= 0){
        return -1;
    }

    uint32_t msg_len = ntohl(rcv_len);

    if(msg_len > MAX_MESSAGE_SIZE){
        return -1;
    }

    // recive message
    int bytes_recived = 0; 
    uint32_t tmp_len = 0;
    //conviene fare cosi o allorare con una malloc nello heap?
    //il farlo sempre a ogni send conviene o no?
    char tmp_rsp[MAX_MESSAGE_SIZE] = {0};
    char buffer[CHUNK_SIZE] = {0};
    while (bytes_recived < msg_len) {
        memset(buffer, 0, CHUNK_SIZE);
        res = recv(sd, &tmp_len, sizeof(uint32_t), 0);
        if (res <= 0) {
            return -1;
        }
        tmp_len = ntohl(tmp_len);
        
        res = recv(sd, &buffer, tmp_len, 0);
        if (res <= 0) {
            return -1;
        }

        bytes_recived += res;
        strncat(tmp_rsp, buffer, tmp_len);
        printf("tmp_rsp: %s \n",tmp_rsp);
        printf("buffer: %s \n",buffer);
    }

    *rsp = malloc(msg_len);
    if (*rsp == NULL) {
        return -1;
    }

    memcpy(*rsp, tmp_rsp, msg_len);
    // perché lo devo pulire? non dealloca tutto quando esce dalla func
    // se lo lascio sporca la risposta successiva
    // perché alla chiamata successia rialloca la variabile a caso nello stesso posto e 
    // quindi evviva stessa memoria?

    return 0;
}
