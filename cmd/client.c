#include <stdio.h>
#include "./../network/network.h"

void accept_function(int sd) {}
void input_function(int sd, char *inputText) {
    // lo switch sulle requests

    // if inputText.StartsWith("login") -> chiama gamer.Login()
}
int response_function(int sd, const message msg, message *rsp) {}

int main(){

    server *s = new_server(accept_function, input_function, response_function);
}