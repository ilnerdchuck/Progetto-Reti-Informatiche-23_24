#pragma once

#include "room.h"

extern gamer* gamer_list;

int loginGamer(const int sd, const char* usr, const char* pwd, const int cs_port);
int signupGamer(const char* usr, const char* pwd);

int avalibleRooms(char** msg);
void activeRooms();
int startRoom(int sd, char* room);

int findAsset(int sd, char* asset, char** rsp);
int takeItem(int sd, char* item);
int retInventory(int sd, char** rsp);
//int findAsset(int sd, char* asset, char** rsp);


int setGamerLocation(int sd, char* loc, char** rsp);
gamer* findLoggedGamer(gamer* head, int sd);
int printGamer(gamer* head, int sd);
void printGamers(gamer* head);


