#pragma once

#include "./../EscapeRoom/room.h"
// FILE ACCESS
int log_port(const char * path, const int port);

int get_user(const char * path, const char* usr, const char* pwd);
int create_user(const char * path, const char* usr, const char* pwd);

int getAvalibleRooms(char** room_list);
int getLocations(const char* map, location** locations, int* tokens);
int get_room_item(const char* path, item* it);




