#include "network.h"
#include <stdint.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MAX_MESSAGE_SIZE 4096
#define CHUNK_SIZE 1024

int _send(size_t sd, const char* msg){
    // check if the message can be sent
    uint32_t len = strlen(msg)+1;
    if (len > MAX_MESSAGE_SIZE) {
        return -1;
    }

    // send actual size
    uint32_t web_len = htonl(len);
    int res = send(sd, &web_len, sizeof(uint32_t), 0);
    if (res <= 0) {
        return -1;
    }

    // send message
    int bytes_sent = 0, tmp_len;
    while (bytes_sent < len) {
        tmp_len = (len - bytes_sent > CHUNK_SIZE) ? CHUNK_SIZE: len - bytes_sent;
        web_len = htonl(tmp_len);
        res = send(sd, &web_len, sizeof(uint32_t), 0);
        if (res <= 0) return -1;

        res = send(sd, msg + bytes_sent, tmp_len, 0);
        if (res <= 0) return -1;
        bytes_sent += res;
    }
}
int _recive(size_t sd, char* rsp);


