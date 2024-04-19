#include "reception.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "./../string/string.h"
#include "./../repo/repo.h"
#include "./../network/network.h"
#include "room.h"

#define REPO_PATH "credentials.txt"

// SERVER BUSINESS DELLA ROOM 
//------------------------Rooms----------------------

//List currently loaded and avalible rooms, returns != 0 on error 
int avalibleRooms(char** aval_rooms){
    int err = getAvalibleRooms(aval_rooms);
    if(err != 0){
      return -1;
    }
    return 0;
}

//Checks for room availability and starts a new one if not already started, returns != 0 on error 
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

//Adds a point and check game status and broadcast 
//to the room, returns != 0 on error
int pointaddHandler(int room_id, int id, char* itm){
    gamer* t_gamer = findLoggedGamer(gamer_list, id);
    if(t_gamer == NULL){
        return -1;
    }
    game_room* t_room = findRoomById(room_list, room_id);
    if(t_room == NULL){
        return -1;
    }
    t_room->tokens--;
    char* buff = malloc(1024);
    memset(buff, 0, 1024);
    sprintf(buff, "%s %s %s\n", t_gamer->username, "ha risolto l'indovinello:", itm);
    sendSolveMessage(room_id, id, buff);
    free(buff);

    //If the game is won broadcast victory message 
    //and remove gamers from the room, and delete the room
    if(!t_room->tokens){
        int err = sendRoomWinMessage(room_id, t_gamer->sd);
        if(err != 0){
            return -1;
        }
        err = delete_room(&room_list, t_room); 
        if(err != 0){
            return -1;
        }
        return 1;
    }
    return 0;
    }

//Returns gamer inventory, return != 0 on error 
int retInventory(int sd, char** rsp){
    char* buff = malloc(4096);
    memset(buff, 0, 4096);

    gamer* t_gamer = findLoggedGamer(gamer_list, sd);
    
    if(t_gamer->inventory == NULL){
        strmalloc(rsp, " ");
        return 0;
    }
  
    for(item* tmp = t_gamer->inventory; tmp; tmp = tmp->next_item){
        strcat(buff, tmp->name);
        strcat(buff, "\n"); 
    }
    strmalloc(rsp, buff);
    free(buff);
    return 0;
}

int getTime(int sd, char **rsp){
    gamer* t_gamer = findLoggedGamer(gamer_list, sd);
    if (t_gamer == NULL) {
        return -1;
    }

    game_room* t_room = findRoomById(room_list, t_gamer->room_id);
    if (t_room == NULL) {
        return -1;
    }
    *rsp = malloc(10);
    sprintf(*rsp,"%d", t_room->time_remaining);
  
    return 0;
}

//Push and item in the gamer inventory list
int insertGamerItem(item** itm_list, item* itm){
    if(*itm_list == NULL){
        *itm_list = itm;
        return 0;
    }
    itm->next_item = *itm_list;
    *itm_list = itm;
    return 0;
}

//Remove an t_item from the gamer inventory and moves it 
//in the location where it was taken
int dropItem(int sd, char* t_item){
    gamer* t_gamer = findLoggedGamer(gamer_list, sd);
    if (t_gamer == NULL) {
        return -1;
    }
    
    item* n_item = t_gamer->inventory;
    item* target = NULL;
    
    //Head inventory removal
    if(!strcmp(n_item->name, t_item)){
        target = n_item;
        t_gamer->inventory = n_item->next_item; 
        target->next_item = NULL;
        insertLocationItem(t_gamer->room_id, target); 
        return 0;
    }
    
    //Inventory removal
    while(n_item && n_item->next_item){
        if(!strcmp(n_item->next_item->name, t_item)){
            break;     
        }
        n_item = n_item->next_item; 
    }
    target = n_item->next_item;
    n_item->next_item = n_item->next_item->next_item;
    target->next_item = NULL;
    insertLocationItem(t_gamer->room_id, target);  

    return 0;
}

//Takes an item from the current gamer location and moves 
//in the gamer inventory, checks if it's a pickable item
//returns 1 if it's a riddle locked item or 2 a puzzle 
//!=0 if there is an error
int takeItem(int sd, char* t_item, char** rsp){
    gamer* t_gamer = findLoggedGamer(gamer_list, sd);
    if(t_gamer == NULL){
        return -1;
    }
    if(t_gamer->items_held == MAX_ITEMS_HELD){
        return -2;
    }
    
    item* res = findItem(t_gamer->room_id,t_gamer->curr_location, t_item);   
    if(res == NULL){
        return -1;
    }
    
    if(res->itemType == 1){
        strmalloc(rsp, res->desc_puzzle);  
        return 1;
    
    }
    if(res->itemType == 2){
        strmalloc(rsp, res->desc_locked);  
        return 2;
    }
    res = removeLocationItem(t_gamer->room_id,t_gamer->curr_location, t_item);   
    if(res == NULL){
        return -1;
    }
    if (res->token) {
        int err = pointaddHandler(t_gamer->room_id, t_gamer->sd, res->name);
        if(err == 1){
            strmalloc(rsp,"I giocatori hanno vinto!");
            return 3;
        }
    } 
    insertGamerItem(&t_gamer->inventory,res);
    strmalloc(rsp, res->success_message);
    t_gamer->items_held++;
    return 0;
 
}

