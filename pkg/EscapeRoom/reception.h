#pragma once

#include "room.h"
#include <sys/time.h>

extern gamer* gamer_list; //Variable to store all logged/gaming clients

extern time_t server_time;

int loginGamer(const int sd, const char* usr, const char* pwd, const int cs_port);
int signupGamer(const char* usr, const char* pwd);

int avalibleRooms(char** msg);
void activeRooms();
int startRoom(int sd, char* room);

int findAsset(int sd, char* asset, char** rsp);
int takeItem(int sd, char* itm, char** rsp);
int checkRiddle(int sd, char* answer, char** rsp);
int retInventory(int sd, char** rsp);
int dropItem(int sd, char* itm);
int dropRoomGamer(int sd);
int polymerization(int sd, char* obj_src, char* obj_dst, char** rsp);
int getTime(int sd, char** rsp);
int dropGamer(int sd);

int sendGamersPorts(int sd, char** rsp);            
int setGamerLocation(int sd, char* loc, char** rsp);
gamer* findLoggedGamer(gamer* head, int sd);
int printGamer(gamer* head, int sd);
void printGamers(gamer* head);


