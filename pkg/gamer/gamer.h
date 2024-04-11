#include "./../network/network.h"

int login(client* c, const int c_s_port);
int signup(client* c, const int c_s_port);
int requestRooms(client* c);
int requestRoom(client* c, const char* room);
