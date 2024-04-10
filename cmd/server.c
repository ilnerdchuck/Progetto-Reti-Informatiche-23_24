#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "./../pkg/string/string.h"
#include "./../pkg/network/network.h"
#include "./../pkg/protocol/protocol.h"
#include "./../pkg/EscapeRoom/room.h"
#include "./../pkg/repo/repo.h"

static server *s;
static int esroom_open = 0;
gamer* gamer_list = NULL;
game_room* room_list = NULL;



static void accept_function(int sd) {

}

static void input_function(int sd, const char* inputText) {
    if (!strcmp(inputText, "start") && esroom_open == 0) {
        esroom_open = 1;
        printf("entrate signori\n");
        return;
    }else if (!strcmp(inputText, "start") && esroom_open ==1) {
        printf("Sei giá aperto\n");
        return;
    }

    if(!strcmp(inputText, "print gamers")){
        printGamers(gamer_list);
        return;
    }
    
    if(!strcmp(inputText, "stop") && esroom_open == 1){
        stop_server(s);
        return;
    }else if(!strcmp(inputText, "stop") && esroom_open == 0){
        printf("Almeno prima apri il negozio\n");
        return;
    }
    printf("Comando errato\n");
}

static int response_function(int sd, const message msg, message *rsp) {
    if(esroom_open == 0){ 
        strmalloc(rsp->field,"closed");
        goto bad_request;
    }

    if(msg.msgtype == MSG_COMMAND){
        if (msg.cmdtype == CMD_LOGIN){
            char* usr = malloc(strlen(msg.field)+1);
            char* pwd = malloc(strlen(msg.field)+1); 
            int err = 0;
            
            //Get credentials from the message
            err = get_usr_pwd(msg.field, usr, pwd);
            if (err != 0) { 
                strmalloc(rsp->field,"Errore di parsing");
                goto cmdError;
            }
            
            //Gamer login
            err = loginGamer(sd, usr, pwd);
            if (err != 0) {
                strmalloc(rsp->field, "Errore di login");
                goto cmdError;
            }
            printf("Accesso effettuato da:\n");
            printGamer(gamer_list, sd);
      
            goto cmdSuccess;
        }

        if (msg.cmdtype == CMD_SIGNUP){
            char* usr = malloc(strlen(msg.field)+1);
            char* pwd = malloc(strlen(msg.field)+1); 
            int err = 0;
            
            //Get credentials
            err = get_usr_pwd(msg.field, usr, pwd);
            if (err != 0) {
                
                strmalloc(rsp->field,"Errore di parsing");
                goto cmdError; 
            }

            //Gamer register
            err = signupGamer(usr, pwd);
            if (err != 0) {
                strmalloc(rsp->field,"Errore di signup");
                goto cmdError; 
            }
            
            //Gamer login
            err = loginGamer(sd, usr, pwd);
            if (err != 0) {
                strmalloc(rsp->field,"Errore di login");
                goto cmdError;
            }
            goto cmdSuccess;
        }
      }

bad_request:
    rsp->msgtype = MSG_BAD_REQUEST;
    rsp->field = NULL; 
    return 0;

cmdSuccess:
    rsp->msgtype = MSG_SUCCESS;
    rsp->cmdtype = msg.cmdtype;
    return 0; 

cmdError:
    rsp->msgtype = MSG_ERROR;
    rsp->cmdtype = msg.cmdtype;
    return 0;
}   

int main(int argc, char* argv[]){

    system("clear");

    s = new_server(accept_function, input_function, response_function);
    
    uint32_t port = 2500;
    while (bind_server(s, port) == -1) {
        port++;
    }
    printf("Benvenuto maestro\n Comandi disponibili:\n");
    printf("start -> start the server EscapeRoom\n");
    printf("stop -> stop the server\n");
    
    log_port("./port.txt", port);
    
    listen_server(s);
    
    system("> port.txt");

}
