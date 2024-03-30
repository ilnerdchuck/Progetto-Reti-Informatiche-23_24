#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_MESSAGE_SIZE 4096
#define CHUNK_SIZE 1024

//------IO-------//
enum msgType{
  COMMAND,
  SUCCESS,
  ERROR,
  TEXT,
};

enum cmdType{
  LOGIN,
  SINGUP,

  //ACTION,
  //All game type actions
  //PICKUP
};
typedef struct{
  enum msgType msgtype;
  enum cmdType cmdtype;
  char *field;
} message;

//------CLIENT-------//
typedef struct {
  char* server_ip;
  size_t server_port;
  size_t client_port;
  int sd;
  int connected;
  int logged;
} client;

client* new_client(const char* server_ip, const size_t server_port);
int request(client* c, const message payload, message* rsp);
void delete_client(client* c);

typedef void (*AcceptFunction)(int sd);
typedef void (*InputFunction)(int sd, char* inputText);
typedef int (*ResponseFunction)(int sd, const message msg, message* rsp);

//------SERVER-------//
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

int _send(int sd, const message msg);
int _receive(int sd, message* rsp);
