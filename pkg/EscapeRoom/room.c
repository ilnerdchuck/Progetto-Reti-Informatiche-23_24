#include "room.h"
#include "./../repo/repo.h"
#include "./../string/string.h"
#include "reception.h"
#include <stdlib.h>

// Function to create a new game room an add to the room list
gamer* findLoggedGamer(gamer* head, int sd){
    for (gamer* tmp = head; tmp; tmp = tmp->next_gamer) {
         if(tmp->sd == sd){
            return tmp;
        }
    } 
    return NULL;
}



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
    res->current_gamers++;
  
    //@TODO: add callback for sending the port to all connected clients

    return 0;
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


