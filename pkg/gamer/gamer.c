#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./../network/network.h"
#include "./../protocol/protocol.h"
#include "./../util/util.h"

// Gamer logic


int login(client* c){
    int err = 0;
  //Read username and password
    char* usr = NULL;
    while(1){
      printf("Inserisci l'username (Senza spazi):\n");
      err = read_stdin_line(&usr);
      if (err != 0) {
          free(usr);
          return -1;
      }
      if(!strcmp(usr, "") || strchr(usr,' ')){
        printf("Username non valido\n");
        continue;
      }
      break;
    }
    
    char* pwd1 = NULL;

    while(1){
      printf("Inserisci la password (senza spazi):\n");
      
      err = read_stdin_line(&pwd1);
      if (err != 0) {
          return -1;
      }

      if (!strcmp(pwd1, "") || strchr(pwd1, ' ')) {
        printf("Password non valida\n");
        continue;
      }else{
        break;
      }
    }
    message credentials = {0};
    
    credentials.field = malloc(strlen(usr) + strlen(pwd1) +2);
    sprintf(credentials.field, "%s %s", usr, pwd1);
    credentials.msgtype = MSG_COMMAND;
    credentials.cmdtype = CMD_LOGIN;
    free(usr);
    free(pwd1);
    message rsp = {0};
    request(c, credentials, &rsp);
    
    if(rsp.msgtype == MSG_ERROR && rsp.cmdtype == CMD_LOGIN){
        return -1;
    }else if (rsp.msgtype == MSG_SUCCESS && rsp.cmdtype == CMD_LOGIN) {
        return 0;
    }
    
    if(rsp.msgtype == MSG_BAD_REQUEST){
      return -2;
    }

    return -1;
}

int signup(client* c){
    char* usr = NULL;
    int err = 0;

    while(1){
      printf("Inserisci l'username (Senza spazi):\n");
      err = read_stdin_line(&usr);
      if (err != 0) {
          return -1;
      }
      if(!strcmp(usr, "") || strchr(usr,' ')){
        printf("Username non valido\n");
        continue;
      }
      break;
    }
    
    char* pwd1 = NULL;
    char* pwd2 = NULL;

    while(1){
      printf("Inserisci la password (senza spazi):\n");
      err = read_stdin_line(&pwd1);
      if (err != 0) {
          return -1;
      }

      if (!strcmp(pwd1, "") || strchr(pwd1, ' ')) {
       printf("Password non valida\n");
        continue;
      }

      printf("Inseriscila ancora:\n");
      err = read_stdin_line(&pwd2);
      if (err != 0) {
          return -1;
      }
      if(!strcmp(pwd1, pwd2)){
        break;
      }
      printf("Password non uguali\n");
    }
    
    message credentials = {0};
    
    //Send username and password register command
  
    credentials.field = malloc(strlen(usr) + strlen(pwd1) +2);
    sprintf(credentials.field, "%s %s", usr, pwd1);
    credentials.msgtype = MSG_COMMAND;
    credentials.cmdtype = CMD_SIGNUP;
    
    message rsp = {0};

    request(c, credentials, &rsp);
    
    //Check if registration is successful
    if(rsp.msgtype == MSG_ERROR && rsp.cmdtype == CMD_SIGNUP){
        return -1;
    }else if (rsp.msgtype == MSG_SUCCESS && rsp.cmdtype == CMD_SIGNUP) {
        return 0;
    }
    if(rsp.msgtype == MSG_BAD_REQUEST && !strcmp(rsp.field, "closed")){
        return -2;
    }

    return -1;
}


