#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct client {
    char* server_ip;
    size_t server_port;
    size_t client_port;
    size_t sd;
    int connected;

} typedef client;

client* new_client(const char* server_ip, const size_t server_port);
int request(client* c, const char* msg, char** rsp);
void delete_client(client* c);

struct server {
    size_t server_port;
} typedef server;

typedef void (*AcceptFunction)(size_t sd);
typedef void (*InputFunction)(size_t sd, char* inputText);
typedef int (*ResponseFunction)(size_t sd, const char* msg, char** rsp);

server* new_server(AcceptFunction a, InputFunction i, ResponseFunction r);
int bind_server(server* s, size_t port);
int listen_server(server* s);
void delete_server(server* s);

int _send(size_t sd, const char* msg);
int _receive(size_t sd, char** rsp);
