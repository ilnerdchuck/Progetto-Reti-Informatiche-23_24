#pragma once
#include <string.h>
#include <stdio.h>

#include "./reception.h"

#define MAX_PLAYERS 5

struct game_room{
    int id;
    char* room_map;
    int time_remaining;
    item* items;
    gamer* gamers_in_room[MAX_PLAYERS];
    struct game_room* next_room;
} typedef game_room;

extern game_room* room_list;

int create_room(game_room* head_room, char* map, int time, item* items, gamer* gamers);
void add_room(game_room* head, game_room* new_room);
void delete_room(game_room* head, game_room* room_to_delete);
void print_rooms(game_room* head);
game_room* find_room_by_map(game_room* head, char* map);
void free_rooms(game_room* head);


