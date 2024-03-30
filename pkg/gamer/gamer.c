#include <string.h>
#include "./../network/network.h"
#include "./../protocol/protocol.h"
#include "./../util/input.h"

// Gamer logic

int auth(client* c){
  
  printf("Autenticazione\n");
  printf("Comandi disponibili:\n");
  printf("\t -> Sign Up -- Registrati nel sistema\n");
  printf("\t -> Login -- Effettua il login");
  
  char buff[4096] = {0};

  read_stdin_line(buff);
  
  message credentials = {0};

  if(strcmp(buff, "Sign Up")){
    char usr[1024] = {0};
    
    while(1){
      printf("Inserisci l'username (Senza spazi):\n");
      read_stdin_line(usr);
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
      read_stdin_line(pwd1);
      if (!strcmp(pwd1, "") || strchr(pwd1, ' ')) {
       printf("Password non valida");
        continue;
      }

      printf("Inseriscila dinouvo:\n");
      read_stdin_line(pwd2);

      if(!strcmp(pwd1, pwd2)){
        break;
      }
      printf("Password non uguali");
    }
    
    //Send username and password register command
    sprintf(credentials.field, "%s %s", usr, pwd1);
    credentials.msgtype = MSG_COMMAND;
    credentials.cmdtype = CMD_SIGNUP;
    
    message rsp = {0};
    request(c, credentials, &rsp);
    
    //Check if registration is successful
    if(rsp.msgtype == MSG_ERROR){
      printf("Registraizone Fallita: %s\n",rsp.field);
      return -1;
    }else if (rsp.msgtype == MSG_SUCCESS && rsp.cmdtype == CMD_SIGNUP) {
      printf("Registrazione Avvenuta\n");
      c->logged = 1;
      return 0;
    }


  }else if(strcmp(buff, "Login")){
    //Read username and password
    char usr[1024] = {0};
    while(1){
      printf("Inserisci l'username (Senza spazi):\n");
      read_stdin_line(usr);
      if(!strcmp(usr, "") || strchr(usr,' ')){
        printf("Username non valido\n");
        continue;
      }
      break;
    }
    
    char pwd1[1024] = {0};

    while(1){
      printf("Inserisci la password (senza spazi):\n");
      
      read_stdin_line(pwd1);
      if (!strcmp(pwd1, "") || strchr(pwd1, ' ')) {
        printf("Password non valida");
        continue;
      }else{
        break;
      }
    }

    sprintf(credentials.field, "%s %s", usr, pwd1);
    credentials.msgtype = MSG_COMMAND;
    credentials.cmdtype = CMD_LOGIN;
    
    message rsp = {0};
    request(c, credentials, &rsp);
    
    if(rsp.msgtype == MSG_ERROR){
      printf("Autenticazione Fallita: %s\n", rsp.field);
      return -1;
    }else if (rsp.msgtype == MSG_SUCCESS && rsp.cmdtype == CMD_LOGIN) {
      printf("Autenticazione Avvenuta\n");
      c->logged = 1;
      return 0;
    }
  } 

  return -1;
}
