#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "./../pkg/string/string.h"
#include "./../pkg/network/network.h"
#include "./../pkg/protocol/protocol.h"
#include "./../pkg/EscapeRoom/reception.h"
#include "./../pkg/repo/repo.h"
#include "./../pkg/util/util.h"

static server *s;
static int esroom_open = 0;
gamer* gamer_list = NULL;
game_room* room_list = NULL;
time_t server_time = 0;

static void accept_function(int sd) {}
static void tick_function() {
    //Updates the server time and determine the time elapsed between 
    //tikcs 

    //double time_elapsed = 1 + time.tv_sec + (time.tv_usec/1000000);
    time_t curr_time = time(NULL);
    double time_elapsed = difftime(curr_time, server_time);
    server_time = time(NULL); 
    checkRoomTime(time_elapsed);
    
}

//Callback for handling server stdin input, returns != 0 on error
static void input_function(int sd, const char* inputText) {
    if(!esroom_open){
        if(!strcmp(inputText, "start")){
            esroom_open = 1;
            system("clear");
            printFile("./menus/startedCommands.txt");
            return;
        }
        if(!strcmp(inputText, "stop")){
            printf("Apri prima di chiudere\n");
            return;
        }
    }
    if(esroom_open){
        if(!strcmp(inputText, "start")){
            printf("Sei giá aperto\n");
            return;
        }
        if(!strcmp(inputText, "print gamers")){
            printGamers(gamer_list);
            return;
        }

        if(!strcmp(inputText, "print rooms")){
            print_rooms(room_list);
            return;
        }
    
        if(!strcmp(inputText, "stop")){
            stop_server(s);
            return;
        }
    }

    printf("Comando errato\n");
    return;
}

