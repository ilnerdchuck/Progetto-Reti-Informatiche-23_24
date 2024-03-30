#include "network.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include "./../test.h"


void *server_thread_func(void *arg) {
    server *s = (server *)arg;
    listen_server(s);
    return NULL;
}

// PING PONG behaviour
// -----------------------------------------
void accept_function(int sd) {}
void input_function(int sd, const message msg) {}
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
// -----------------------------------------

int TestPingPong() {

    server *s = new_server(accept_function, input_function, response_function);

    // start the server
    uint32_t port = 2500;
    int attempts = 0;
    while (bind_server(s, port) == -1) {
        port++;
        attempts++;
        ASSERT(attempts < 100);
    }

    // start the server in a separate thread
    pthread_t server_thread;
    int res = pthread_create(&server_thread, NULL, server_thread_func, s);
    ASSERT(res == 0);   

    // wait for the server to startup 
    sleep(1);

    client *c = new_client("127.0.0.1", port);

    message msg_test = {
        .field = "ping"
    };
    message rsp = {0};

    res = request(c, msg_test, &rsp);
    ASSERT(res != -1)
    ASSERT(strcmp("pong", rsp.field) == 0);

    // msg_test->msgtype = TEXT;
    msg_test.field = "bad ping";

    res = request(c, msg_test, &rsp);
    ASSERT(res != -1)
    ASSERT(strcmp("pang", rsp.field) == 0);

    stop_server(s); 
    // join the server thread
    pthread_join(server_thread, NULL);

    delete_server(s);
    delete_client(c);

    return 0;
}
