#include "room.h"
#include "./../repo/repo.h"
#include "./../string/string.h"
#include "reception.h"
#include "./../network/network.h"

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// Function to create a new game room an add to the room_list,
// returns != 0 on error
int createRoom(game_room** head_room, char* map, int time) {
    game_room* new_room = (game_room*)malloc(sizeof(game_room));
    new_room->id = 0;

    strmalloc(&new_room->room_map,map);
    new_room->time_remaining = time;
    new_room->gamers_in_room = 0;    
    new_room->locations = NULL;

    int err = getLocations(map, &new_room->locations,&new_room->tokens);
    if(err != 0){
      free(new_room);
      return -1;
    }

    new_room->gamers_in_room = NULL;
    new_room->next_room = NULL;
    
    //Tail insert to handle room id
    if(*head_room == NULL){
        *head_room = new_room;
        return 0;
    }
    
    game_room* tmp = *head_room;
    
    while (tmp->next_room != NULL) {
        if(tmp->id > new_room->id){
            new_room->id = tmp->id +1;
        }
        tmp = tmp->next_room;
    }

    tmp->next_room = new_room;

    return 0;
}

//Sends a bradcast message to all gamers in the room, 
//returns != 0 on error
int sendRoomMessage(int room_id, char* buff){
    message msg = {0};
    message rsp = {0};
    msg.msgtype = MSG_TEXT;
    strmalloc(&msg.field, buff);
    
    for(gamer* tmp_gamer = gamer_list; tmp_gamer; tmp_gamer = tmp_gamer->next_gamer){
        if (tmp_gamer->room_id == room_id) {
            client* cc = new_client("127.0.0.1", tmp_gamer->port);
            int err = 0;
            
            err = request(cc, msg, &rsp);
            
            if(rsp.msgtype != MSG_SUCCESS){
                return -1;
            }
        }
    }
 
    return 0;
}

//Sends victory message to all gamers in the room, 
//returns != 0 on error
int sendRoomWinMessage(int room_id){
    message msg = {0};
    message rsp = {0};
    msg.msgtype = MSG_TEXT;
    msg.cmdtype = CMD_WIN;
    strmalloc(&msg.field, "I giocatori hanno vinto!");
    
    int err = 0;
    for(gamer* tmp_gamer = gamer_list; tmp_gamer; tmp_gamer = tmp_gamer->next_gamer){
        if (tmp_gamer->room_id == room_id) {
            client* cc = new_client("127.0.0.1", tmp_gamer->port);
            
            err = request(cc, msg, &rsp);
            
            if(rsp.msgtype != MSG_SUCCESS){
                return -1;
            }
            
            err = dropRoomGamer(tmp_gamer->sd);
            if(err != 0){
                return -1;
            }
        }
    }
    return 0;
}

//Inserts a gamer in a room and sets it's default location
//returns != 0 on error
int insertGamerInRoom(game_room* head, int sd, char* room){
    game_room* res = findRoomByMap(head, room);
    if(res == NULL){
        return -1;
    }
    
    if(res->current_gamers == MAX_PLAYERS){
       return -2; 
    }
    
    gamer* t_gamer = findLoggedGamer(gamer_list, sd);
    t_gamer->room_id = res->id;
    strcpy(t_gamer->curr_location,"room");
    res->current_gamers++;
  
    return 0;
}



//Returs the given location in a room,
//returns NULL on location not found
location* getLocation(game_room* head, int room_id, char* t_loc){
    game_room* res = findRoomById(head, room_id);
    if(res == NULL){
        return NULL;
    }
    for (location* tmp_loc = res->locations; tmp_loc; tmp_loc = tmp_loc->next_location) {
        if (!strcmp(t_loc, tmp_loc->name)) {
            return tmp_loc;
        }
    }
    return NULL;
}

//Finds an item in a gamer inventory and returns it,
//returns NULL on item not found
item* findInvItem(int sd, char* it_name){
    gamer* t_gamer = findLoggedGamer(gamer_list, sd);
    if (t_gamer == NULL) {
        return NULL;
    }
    
    for (item* t_item = t_gamer->inventory; t_item; t_item = t_item->next_item) {
        if(!strcmp(t_item->name, it_name)){
            return t_item;
        }
    }
    return NULL;
}