//Callback for handling client messages request, returns != 0 on error
static int response_function(int sd, const message msg, message *rsp) {
    if(esroom_open == 0){ 
        strmalloc(&rsp->field,"closed");
        goto bad_request;
    }

    if(msg.msgtype == MSG_COMMAND){
        int err = 0;
        
        if (msg.cmdtype == CMD_LOGIN){
            char* usr = malloc(strlen(msg.field)+1);
            char* pwd = malloc(strlen(msg.field)+1); 
            int c_s_port = 0;

            //Get credentials from the message
            err = get_usr_pwd_port(msg.field, usr, pwd, &c_s_port);
            if (err != 0) { 
                strmalloc(&rsp->field,"Errore di parsing");
                goto cmdError;
            }
            
            //Gamer login
            err = loginGamer(sd, usr, pwd, c_s_port);
            if (err != 0) {
                printf("Errore login: credenziali non valide\n");
                strmalloc(&rsp->field, "Errore di login: credenziali non valide");
                goto cmdError;
            }
            
            printf("Accesso effettuato da:\n");
            printGamer(gamer_list, sd);
            strmalloc(&rsp->field, "vuoto");
            goto cmdSuccess;
        }

        if (msg.cmdtype == CMD_SIGNUP){
            char* usr = malloc(strlen(msg.field)+1);
            char* pwd = malloc(strlen(msg.field)+1); 
            int c_s_port = 0;
            //Get credentials
            err = get_usr_pwd_port(msg.field, usr, pwd, &c_s_port);
            if (err != 0) {
                
                strmalloc(&rsp->field,"Errore di parsing");
                goto cmdError; 
            }

            //Gamer register
            err = signupGamer(usr, pwd);
            if (err != 0) {
                strmalloc(&rsp->field,"Errore di signup");
                goto cmdError; 
            }
            
            //Gamer login
            err = loginGamer(sd, usr, pwd, c_s_port);
            
            if (err != 0) {
                strmalloc(&rsp->field,"Errore di login");
                goto cmdError;
            }
            
            goto cmdSuccess;
        }
        
        if(msg.cmdtype == CMD_RROOMS){
            char* avail_rooms = NULL;
            err = avalibleRooms(&avail_rooms);
            if(err != 0){
              strmalloc(&rsp->field, "Errore di listaggio delle room");
              goto cmdError;
            }
            strmalloc(&rsp->field, avail_rooms);
            goto cmdSuccess;
        }

        if(msg.cmdtype == CMD_RROOM){
            
            err = startRoom(sd, msg.field);
            if(err == -1){
              strmalloc(&rsp->field, "Errore di avvio della room");
              goto cmdError;
            }
            if(err == -2){
              strmalloc(&rsp->field,"Numero di giocatori massimo della stanza ragigunto");
              goto cmdError;
            }
            if(err == -3){
              strmalloc(&rsp->field,"Room non valida");
              goto cmdError;
            }
            setGamerLocation(sd,"room",&rsp->field);
            printf("Nuova room avviata, mappa: %s\n",msg.field); 
            goto cmdSuccess;
        }

        if(msg.cmdtype == CMD_LOOK){
            err = findAsset(sd, msg.field, &rsp->field);
            if(err != 0){
              strmalloc(&rsp->field, "Errore di ");
              goto cmdError;
            }
            goto cmdSuccess; 
        }
        if(msg.cmdtype == CMD_TAKE){
            err = takeItem(sd, msg.field, &rsp->field);
            if (err == 1) {
                rsp->msgtype = MSG_SUCCESS;
                rsp->cmdtype = CMD_ANSWER;
                return 0;
            }
            if (err == 2) {
                goto cmdSuccess;
            }
            if(err != 0){
              strmalloc(&rsp->field, "Errore nel prendere l'oggetto");
              goto cmdError;
            }
            goto cmdSuccess;
        }
        if(msg.cmdtype == CMD_USE){
            char* obj_src = malloc(100);
            char* obj_dst = malloc(100);
            sscanf(msg.field, "%s %s", obj_src, obj_dst);
            int err = polymerization(sd, obj_src, obj_dst, &rsp->field);
            if(err != 0){
              strmalloc(&rsp->field, "Errore nel comando use");
              goto cmdError;
            }
            goto cmdSuccess;
        }       
  
        if(msg.cmdtype == CMD_ANSWER){
            int err = checkRiddle(sd, msg.field, &rsp->field);
            if(err != 0){
              strmalloc(&rsp->field, "Errore nel comando Answer");
              goto cmdError;
            }
            goto cmdSuccess;
        }

        if(msg.cmdtype == CMD_OBJS){
            int err = retInventory(sd, &rsp->field);
            if(err != 0){
              strmalloc(&rsp->field, "Errore di inventario");
              goto cmdError;
            }
            goto cmdSuccess;
        }
        if(msg.cmdtype == CMD_DROP){
            int err = dropItem(sd, msg.field);
            if(err != 0){
              strmalloc(&rsp->field, "Errore di rilascio oggetto");
              goto cmdError;
            }
            goto cmdSuccess;
        }

      }

bad_request:
    rsp->msgtype = MSG_BAD_REQUEST;
    rsp->field = NULL; 
    return 0;

//On command cmd success send SUCCESS and the associated CMD
cmdSuccess:
    rsp->msgtype = MSG_SUCCESS;
    rsp->cmdtype = msg.cmdtype;
    return 0; 

cmdError:
    rsp->msgtype = MSG_ERROR;
    rsp->cmdtype = msg.cmdtype;
    return 0;
}   

//Callback to handle a client disconnencting
static void disconnect_function(int sd) {
    dropGamer(sd);
}



int main(int argc, char* argv[]){

    system("clear");
    //Server creation and bind
    s = new_server(accept_function, input_function, response_function, disconnect_function, tick_function);
    
    //bind server on a free port and log it, done to avoid unix socket flushing 
    uint32_t port = 2500;
    while (bind_server(s, port) == -1) {
        port++;
    }
    log_port("./port.txt", port);
    
    //Print commands and start listening
    printFile("./menus/serverCommands.txt");
    listen_server(s);
    
    delete_server(s);
    system("> port.txt");
    return 0;
}
