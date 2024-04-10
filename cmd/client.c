#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "./../pkg/network/network.h"
#include "./../pkg/gamer/gamer.h"
#include "./../pkg/util/util.h"

static server *s_client = NULL;

static client *c = NULL;
static int logged = 0;
static uint32_t s_client_port = 7070;

static void accept_function(int sd) {}
static void input_function(int sd, const char* inputText) {
    int err = 0;
    if(!strcmp(inputText,"login") && logged == 0){
        err = login(c);
        if (err == -2) {
            printf("Orario di chiusura\n");
            goto exit;
        }
        if (err != 0) {
            printf("Errore di login\n");
            goto exit;
        }  
        printf("Login effettuato correttamente\n");
        logged = 1;
        goto exit;
    }else if (!strcmp(inputText, "login") && logged == 1){
        printf("Comando errato sei giá loggato\n");
        goto exit;
    }

    if (!strcmp(inputText, "signup") && logged == 0) {
        err = signup(c);
        if (err != 0) {
            printf("Errore di registrazione");
            goto exit;
        }
        printf("Registrazione e login effettuati correttamente\n");
        logged = 1;
        goto exit;

    }else if(!strcmp(inputText, "signup") && logged == 1){
        printf("Comando errato hai effettuato il login\n");
        goto exit;
    }
    
    
    
exit:
    return;
  
}
static int response_function(int sd, const message msg, message *rsp) {

  return 0;
}

int main(int argc, char* argv[]){

    system("clear");
    printf(" _____ _____ _____   ___  ______ _____   ______ _____  ________  ___\n");
    printf("|  ___/  ___/  __ \\ / _ \\ | ___ \\  ___|  | ___ \\  _  ||  _  |  \\/  |\n");
    printf("| |__ \\ `--.| /  \\ / /_\\ \\| |_/ / |__    | |_/ / | | || | | | .  . |\n");
    printf("|  __| `--. \\ |    |  _  ||  __/|  __|   |    /| | | || | | | |\\/| |\n");
    printf("| |___/\\__/ / \\__/\\| | | || |   | |___   | |\\ \\\\ \\_/ /\\ \\_/ / |  | |\n");
    printf("\\____/\\____/ \\____/\\_| |_/\\_|   \\____/   \\_| \\_|\\___/  \\___/\\_|  |_/\n");
    int port = read_port("./port.txt");
    while(port == -1){
        port = read_port("./port.txt");
    }
    c = new_client("127.0.0.1", port);
    s_client = new_server(accept_function, input_function, response_function);
    
    while (bind_server(s_client, s_client_port) == -1) {
        s_client_port++;
    } 

    printf("Autenticazione\n");
    printf("Comandi disponibili:\n");
    printf("\t -> signup -- Registrati nel sistema (con login automatico)\n");
    printf("\t -> login -- Effettua il login\n");
    
    listen_server(s_client);
}
