#include "./../repo/repo.h"

#include <stdlib.h>

// Function to create a new game room an add to the room list
int create_room(game_room* head_room, char* map, int time, item* items, gamer* gamers) {
    game_room* new_room = (game_room*)malloc(sizeof(game_room));
    new_room->id = 0;
    new_room->room_map = map;
    new_room->time_remaining = time;
    new_room->items = items;
    for (int i = 0; i <= MAX_PLAYERS; i++) {
        new_room->gamers_in_room[i] = NULL;
    }
    new_room->next_room = NULL;
    
    //Insert
    if(head_room == NULL){
        head_room = new_room;
        return 0;
    }
    
    game_room* tmp = head_room;
    while (tmp->next_room != NULL) {
        //@TODO: check and update room id while you are here 
        
        tmp = tmp->next_room;
    }
    
    tmp->next_room = new_room;

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
        //@TODO: Add code here to print items and gamers
        temp = temp->next_room;
    }
}

// Function to find a room by map
game_room* find_room_by_map(game_room* head, char* map) {
    game_room* temp = head;
    while (temp != NULL) {
        if (strcmp(temp->room_map, map) == 0) {
            return temp;
        }
        temp = temp->next_room;
    }
    return NULL; // return NULL if no room with the given map is found
}