//Finds an item in a given room location, returns NULL
//on item not found
item* findItem(int room_id, char* t_loc, char* it_name){
    location* t_location = getLocation(room_list, room_id, t_loc);
    if (t_location == NULL) {
        return NULL;
    }
    
    for (item* t_item = t_location->items; t_item; t_item = t_item->next_item) {
        if(!strcmp(t_item->name, it_name)){
            return t_item;
        }
    }
    return NULL;
}

//Insert a given item in it's loaded location,
//returns != 0 on error
int insertLocationItem(int room_id, item* itm){
    location* t_location = getLocation(room_list, room_id, itm->location);
    if (t_location == NULL) {
        return -1;
    }
    
    if(t_location->items == NULL){
        itm->next_item = NULL;
        t_location->items = itm;
        return 0;
    }
    itm->next_item = t_location->items;
    t_location->items = itm; 

    return 0;
}


//Removes a given item from a given location and returns it, 
//return != 0 on error
item* removeLocationItem(int room_id, char* t_loc, char* it_name){
    location* t_location = getLocation(room_list, room_id, t_loc);
    if (t_location == NULL) {
        return NULL;
    }
    
    //List removal handling
    item* n_item = t_location->items;
    item* target = NULL;
    if(!strcmp(n_item->name, it_name)){
        target = n_item;
        t_location->items = n_item->next_item; 
        target->next_item = NULL;
        return target;
    }
    
    while(n_item && n_item->next_item){
        if(!strcmp(n_item->next_item->name, it_name)){
            break;     
        }
        n_item = n_item->next_item; 
    }
    target = n_item->next_item;
    n_item->next_item = n_item->next_item->next_item; 
    target->next_item = NULL;
    return target;
}

//Function to delete a room from the list
int delete_room(game_room** head, game_room* room_to_delete) {
    if (*head == NULL) {
        return -1;
    }
    
    if (*head == room_to_delete) {
        *head = room_to_delete->next_room;
        free(room_to_delete);
        return 0;
    }

    game_room* temp = *head;
    while (temp->next_room != NULL && temp->next_room != room_to_delete) {
        temp = temp->next_room;
    }
    
    if (temp->next_room == NULL) {
        return -1;
    }
  
    temp->next_room = temp->next_room->next_room;
    free(room_to_delete);
    return 0;
}

//Function to print the details of all rooms
void print_rooms(game_room* head) {
    system("clear");
    
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    int width = w.ws_col;
    game_room* temp = head;
    while (temp != NULL) {
        for(int i = 0; i < width; i++) {
            printf("-");
        }
        printf("\n"); 
        printf("RoomId:%d Map: %s Time:%d Tk:%d gamers:%d\n",
                temp->id,
                temp->room_map,
                temp->time_remaining,
                temp->tokens,
                temp->current_gamers
               );
        for(location* t_loc = temp->locations; t_loc; t_loc = t_loc->next_location){
            for(int i = 0; i < width; i++) {
                printf("*");
            }
            printf("\n"); 
            printf("loc: %s desc: %s\n",t_loc->name,t_loc->desc_location);

            for(item* t_item = t_loc->items; t_item; t_item = t_item->next_item){
            printf("Item: %s It: %d tk: %d ui: %s ans: %s sm: %s descl: %s descu:%s\n",
                   t_item->name, 
                   t_item->itemType, 
                   t_item->token, 
                   t_item->unloked_item, 
                   t_item->answer, 
                   t_item->success_message, 
                   t_item->desc_locked, 
                   t_item->desc_unlocked
                   );
            }
            printf("\n");
        }
        temp = temp->next_room;
        for(int i = 0; i < width; i++) {
            printf("-");
        }
        printf("\n");   
    }

}
//Function to find a room by map name
game_room* findRoomByMap(game_room* head, char* map) {
    game_room* temp = head;
    while (temp != NULL) {
        if (strcmp(temp->room_map, map) == 0) {
            return temp;
        }
        temp = temp->next_room;
    }
    return NULL;
}

//Function to find a room by map Id
game_room* findRoomById(game_room* head, int id) {
    game_room* temp = head;
    while (temp != NULL) {
        if (temp->id == id) {
            return temp;
        }
        temp = temp->next_room;
    }
    return NULL;
}



