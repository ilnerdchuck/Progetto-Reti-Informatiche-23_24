#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "./../pkg/network/network.h"
#include "./../pkg/gamer/gamer.h"
#include "./../pkg/util/util.h"

server *s_client = NULL;

client *c = NULL;
int logged = 0;
uint32_t s_client_port = 7070;

void accept_function(int sd) {}
void input_function(int sd, const char* inputText) {
    int err = 0;
    printf("hello\n");
    if(!strcmp(inputText,"login") && logged == 0){
        err = login(c);
        if (err != 0) {
            printf("errore di login\n");
            goto error;
        }  
        
        logged = 1;
        printf("accesso eseguito correttamente");
        return;
    }
    if (!strcmp(inputText, "signup") && logged == 0) {
        err = signup(c);
        if (err != 0) {
            printf("errore di registrazione");
            goto error;
        }
        err = login(c);
        if (err != 0) {
            printf("errore di login nella registrazione");
            goto error;
        }
        return;
    }
    

error:
    return;
  
}
int response_function(int sd, const message msg, message *rsp) {

  return 0;
}

static void spacca(int signaln)
{
  abort();
}

int main(int argc, char* argv[]){
    signal(SIGPIPE, spacca);
    // printf("▄███▄     ▄▄▄▄▄   ▄█▄    ██   █ ▄▄  ▄███▄       █▄▄▄▄ ████▄ ████▄ █▀▄▀█ \n");
    // printf("█▀   ▀   █     ▀▄ █▀ ▀▄  █ █  █   █ █▀   ▀      █  ▄▀ █   █ █   █ █ █ █ \n");
    // printf("██▄▄   ▄  ▀▀▀▀▄   █   ▀  █▄▄█ █▀▀▀  ██▄▄        █▀▀▌  █   █ █   █ █ ▄ █ \n");
    // printf("█▄   ▄▀ ▀▄▄▄▄▀    █▄  ▄▀ █  █ █     █▄   ▄▀     █  █  ▀████ ▀████ █   █ \n");
    // printf("▀███▀             ▀███▀     █  █    ▀███▀         █                  █  \n");
    // printf("                           █    ▀                ▀                  ▀   \n");
    // printf("                          ▀\n");    
   

    printf(" _____ _____ _____   ___  ______ _____   ______ _____  ________  ___\n");
    printf("|  ___/  ___/  __ \\ / _ \\ | ___ \\  ___|  | ___ \\  _  ||  _  |  \\/  |\n");
    printf("| |__ \\ `--.| /  \\ / /_\\ \\| |_/ / |__    | |_/ / | | || | | | .  . |\n");
    printf("|  __| `--. \\ |    |  _  ||  __/|  __|   |    /| | | || | | | |\\/| |\n");
    printf("| |___/\\__/ / \\__/\\| | | || |   | |___   | |\\ \\\\ \\_/ /\\ \\_/ / |  | |\n");
    printf("\\____/\\____/ \\____/\\_| |_/\\_|   \\____/   \\_| \\_|\\___/  \\___/\\_|  |_/\n");
    
    c = new_client("127.0.0.1", 6969);
    s_client = new_server(accept_function, input_function, response_function);
    
    bind_server(s_client, s_client_port); 
  printf("Autenticazione\n");
  printf("Comandi disponibili:\n");
  printf("\t -> signup -- Registrati nel sistema (con login automatico)\n");
  printf("\t -> login -- Effettua il login\n");
  
  listen_server(s_client);
}
