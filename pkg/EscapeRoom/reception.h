#pragma once


typedef struct{
    char* name;
    char* solve;
    char* desc;
    struct item* next_item;
} item;

struct gamer{
    int sd;
    char* username;
    int port; // used only for in-game chat 
    int room_id;
    item* inventory;
    struct gamer* next_gamer;
};

typedef struct gamer gamer;

extern gamer* gamer_list;

int loginGamer(const int sd, const char* usr, const char* pwd);
int signupGamer(const char* usr, const char* pwd);
int printGamer(gamer* head, int sd);
void printGamers(gamer* head);
