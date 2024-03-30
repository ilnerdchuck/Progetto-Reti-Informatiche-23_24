#include <stdio.h>
#include "./../network/network.h"
#include "./../protocol/protocol.h"
#include "./../room/room.h"

void accept_function(int sd) {}
void input_function(int sd, char *inputText) {}
int response_function(int sd, const message msg, message *rsp) {
    if (msg.cmdtype == CMD_LOGIN){
        // login(msg.usr, msg.pwd)
        // return a response
        return 0;
    }

    // create a bad request message
    // *rsp = message{
    //     mstype bad request
    // }
    return 0;
}   

int main(){
    server *s = new_server(accept_function, input_function, response_function);
}