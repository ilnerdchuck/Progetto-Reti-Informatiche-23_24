#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "network.h"
#include "./../string/string.h"

// msg_serialize serializes the message into the buff, returns err != 0 on error
int msg_serialize(const message msg, char* buff){
    buff = fmt_Sprintf("%d %d %s",msg.msgtype,msg.cmdtype,msg.field);
    if (buff == NULL){
            return -1;
    }
    return 0;
}

// msg_deserialize serilized the buff into the msg, returns err != 0 on error
int msg_deserialize(const char* buff, message *msg ){
    msg->field = (char*)malloc(strlen(buff) + 1);
    if (msg->field == NULL) {
        return -1;
    }
    sscanf(buff, "%d %d %s",(int*)&msg->msgtype,(int*)&msg->cmdtype,&msg->field);
    return 0;
}


// _send sends the msg into the sd, returns err != 0 on error
int _send(int sd, const message msg) {
    char * payload = NULL; 
    int err = msg_serialize(msg, payload);
    if (err != 0){
        return err;
    }

    // check if the message can be sent
    uint32_t len = strlen(payload) + 1;
    if (len > MAX_MESSAGE_SIZE) goto error;

    // send payload size
    uint32_t web_len = htonl(len);
    int res = send(sd, &web_len, sizeof(uint32_t), 0);
    if (res <= 0) goto error;

    // send message
    uint32_t bytes_sent = 0;
    uint32_t tmp_len = 0;
    while (bytes_sent < len) {
        tmp_len = (len - bytes_sent > CHUNK_SIZE) ? CHUNK_SIZE : len - bytes_sent;
        web_len = htonl(tmp_len);

        res = send(sd, &web_len, sizeof(uint32_t), 0);
        if (res <= 0) {
            goto error;
        }

        res = send(sd, payload+bytes_sent , tmp_len, 0);
        if (res <= 0) return -1;

        bytes_sent += res;
    }

    return 0;
error:
    free(payload);
    payload = NULL;
    return -1;
}

// _receive reveive the msg from the sd, returns err != 0 on error
int _receive(int sd, message* msg) {
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

    char * rsp = malloc(msg_len);
    if (rsp == NULL) {
        return -1;
    }
    memcpy(rsp, tmp_rsp, msg_len);

    return msg_deserialize(rsp, msg);
}

