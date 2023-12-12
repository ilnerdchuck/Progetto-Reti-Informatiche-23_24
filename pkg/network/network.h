#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct client {
    char* server_ip;
    size_t server_port;
    size_t client_port;
    int sd;
    int connected;

} typedef client;

client* new_client(const char* server_ip, const size_t server_port);
int request(client* c, const char* msg, char** rsp);
void delete_client(client* c);

typedef void (*AcceptFunction)(int sd);
typedef void (*InputFunction)(int sd, char* inputText);
typedef int (*ResponseFunction)(int sd, const char* msg, char** rsp);

struct server {
    int listener;
    AcceptFunction a;
    InputFunction i;
    ResponseFunction r;
} typedef server;

server* new_server(AcceptFunction a, InputFunction i, ResponseFunction r);
int bind_server(server* s, int port);
int listen_server(server* s);
void delete_server(server* s);

int _send(int sd, const char* msg);
int _receive(int sd, char** rsp);