//Handles riddle check and moves item in the gamer inventory, 
//returns != 0 on error, 1 on victory
int checkRiddle(int sd, char* buff, char **rsp){
    gamer* t_gamer = findLoggedGamer(gamer_list, sd);
    if(t_gamer == NULL){
        return -1;
    }
    char* itm = malloc(100);
    char* answer = malloc(300);
    memset(itm, 0, 100);  
    memset(answer, 0, 300);  
  
    sscanf(buff,"%s %[^\n]", itm, answer);

    item* res = findItem(t_gamer->room_id,t_gamer->curr_location, itm);   
    if(res == NULL){
        return -1;
    }
    
    if(res->itemType == 1){
        if (!strcmp(res->answer, answer)) {
            strmalloc(rsp, res->success_message);
    
            res = removeLocationItem(t_gamer->room_id,t_gamer->curr_location, itm);   
            if(res == NULL){
                return -1;
            }
            if (res->token) {
                int err = pointaddHandler(t_gamer->room_id, t_gamer->sd, res->name);
                if(err == 1){
                    strmalloc(rsp,"I giocatori hanno vinto!");
                    return 1;
                }
                res->token = 0;
                
            }
            res->itemType = 0;
            insertGamerItem(&t_gamer->inventory,res);
            t_gamer->items_held++;

            return 0;
        }
        strmalloc(rsp, "Risposta errata");
        return 0;
    }
    return -1; 
}

//AKA:Use command, handles the use of an obj on another, on success adds the item
//in the gamer inventory, returns != 0 on error, 1 on victory
int polymerization(int sd, char* obj_src, char* obj_dst, char** rsp){
    gamer* t_gamer = findLoggedGamer(gamer_list, sd);
    if (t_gamer == NULL) {
        return -1;
    }
    if(t_gamer->room_id == -1){
        return -1;
    }

    item* dst = findItem(t_gamer->room_id, t_gamer->curr_location, obj_dst);
    if(dst == NULL){
        return -1;
    }
    if(dst->itemType != ITM_PUZZLE){
        return 1;
    }
    //call a inventory find item
    item* src = findInvItem(sd, obj_src);
    if(src == NULL){
        return -1;
    }  

    if(!strcmp(src->name, dst->answer)){
        strmalloc(rsp, dst->success_message);
    
        item* res = removeLocationItem(t_gamer->room_id,t_gamer->curr_location, obj_dst);   
        if(res == NULL){
            return -1;
        }
        if (dst->token) {
            int err =pointaddHandler(t_gamer->room_id, t_gamer->sd, res->name);
            if(err == 1){
                strmalloc(rsp,"I giocatori hanno vinto!");
                return 1;
            }
            dst->token = 0;
        }
        res->itemType = 0;
        insertGamerItem(&t_gamer->inventory,res);
        t_gamer->items_held++;

        return 0;
    }
    strmalloc(rsp, "Oggetto errato");

    return 0;
}

//------------------------Gamers----------------------
//I have to findGamer functions the first one is more testable
//the second one i wrote for ease of use
int findGamer(int sd, gamer** target){
    for (gamer* tmp = gamer_list; tmp; tmp = tmp->next_gamer) {
         if(tmp->sd == sd){
            *target = tmp;
            return 0;
        }
    } 
    return -1;
}
gamer* findLoggedGamer(gamer* head, int sd){
    for (gamer* tmp = head; tmp; tmp = tmp->next_gamer) {
         if(tmp->sd == sd){
            return tmp;
        }
    } 
    return NULL;
}


