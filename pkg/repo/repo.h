#pragma once

#include "./../EscapeRoom/room.h"
// FILE ACCESS
int get_user(const char * path, char* usr, char* pwd);
int create_user(const char * path, char* usr, char* pwd);

int get_room_item(const char* path, item* it);




