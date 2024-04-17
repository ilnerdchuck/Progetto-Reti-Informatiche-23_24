#pragma once

#include <sys/time.h>
#include <stddef.h>
#include "./../protocol/protocol.h"
#include <stdint.h>
#include <stdio.h>

#define MAX_MESSAGE_SIZE 4096
#define CHUNK_SIZE 1024

typedef struct{
  int msgtype; //Define the type of message sent
  int cmdtype; //Used if msgtype is MSG_COMMAND to handle different commands
  char *field; //Used to write the effetive message 
} message;

//------CLIENT-------//
typedef struct {
  int sd;
  char* server_ip;
  size_t server_port;
  size_t client_port;
  int connected; //Used to mark if a client is connected with a connect()
} client;

client* new_client(const char* server_ip, const size_t server_port);
int request(client* c, const message payload, message* rsp);
void delete_client(client* c);

typedef void (*AcceptFunction)(int sd);
typedef void (*TickFunction)(struct timeval time);
typedef void (*InputFunction)(int sd, const char* inputText);
typedef int (*ResponseFunction)(int sd, const message msg, message* rsp);
typedef void (*DisconnectFunction)(int sd);

//------SERVER-------//
struct server {
    int listener; //Stores the listener socket
    int run; //Handles the listen_server cycle to start/stop it
    AcceptFunction a; //Callback to handle function/istruction when accept() is called
    TickFunction t; //Callback to handle istruction on a tick of the server
    InputFunction i; //Callback to handle stdin stream input
    ResponseFunction r; //Callback to handle response to a receive o f a message
    DisconnectFunction d; //Callback to handle a disconnect on a socket
} typedef server;

server* new_server(AcceptFunction a, InputFunction i, ResponseFunction r, DisconnectFunction d, TickFunction t);
int bind_server(server* s, int port);
int listen_server(server* s);
void delete_server(server* s);
void stop_server(server*s);

int _send(int sd, const message msg);
int _receive(int sd, message* rsp);