//Function to create a new gamer, returns != 0 on error
int newGamer(gamer** head, const int sd, const char* username, const int cs_port) {
    gamer* tmp = (gamer*)malloc(sizeof(gamer));
    tmp->sd = sd;
    tmp->username = malloc(strlen(username)+1);
    strcpy(tmp->username, username);
    tmp->port = cs_port;
    tmp->room_id = -1;

    tmp->curr_location = malloc(100);
    memset(tmp->curr_location, 0, 100);
  
    tmp->inventory = NULL;
    tmp->items_held = 0;
  
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

// Function to delete a gamer, return != 0 on error
int deleteGamer(gamer** head, const int sd) {
    gamer* prev = NULL;
    gamer* next = *head;

    while (next != NULL && next->sd != sd) {
        prev = next;
        next = next->next_gamer;
    }
    //Head removal
    if (!prev && next->sd == sd) {
        *head = next->next_gamer;
        free(next);
        return 0;
    }
    if(next->sd == sd){
        prev->next_gamer = next->next_gamer;
        free(next);
        return 0;
    }
    return -1;
}

//Finds a gamer and changes it's location, returns != 0 on error
int setGamerLocation(int sd, char* loc, char** rsp){
    gamer* t_gamer = findLoggedGamer(gamer_list, sd);
    if(!t_gamer){
        return -1;
    }
    
    location* t_loc = getLocation(room_list, t_gamer->room_id, loc);
    if(!t_loc){
        return -1;
    }
    strcpy(t_gamer->curr_location,t_loc->name);
    strmalloc(rsp, t_loc->desc_location);
    return 0;
}

//Finds an Asset (location or item) and write it's description in the response
//returns != 0 on error
int findAsset(int sd, char* asset, char** rsp){
    gamer* t_gamer = findLoggedGamer(gamer_list, sd);
    if(t_gamer == NULL){
        return -1;
    }
    item* res = findItem(t_gamer->room_id,t_gamer->curr_location, asset);   
    //If the item is NULL we find a Location
    if(res == NULL){
        int err = setGamerLocation(sd, asset, rsp);
        if(err != 0){
            return -1;
        }
        return 0;
    }

    if(res->itemType != 0){
        strmalloc(rsp, res->desc_locked);
        return 0;
    }
    strmalloc(rsp, res->desc_unlocked);
    return 0;
} 

//Function to print a gamer status
int printGamer(gamer* head, int sd){
    gamer* tmp_gamer = NULL;
    int err = findGamer(sd, &tmp_gamer);
    if (err != 0) {
        return -1;
    }

    printf("Gamer: %d %s Room: %d Loc: %s G_port: %d\n", tmp_gamer->sd, tmp_gamer->username, tmp_gamer->room_id, tmp_gamer->curr_location, tmp_gamer->port);
    return 0;
}

int sendGamersPorts(int sd, char** rsp){
    char* buff = malloc(1024);
    memset(buff, 0, 1024);
    gamer* t_gamer = findLoggedGamer(gamer_list, sd);
    if(t_gamer == NULL){
        return -1;
    }

    for(gamer* tmp_gamer = gamer_list; tmp_gamer; tmp_gamer = tmp_gamer->next_gamer){
        if(t_gamer->room_id == tmp_gamer->room_id && tmp_gamer->sd != sd){
            char* port = malloc(10);
            sprintf(port, "%d", tmp_gamer->port);
            strcat(buff, port);
            strcat(buff, " ");
        }
    }
    strmalloc(rsp, buff);
    free(buff);
    return 0; 
}
//Function to print all gamer status
void printGamers(gamer *head){
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    int width = w.ws_col;

    for(int i = 0; i < width; i++) {
        printf("*");
    }
    printf("\n");
    for (gamer* tmp = head; tmp; tmp = tmp->next_gamer) {
        printf("Gamer: %d %s Room: %d Loc: %s G_port: %d\n", tmp->sd, tmp->username, tmp->room_id, tmp->curr_location, tmp->port);
    }

    for(int i = 0; i < width; i++) {
        printf("*");
    }
    printf("\n");
}

//Check if user is registered and add the gamer to the reception
int loginGamer(const int sd, const char* usr, const char* pwd, const int cs_port){
  if(get_user(REPO_PATH, usr, pwd) != 0){
    return -1;
  }
  gamer* tmp_gamer = NULL;
  int err = findGamer(sd, &tmp_gamer);
  if(err == 0){
    return -1;
  }
   
  err = newGamer(&gamer_list, sd, usr, cs_port);
  if (err != 0) {
      return -1;
  }
  return 0;
};

//SignUp a gamer 
int signupGamer(const char* usr, const char* pwd){
  //leggere da file e confrontare con utente e password
  if (get_user(REPO_PATH, usr, pwd) == 0) {
    return -1;
  }
  return create_user(REPO_PATH, usr, pwd);
};

//Removes a gamer from a room and dropping it's inventory, 
//returns != 0 on erorr
int dropRoomGamer(int sd){
    gamer* t_gamer = findLoggedGamer(gamer_list, sd);
    if (t_gamer == NULL) {
        return -1;
    } 

    item* target = NULL;

    for (item* tmp = t_gamer->inventory; tmp; tmp = t_gamer->inventory) {
        target = tmp;
        t_gamer->inventory = tmp->next_item; 
        target->next_item = NULL;
        insertLocationItem(t_gamer->room_id, target);  
    }

    game_room* t_room = findRoomById(room_list, t_gamer->room_id);
    if(t_room == NULL){
        return -1;
    }
    t_room->current_gamers--;
    t_gamer->room_id = -1; 
    return 0;
}


//Removes a gamer form the system, returns !=0 on error
int dropGamer(int sd){
    gamer* t_gamer = findLoggedGamer(gamer_list, sd);
    if (t_gamer == NULL) {
        return -1;
    } 

    if (t_gamer->room_id == -1) {
        deleteGamer(&gamer_list, sd);
        return 0;
    }
    item* target = NULL;

    for (item* tmp = t_gamer->inventory; tmp; tmp = t_gamer->inventory) {
        target = tmp;
        t_gamer->inventory = tmp->next_item; 
        target->next_item = NULL;
        insertLocationItem(t_gamer->room_id, target);  
    }

    game_room* t_room = findRoomById(room_list, t_gamer->room_id);
    if(t_room == NULL){
        return -1;
    }
    t_room->current_gamers--;
    
    deleteGamer(&gamer_list, sd);
    return 0;
}


