#pragma once
#include <string.h>
#include <stdio.h>
#include <stdbool.h>


#define MAX_PLAYERS 5
#define MAX_ROOM_TIME 5000
#define MAX_ITEMS_HELD 5

typedef enum {
  ITM_PICKABLE,
  ITM_RIDDLE,
  ITM_PUZZLE
} itemType;

struct item{
    char* name; 
    itemType itemType; // item/riddle type
    bool token; // item token giver 
    char* unloked_item; // item added to inventory
    char* answer; // used to check input/obj
    char* success_message;
    char* desc_locked; // locked description
    char* desc_unlocked; // generic description
    struct item* next_item;
};
typedef struct item item;

struct location{
    char* name;
    char* desc_location;
    item* items;
    
    struct location* next_location;
};
typedef struct location location;

struct gamer{
    int sd;
    char* username;
    int port; // used only for in-game chat 
    int room_id;
    int items_held;
    char* curr_location;
    item* inventory;
    struct gamer* next_gamer;
};
typedef struct gamer gamer;

struct game_room{
    int id;
    char* room_map;
    int time_remaining;
    int tokens;
    location* locations;
    int current_gamers;
    gamer* gamers_in_room;// list of current gamer in the room
    struct game_room* next_room;
} typedef game_room;
extern game_room* room_list;

int createRoom(game_room** head_room, char* map, int time);
int insertGamerInRoom(game_room* t_room,int sd,char* room);
void delete_room(game_room* head, game_room* room_to_delete);
void print_rooms(game_room* head);

location* getLocation(game_room* head, int room_id, char* t_loc);
item* findItem(int room_id,char* t_loc, char* it_name);

game_room* findRoomById(game_room* head, int id);
game_room* findRoomByMap(game_room* head, char* map);
void free_rooms(game_room* head);


