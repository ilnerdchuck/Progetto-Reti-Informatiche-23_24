#include <stdio.h>
#include "./../network/network.h"

void accept_function(int sd) {}
void input_function(int sd, char *inputText) {}
int response_function(int sd, const message msg, message *rsp) {
    char *s = "pang";
    if (strcmp(msg.field, "ping") == 0) {
        s = "pong";
    }

    uint32_t len = strlen(s) + 1;
    rsp->field = malloc(len);
    if (rsp->field == NULL) {
        return -1;
    }
    memcpy(rsp->field, s, len);
    return 0;
}

int main(){
    server *s = new_server(accept_function, input_function, response_function);
}