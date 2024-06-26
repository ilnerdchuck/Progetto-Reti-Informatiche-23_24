#include "./../network/network.h"


int login(client* c, const int c_s_port);
int signup(client* c, const int c_s_port);
int sayToRoom(client* c, char* buff);

int requestRooms(client* c);
int requestRoom(client* c, const char* room);
int requestLook(client* c, const char* asset);
int requestTake(client* c, const char* asset);
int requestUse(client* c, const char* obj_src, const char* obj_dst);
int requestObj(client* c);
int requestDrop(client* c, const char* itm);
int requestTime(client* c);
int requestEnd(client* c);
int usePolymerization(client* c, char* obj_src,char* obj_dst);

