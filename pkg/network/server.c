#include <arpa/inet.h>
#include <malloc.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "network.h"

server* new_server(AcceptFunction a, InputFunction i, ResponseFunction r) {
    // TODO: check that functions are not null
    server* s = malloc(sizeof(server));
    if (s == NULL) {
        goto error;
    }

    s->a = a;
    s->i = i;
    s->r = r;

    s->listener = socket(AF_INET, SOCK_STREAM, 0);
    if (s->listener == -1) {
        goto error;
    }

    return s;
error:
    return NULL;
}

int bind_server(server* s, int port) {
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    return bind(s->listener, (struct sockaddr*)&address, sizeof(address));
} 

int listen_server(server* s) {
    fd_set master, read_fds;

    struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};

    listen(s->listener, 10);

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    // FD_SET(STDIN_FILENO, &master);
    FD_SET(s->listener, &master);

    int fdmax = s->listener;

    while (1) {
        read_fds = master;

        select(fdmax + 1, &read_fds, NULL, NULL, &timeout);

        for (int fd = 0; fd <= fdmax; fd++) {
            if (!FD_ISSET(fd, &read_fds)) continue;

            // TODO: add input callback and fd
            if (fd == STDIN_FILENO) {
                // TODO:
                // receive the message from stdin
                //s->i(sd, msg);
                continue;
            }
            // add a connection in case the full sd is the listener
            if (fd == s->listener) {
                struct sockaddr_in cl_addr;
                uint32_t addrlen = sizeof(cl_addr);

                int newsd = accept(s->listener, 
                                 (struct sockaddr*)&cl_addr,
                             (socklen_t*)&addrlen);

                FD_SET(newsd, &master);
                if (newsd > fdmax) {
                    fdmax = newsd;
                }
                // call accept callback
                s->a(fd);
                continue;
            }

            // in case of a message call the receive
            char* msgg;
            int err = _receive(fd, &msgg);

            // in case of an error close the socket
            if (err == -1) {
                FD_CLR(fd, &master);
                close(fd);
                // TODO: add callback for disconnect
                continue;
            }

            // call the message callback
            char* rsp = NULL;
            err = s->r(fd, msgg, &rsp);
            if (err == -1) {
                FD_CLR(fd, &master);
                close(fd);
                // TODO: add callback for disconnect
                continue;
            }

            // otherwise send the response
            msg tmp = {0};

            strcpy(tmp.field, rsp);
            err = _send(fd, rsp);

            free(rsp);
            rsp = NULL;

            free(msgg);
            msgg = NULL;

            if (err == -1) {
                FD_CLR(fd, &master);
                close(fd);
                // TODO: add callback for disconnect
                continue;
            }
        }
    }
}

void delete_server(server* s) {
    // TODO: delete
}

int find_credentials(char* usr, char* pwd){
  FILE* crd_file;
  char tmp_line[4096];
  char* f_usr = NULL; 
  char* f_pwd = NULL;

  crd_file = fopen("credentials.txt", "r");
  if(crd_file == NULL){
    printf("Errore di apertura del file di credenziali\n");
    return 0;
  }
  
  while(fgets(tmp_line, sizeof(tmp_line), crd_file) != NULL){
    sscanf(tmp_line, "%s %s", f_usr, f_pwd);
    if(!strcmp(f_usr, usr)){
      if(!strcmp(f_pwd, pwd)){
        printf("Utente Registrato Evviva\n");
        fclose(crd_file);
        return 1;
      }
    }
  }

  printf("Utente non Registrato\n");
  
  fclose(crd_file);
  return 0;
};

int login(char* usr, char* pwd){
  if(!find_credentials(usr, pwd)){
    printf("Utente Non Registrato\n");
    return -1;
  }
  
  printf("Utente Registrato\n");
  return 0;
};


int resgister(char* usr, char* pwd){
  //leggere da file e confrontare con utente e password
  if (!find_credentials(usr, pwd)) {
    printf("Utente giá registrato:\n");
    return -1;
  }
  //scrivi l'untente nel file
  FILE* crd_file;
  
  crd_file = fopen("credentials.txt", "a");
  if (crd_file == NULL) {
    printf("errore nell'apertura del file\n");
    return -1;
  }
  
  fprintf(crd_file, "%s %s", usr, pwd);
  fclose(crd_file);

  if(login(usr,pwd)){
    printf("Errore durante la registrazione dell'utente\n");
    return -1;
  }
  
  printf("Utente registrato correttamente\n");
  return 0;
};



