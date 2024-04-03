#include <string.h>
#include <stdio.h>

#define MAX_PLAYERS 5

typedef struct{
    char* name;
    char* desc;
    char* solve;
} item ;

typedef struct{
    int id;
    int connected_clients [MAX_PLAYERS];
    item* items;
} game_room;

int room_init(game_room* r, const char* path);


