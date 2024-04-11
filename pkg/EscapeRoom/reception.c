#include "reception.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "./../string/string.h"
#include "./../repo/repo.h"
#include "room.h"

#define REPO_PATH "credentials.txt"
// SERVER BUSINESS DELLA ROOM 
//------------------------Rooms----------------------

int avalibleRooms(char** aval_rooms){
    int err = getAvalibleRooms(aval_rooms);
    if(err != 0){
      return -1;
    }
    return 0;
}

int startRoom(int sd, char* room){
    int err = 0;
    char* aval_rooms = NULL;
    err = getAvalibleRooms(&aval_rooms);
    if(!strstr(aval_rooms, room)){
        return -3;
    }
    game_room* res = findRoomByMap(room_list, room);
    if(!res){
        err = createRoom(&room_list, room, MAX_ROOM_TIME);
        if (err != 0) {
            return -1;
        }
    }
    
    //inserisci gamer nella stanza corrente
    err = insertGamerInRoom(room_list, sd, room);
    if(err == -1){
        return err;
    }
    if(err == -2){
        return err;
    }


    return 0;
}

//------------------------Gamers----------------------
int findGamer(int sd, gamer** target){
    for (gamer* tmp = gamer_list; tmp; tmp = tmp->next_gamer) {
         if(tmp->sd == sd){
            *target = tmp;
            return 0;
        }
    } 
    return -1;
}

// Function to create a new gamer
int newGamer(gamer** head, const int sd, const char* username) {
    gamer* tmp = (gamer*)malloc(sizeof(gamer));
    tmp->sd = sd;
    tmp->username = malloc(strlen(username)+1);
    strcpy(tmp->username, username);
    tmp->port = 0;
    tmp->room_id = -1;
    tmp->inventory = NULL;
    tmp->next_gamer = NULL;
    
    gamer* work = *head;
   
    if(!work){
        *head = tmp;
        return 0; 
    }

    while(work->next_gamer){
        work = work->next_gamer;
    }

    if (work->next_gamer) {
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

int printGamer(gamer* head, int sd){
    gamer* tmp_gamer = NULL;
    int err = findGamer(sd, &tmp_gamer);
    if (err != 0) {
        return -1;
    }

    printf("Gamer: %d %s Room: %d G_port: %d\n", tmp_gamer->sd, tmp_gamer->username, tmp_gamer->room_id, tmp_gamer->port);
    return 0;
}

void printGamers(gamer *head){
    for (gamer* tmp = head; tmp; tmp = tmp->next_gamer) {
        printf("Gamer: %d %s Room: %d G_port: %d\n", tmp->sd, tmp->username, tmp->room_id, tmp->port);
    }
}

// Check if user is registered and add the gamer to the reception
int loginGamer(const int sd, const char* usr, const char* pwd){
  if(get_user(REPO_PATH, usr, pwd) != 0){
    return -1;
  }
  gamer* tmp_gamer = NULL;
  int err = findGamer(sd, &tmp_gamer);
  if(err == 0){
    return -1;
  }
   
  err = newGamer(&gamer_list, sd, usr);
  if (err != 0) {
      return -1;
  }
  return 0;
};


int signupGamer(const char* usr, const char* pwd){
  //leggere da file e confrontare con utente e password
  if (get_user(REPO_PATH, usr, pwd) == 0) {
    return -1;
  }
  return create_user(REPO_PATH, usr, pwd);
};



