#include "network.h"

#include <pthread.h>
#include <unistd.h>

#include "./../test.h"

void *server_thread_func(void *arg) {
    server *s = (server *)arg;
    listen_server(s);
    return NULL;
}

// PING PONG behaviour
// -----------------------------------------
void accept_function(int sd) {}
void input_function(int sd, char *inputText) {}
int response_function(int sd, const char *msg, char **rsp) {
    char *s = "pang";
    if (strcmp(msg, "ping") == 0) {
        s = "pong";
    }

    uint32_t len = strlen(s);
    *rsp = malloc(len + 1);
    if (*rsp == NULL) {
        return -1;
    }
    memcpy(*rsp, s, len);
    return 0;
}
// -----------------------------------------

int TestPingPong() {
    server *s = new_server(accept_function, input_function, response_function);

    uint32_t port = 1010;
    int attempts = 0;
    while (bind_server(s, port) != -1) {
        port++;
        attempts++;
        ASSERT(attempts < 100);
    }

    pthread_t server_thread;
    int res = pthread_create(&server_thread, NULL, server_thread_func, s);
    ASSERT(res == 0);

    usleep(1000 * 1000);

    client *c = new_client("127.0.0.1", port);

    char *rsp = NULL;
    printf("miao");

    res = request(c, "ping", &rsp);
    ASSERT(res != -1)
    ASSERT(strcmp("pong", rsp) == 0);
    free(rsp);
    rsp = NULL;

    res = request(c, "boh", &rsp);
    ASSERT(res != -1)
    ASSERT(strcmp("pang", rsp) == 0);
    free(rsp);
    rsp = NULL;

    pthread_join(server_thread, NULL);

    delete_server(s);
    delete_client(c);

    return 0;
}