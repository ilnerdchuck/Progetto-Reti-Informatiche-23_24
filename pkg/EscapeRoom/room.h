#pragma once
#include <string.h>
#include <stdio.h>
#include <stdbool.h>


#define MAX_PLAYERS 5
#define MAX_ROOM_TIME 5000
#define MAX_ITEMS_HELD 5

typedef enum {
  ITM_PICKABLE, //The item is pickable 
  ITM_RIDDLE, //The item needs a riddle to be picked or else
  ITM_PUZZLE //The item needs a puzzle to be solved 
} itemType;

//Item struture
//The order in the maps files needs to be in this order separated by an "_"
//and with no spaces in the name 
struct item{
    char* name; //Item name
    itemType itemType; // item/riddle/puzzle type
    bool token; //If set to true the given item on pick/solve grants a token 
    char* unloked_item; //If != NULL it can be used to link an item that will be unclocked by interacting
    char* answer; //Answer to the riddle or object that needs to be used to solve the puzzle
    char* success_message; //Message shown on riddle/puzzle solved
    char* desc_locked; // locked description
    char* desc_unlocked; // generic description
    char* location;
    struct item* next_item;
};
typedef struct item item;

//location structure
struct location{
    char* name; //Name of the location
    char* desc_location; //Description of the location 
    item* items; //List of items in the locaiton
    struct location* next_location; //Used to handle the list
};
typedef struct location location;

//Gamer structure
struct gamer{
    int sd; //Id initialized with the socket of the client
    char* username; //Username of the logged Gamer
    int port; //Used to broadcast messages and in the future to implement a P2P chat 
    int room_id; //Indicates the room if the gamer is palying
    int items_held; //Number of items held in the inventory
    char* curr_location; //Current location of the gamer in a room DEFAULT: room
    item* inventory; //List of items held by the gamer
    struct gamer* next_gamer; //Used to handle the list
};
typedef struct gamer gamer;

struct game_room{
    int id; //Room id it can be used to implement more rooms with the same map
    char* room_map; //Map that the room is based on
    int time_remaining; //Room time left
    int tokens; //Number of tokens in the room to win
    location* locations; //List of locations in the room
    int current_gamers; //Number of gamers in the room
    gamer* gamers_in_room; //List of current gamer in the room
    struct game_room* next_room; //Used to handle the list
} typedef game_room;

extern game_room* room_list; //Variable that stores the room list

int createRoom(game_room** head_room, char* map, int time);
int insertGamerInRoom(game_room* t_room,int sd,char* room);
int delete_room(game_room** head, game_room* room_to_delete);
void print_rooms(game_room* head);

location* getLocation(game_room* head, int room_id, char* t_loc);
item* removeLocationItem(int room_id, char* t_loc, char* it_name);
item* findInvItem(int sd, char* it_name);
item* findItem(int room_id,char* t_loc, char* it_name);
int insertLocationItem(int room_id, item* itm);
int sendRoomMessage(int room_id,char* buff);
int sendRoomWinMessage(int room_id);

game_room* findRoomById(game_room* head, int id);
game_room* findRoomByMap(game_room* head, char* map);
void free_rooms(game_room* head);


