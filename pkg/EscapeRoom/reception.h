#pragma once

#include "room.h"

extern gamer* gamer_list;

int loginGamer(const int sd, const char* usr, const char* pwd);
int signupGamer(const char* usr, const char* pwd);

int avalibleRooms(char** msg);
void activeRooms();
int startRoom(int sd, char* room);

int printGamer(gamer* head, int sd);
void printGamers(gamer* head);


