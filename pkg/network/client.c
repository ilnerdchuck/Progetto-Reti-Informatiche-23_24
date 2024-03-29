#include <arpa/inet.h>
#include <malloc.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "network.h"

client *new_client(const char *server_ip, const size_t server_port) {
  client *c = malloc(sizeof(client));

  if (c == NULL) {
      goto error;
  }
  c->server_ip = malloc(strlen(server_ip) + 1);
  if (c->server_ip == NULL) {
      goto error;
  }

  memcpy(c->server_ip, server_ip, strlen(server_ip) + 1);
  c->server_port = server_port;
  return c;

error:
  delete_client(c);
  return NULL;
}

void delete_client(client *c) {
  if (c == NULL) {
      return;
  }
  if (c->server_ip != NULL) {
      free(c->server_ip);
      c->server_ip = NULL;
  }
  free(c);
  c = NULL;

  return;
}

int client_connect(client *c) {
  if (c->connected) return 0;

  c->sd = socket(AF_INET, SOCK_STREAM, 0);

  if (c->sd == -1) {
      return -1;
  }

  // timeout of 100ms
  struct timeval recive_timeout = {.tv_sec = 0, .tv_usec = 100 * 1000};

  int err = setsockopt(c->sd, 
                       SOL_SOCKET, 
                       SO_RCVTIMEO, 
                       &recive_timeout,
                       sizeof(struct timeval));

  if (err == -1) {
      return -1;
  }

  struct sockaddr_in server_address = {0};
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(c->server_ip);
  server_address.sin_port = htons(c->server_port);

  err = connect(c->sd, (struct sockaddr *)&server_address,
                sizeof(server_address));

  if (err == 0) {
      c->connected = 1;
      return 0;
  }

  return -1;
}

int request(client *c, const msg payload, msg** rsp) {

  int err = client_connect(c);
  if (err == -1) {
      c->connected = 0;
      close(c->sd);
      return -1;
  }

  //@TODO parse message
  char* tmp_buff = msg_serialize(&payload);
  sprintf(tmp_buff, "%d %d %s",payload.msgtype,payload.cmdtype,payload.field);
  err = _send(c->sd, tmp_buff);
  if (err == -1) {
      c->connected = 0;
      close(c->sd);
      return -1;
  }
  char* rcv_buff = NULL;
  err = _receive(c->sd, &rcv_buff);
  if(err == -1){
    return err;
  }

  //@TODO Check if there is a better way to handle strng reception
  msg tmp_msg = msg_deserialize(rcv_buff);
  *rsp = &tmp_msg;
  
  return err;
}

int auth(client* c){
  printf("Autenticazione\n");
  printf("Comandi disponibili:\n");
  printf("\t -> Sing Up -- Registrati nel sistema\n");
  printf("\t -> Login -- Effettua il login");
  char buff[4096] = {0};

  read_stin_line(buff);
  
  msg credentials = {0};

  if(strcmp(buff, "Sing Up")){
    char usr[1024] = {0};
    while(1){
      printf("Inserisci l'username (Senza spazi):\n");
      read_stin_line(usr);
      if(!strcmp(usr, "") || strchr(usr,' ')){
        printf("Username non valido\n");
        continue;
      }
      break;
    }
    
    char pwd1[1024] = {0};
    char pwd2[1024] = {0};

    while(1){
      printf("Inserisci la password (senza spazi):\n");
      read_stin_line(pwd1);
      if (!strcmp(pwd1, "") || strchr(pwd1, ' ')) {
       printf("Password non valida");
        continue;
      }

      printf("Inseriscila dinouvo:\n");
      read_stin_line(pwd2);

      if(!strcmp(pwd1, pwd2)){
        break;
      }
      printf("Password non uguali");
    }
    
    //@TODO Send username and password register command
    sprintf(credentials.field, "%s %s", usr, pwd1);
    credentials.msgtype = COMMAND;
    credentials.cmdtype = SINGUP;
    msg* rsp;
    request(c, credentials, &rsp);
    // @TODO Check if registration is successful
    if(rsp->msgtype == ERROR){
      printf("Registraizone Fallita: %s\n",rsp->field);
      return -1;
    }else if (rsp->msgtype == SUCCESS && rsp->cmdtype == SINGUP) {
      printf("Registrazione Avvenuta\n");
      return 0;
    }


  }else if(strcmp(buff, "Login")){
    // @TODO Read username and password
    char usr[1024] = {0};
    while(1){
      printf("Inserisci l'username (Senza spazi):\n");
      read_stin_line(usr);
      if(!strcmp(usr, "") || strchr(usr,' ')){
        printf("Username non valido\n");
        continue;
      }
      break;
    }
    
    char pwd1[1024] = {0};

    while(1){
      printf("Inserisci la password (senza spazi):\n");
      read_stin_line(pwd1);
      if (!strcmp(pwd1, "") || strchr(pwd1, ' ')) {
       printf("Password non valida");
        continue;
      }else{
        break;
      }
    }

    sprintf(credentials.field, "%s %s", usr, pwd1);
    credentials.msgtype = COMMAND;
    credentials.cmdtype = LOGIN;
    msg* rsp;
    request(c, credentials, &rsp);
 
    if(rsp->msgtype == ERROR){
      printf("Autenticazione Fallita: %s\n", rsp->field);
      return -1;
    }else if (rsp->msgtype == SUCCESS && rsp->cmdtype == LOGIN) {
      printf("Autenticazione Avvenuta\n");
      return 0;
    }
  }
  

  return -1;
}

