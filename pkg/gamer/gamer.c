#include "./../network/network.h"
#include "./../protocol/protocol.h"

// BUSINESS LOGIC DEL GAMER

int auth(client* c){
  printf("Autenticazione\n");
  printf("Comandi disponibili:\n");
  printf("\t -> Sing Up -- Registrati nel sistema\n");
  printf("\t -> Login -- Effettua il login");
  char buff[4096] = {0};

  read_stin_line(buff);
  
  message credentials = {0};

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
    message* rsp;
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
    message* rsp;
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
