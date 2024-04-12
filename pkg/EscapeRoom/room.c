#include "room.h"
#include "./../repo/repo.h"
#include "./../string/string.h"
#include "reception.h"
#include <stdlib.h>
#include <string.h>

// Function to create a new game room an add to the room list

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



//trova e stampa la location
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



item* removeLocationItem(int room_id, char* t_loc, char* it_name){
    location* t_location = getLocation(room_list, room_id, t_loc);
    if (t_location == NULL) {
        return NULL;
    }

    item* n_item = t_location->items;
    item* target = NULL;
    //rimozione testa 
    if(!strcmp(n_item->name, it_name)){
        target = n_item;
        t_location->items = n_item->next_item; 
        target->next_item = NULL;
        return target;
    }
    
    //rimozione nel mezzo o coda
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

// Function to delete a room from the list
void delete_room(game_room* head, game_room* room_to_delete) {
    if (head == NULL) {
        return;
    }
    
    if (head == room_to_delete) {
        head = head->next_room;
        free(room_to_delete);
        return;
    }

    game_room* temp = head;
    while (temp->next_room != NULL && temp->next_room != room_to_delete) {
        temp = temp->next_room;
    }
    
    if (temp->next_room == NULL) {
        return;
    }
    temp->next_room = temp->next_room->next_room;
    free(room_to_delete);
}

// Function to print the details of all rooms
void print_rooms(game_room* head) {
    game_room* temp = head;
    while (temp != NULL) {
        printf("RoomId:vieni  %d Map: %s Time:%d Tk:%d gamers:%d\n",
                temp->id,
                temp->room_map,
                temp->time_remaining,
                temp->tokens,
                temp->current_gamers
               );
        for(location* t_loc = temp->locations; t_loc; t_loc = t_loc->next_location){
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
        }
        temp = temp->next_room;
    }
}
// Function to find a room by map
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
// Function to find a room by map
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



