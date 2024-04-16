#include <bits/types/struct_timeval.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "./../pkg/network/network.h"
#include "./../pkg/gamer/gamer.h"
#include "./../pkg/util/util.h"

static server *s_client = NULL;

static client *c = NULL;
static int logged = 0;
static int client_gaming = 0;
static uint32_t s_client_port = 7070;


static void accept_function(int sd) {}
static void tick_function(struct timeval time) {}

//Callback for handling client stdin, returns != 0 on error
static void input_function(int sd, const char* inputText) {
    int err = 0;
    if(!logged){
      if(!strcmp(inputText,"login") && logged == 0){
        err = login(c, s_client_port);
        if (err == -2) {
            printf("Orario di chiusura\n");
            goto exit;
        }
        if (err != 0) {
            printf("Errore di login\n");
            goto exit;
        }
        system("clear");
        printf("Login effettuato correttamente\n");
        printFile("./menus/clientCommands.txt");
        logged = 1;
        goto exit;
      }

      if (!strcmp(inputText, "signup")) {
        err = signup(c, s_client_port);
        if (err != 0) {
            printf("Errore di registrazione");
            goto exit;
        }
        printf("Registrazione e login effettuati correttamente\n");
        logged = 1;
        printFile("./menus/clientCommands.txt");
        goto exit;
      }
    }

    if(logged && client_gaming == 0){
        //get command
        char curr_command[30] = {0};
        char curr_value[30] = {0};
        sscanf(inputText, "%s %s", curr_command, curr_value);

        if(!strcmp(curr_command, "show") && !strcmp(curr_value, "rooms") ){
            err = requestRooms(c);
            if(err != 0){
                printf("errore nella richiesta delle stanze\n");
                goto exit;
            }
            goto exit;
        }

        if (!strcmp(curr_command, "start")) {
            err = requestRoom(c, curr_value);
            if (err != 0) {
                goto exit;
            }
            client_gaming = 1;
            goto exit;
        }

        if (!strcmp(curr_command, "login")){
            printf("Hai giá effettuato il login\n");
            goto exit;
        }

        if(!strcmp(curr_command, "signup")){
            printf("Comando errato hai effettuato il login\n");
            goto exit;
        }
    }
    //Gamer commands while in a room
    if(logged && client_gaming){
        //get command
        char curr_command[30] = {0};
        char curr_value1[30] = {0};
        char curr_value2[30] = {0};
        sscanf(inputText, "%s %s %s", curr_command, curr_value1, curr_value2);
        
        if (!strcmp(curr_command, "look")) {
            err = requestLook(c, curr_value1);
            if (err != 0) {
                goto exit;
            }
            goto exit;
        }      

        if (!strcmp(curr_command, "take")) {
            err = requestTake(c, curr_value1);
            if (err != 0) {
                goto exit;
            }
            printf("Oggetto aggiunto all'inventario\n");
            goto exit;
        }      


        if (!strcmp(curr_command, "use")) {
            err = usePolymerization(c, curr_value1, curr_value2);
            if (err != 0) {
                goto exit;
            }
            goto exit;
        }

        if (!strcmp(curr_command, "objs")) {
            err = requestObj(c);
            if (err != 0) {
                goto exit;
            }
            goto exit;
        } 
        
        if (!strcmp(curr_command, "drop")) {
            err = requestDrop(c, curr_value1);
            if (err != 0) {
                goto exit;
            }
            goto exit;
        }

        if (!strcmp(curr_command, "say")) {
            char* buff = malloc(1024);
            sscanf(inputText,"%s %1024[^\n]",curr_command, buff);
            int err = sayToRoom(c, buff);
            if(err != 0){
                goto exit;
            }
            goto exit;
        }

        if (!strcmp(curr_command, "end")) {
            stop_server(s_client);
            goto exit;
        }
     }
    
    printf("Comando non disponibile\n");
    
exit:
    return;
  
}

//Callback for handling server messages, returns != 0 on error
static int response_function(int sd, const message msg, message *rsp) {
    if(msg.msgtype == MSG_TEXT){
        printf("%s\n", msg.field);
        if(msg.cmdtype == CMD_WIN || msg.cmdtype == CMD_LOSS){
            client_gaming = 0;
        }
        rsp->msgtype = MSG_SUCCESS;
    }
    return 0;
}

void disconnect_function(int sd) {}

int main(int argc, char* argv[]){

    system("clear");
    //Read port or wait and then connect to the server 
    int port = read_port("./port.txt");
    while(port == -1){
        port = read_port("./port.txt");
    }
    c = new_client("127.0.0.1", port);
    
    //bind a client server on a free port and log it, done to avoid unix socket flushing 
    s_client = new_server(accept_function, input_function, response_function, disconnect_function, tick_function);
    while (bind_server(s_client, s_client_port) == -1) {
        s_client_port++;
    } 
      
    printFile("./menus/initClient.txt");
    listen_server(s_client);
    
    delete_server(s_client);
    delete_client(c);
    return 0;
}
