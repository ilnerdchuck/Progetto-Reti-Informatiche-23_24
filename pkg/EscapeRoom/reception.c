#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "./../string/string.h"
#include "./../repo/repo.h"

#define REPO_PATH "credentials.txt"
// SERVER BUSINESS DELLA ROOM 


int findGamer(int sd){
    for (gamer* tmp = gamer_list; tmp; tmp = tmp->next_gamer) {
        if(tmp->sd == sd){
            return 0;
        }
    }
  return -1;
}

// Function to create a new gamer
int newGamer(gamer* head, const int sd, const char* username) {
    gamer* tmp = (gamer*)malloc(sizeof(gamer));
    tmp->sd = sd;
    tmp->username = _strdup(username);
    tmp->port = 0;
    tmp->room_id = -1;
    tmp->next_gamer = NULL;
    
    gamer* work = head;
    while(work->next_gamer != NULL){
        work = work->next_gamer;
    }

    if (work->next_gamer != NULL) {
        return -1;
    }

    work->next_gamer = tmp;
    return 0;
} 

//@TODO: a function for maybe a sssregister
// Function to delete a gamer
int deleteGamer(gamer *head, const int sd) {
    gamer* prev = NULL;
    gamer* next = head;

    while (next != NULL && next->sd != sd) {
        prev = next;
        next = next->next_gamer;
    }

    if (next == NULL) return -2;

    prev->next_gamer = next->next_gamer;
    free(next);
    return 0;
}

// Check if user is registered and add the gamer to the reception
int login(const int sd, const char* usr, const char* pwd){
  if(get_user(REPO_PATH, usr, pwd) != 0){
    return -1;
  }

  int err = findGamer(sd);
  if(err == 0){
    return -1;
  }
  
  err = newGamer(gamer_list, sd, usr);
  if (err != 0) {
      return -1;
  }

  return 0;
};


int signup(const char* usr, const char* pwd){
  //leggere da file e confrontare con utente e password
  if (get_user(REPO_PATH, usr, pwd) == 0) {
    return -1;
  }
  return create_user(REPO_PATH, usr, pwd);
};



