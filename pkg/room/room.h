#include <string.h>

#include "./../network/network.h"
#include "./../protocol/protocol.h"
#include "./../repo/repo.h"

#define REPO_PATH "credentials.txt"
// SERVER BUSINESS DELLA ROOM 

int login(char* usr, char* pwd){
  if(get_user(REPO_PATH, usr, pwd) != 0){
    printf("Utente Non Registrato\n");
    return -1;
  }
  
  printf("Utente Registrato\n");
  return 0;
};


int signup(char* usr, char* pwd){
  //leggere da file e confrontare con utente e password
  if (get_user(REPO_PATH, usr, pwd) == 0) {
    printf("Utente giá registrato:\n");
    return -1;
  }
  return create_user(REPO_PATH, usr, pwd);
};



