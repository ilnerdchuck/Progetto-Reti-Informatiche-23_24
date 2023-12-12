#pragma once
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>


struct client {
    char * server_ip;
    size_t server_port; 
    size_t client_port; 
    size_t sd; 
    int connected;
    
} typedef client;

client * new_client(const char * server_ip, const size_t server_port, const size_t client_port);
int request(client * c,const char *msg, char* rsp);
void delete_client(client * c);


int _recive(size_t sd, char* rsp);
int _send(size_t sd, const char* msg);

