#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gamer.h"
#include "./../network/network.h"
#include "./../protocol/protocol.h"
#include "./../util/util.h"
#include "./../string/string.h"

int gamerChat[5] = {0};
char* username = NULL;

// Gamer logic
//-----------------Commands Request-----------------
//All callbacks send a cmdtype and wait for a MSG_SUCCESS
//with the same cmdtype or on error it will return a 
//MSG_ERROR with the corrisponding cmdtype
int requestRooms(client* c){
      message msg = {0};
      msg.msgtype = MSG_COMMAND;
      msg.cmdtype = CMD_RROOMS;
      
      message rsp = {0};

      int err = request(c,msg,&rsp);
      if(err != 0){
          return -1;
      }
      if(rsp.msgtype == MSG_SUCCESS && rsp.cmdtype == msg.cmdtype){
          printf("%s\n",rsp.field);
          printf("%s\n", username);
          return 0;
      }
      return -1;
}

int requestRoom(client* c, const char* room){
      message msg = {0};
      msg.msgtype = MSG_COMMAND;
      msg.cmdtype = CMD_RROOM;
      strmalloc(&msg.field, room);
      
      message rsp = {0};

      int err = request(c,msg,&rsp);
      if(err != 0){
          return -1;
      }
        
      if(rsp.msgtype == MSG_SUCCESS && rsp.cmdtype == msg.cmdtype){
          printFile("./menus/roomCommands.txt");
          printf("%s\n",rsp.field);
          return 0;
      }
      if(rsp.msgtype == MSG_ERROR && rsp.cmdtype == msg.cmdtype){
          printf("%s\n",rsp.field);
          return -1;
      }

      return 0;
}


int requestLook(client* c, const char* asset){
      message msg = {0};
      msg.msgtype = MSG_COMMAND;
      msg.cmdtype = CMD_LOOK;
      strmalloc(&msg.field, asset);
      
      message rsp = {0};

      int err = request(c,msg,&rsp);
      if(err != 0){
          return -1;
      }
        
      if(rsp.msgtype == MSG_SUCCESS && rsp.cmdtype == msg.cmdtype){
          printFile("./menus/roomCommands.txt");
          printf("%s\n",rsp.field);
          return 0;
      }
      if(rsp.msgtype == MSG_ERROR && rsp.cmdtype == msg.cmdtype){
          printf("%s\n",rsp.field);
          return -1;
      }

      return 0;
}

int requestObj(client* c){
      message msg = {0};
      msg.msgtype = MSG_COMMAND;
      msg.cmdtype = CMD_OBJS;
      
      message rsp = {0};

      int err = request(c,msg,&rsp);
      if(err != 0){
          return -1;
      }
        
      if(rsp.msgtype == MSG_SUCCESS && rsp.cmdtype == msg.cmdtype){
          printFile("./menus/roomCommands.txt");
          printf("%s\n",rsp.field);
          return 0;
      }
      if(rsp.msgtype == MSG_ERROR && rsp.cmdtype == msg.cmdtype){
          printf("%s\n",rsp.field);
          return -1;
      }

      return 0;
}

int requestTake(client* c, const char* item){
      message msg = {0};
      msg.msgtype = MSG_COMMAND;
      msg.cmdtype = CMD_TAKE;
      strmalloc(&msg.field, item);
      
      message rsp = {0};

      int err = request(c,msg,&rsp);
      if(err != 0){
          return -1;
      }

      if(rsp.msgtype == MSG_SUCCESS && rsp.cmdtype == msg.cmdtype){
          return 0;
      }
      //On take if the item needs a riddle it will answer with a 
      //CMD_ANSWER
      if(rsp.msgtype == MSG_SUCCESS && rsp.cmdtype == CMD_ANSWER){
          printf("%s\n",rsp.field);
          char* buff = NULL;
          read_stdin_line(&buff);
          
          message ans = {0};
          char* answer = malloc(strlen(buff)+strlen(msg.field)+2); 
          ans.msgtype = MSG_COMMAND;
          ans.cmdtype = CMD_ANSWER;
          sprintf(answer, "%s %s", msg.field, buff);
          strmalloc(&ans.field, answer);
            
          message res = {0};

          err = request(c, ans, &res);
          if (err != 0) {
              return -1;
          }
          if(res.msgtype == MSG_SUCCESS && res.cmdtype == ans.cmdtype){
              printf("%s\n",res.field);
              return 0;
          }

          if(res.msgtype == MSG_ERROR && res.cmdtype == ans.cmdtype){
              printf("%s\n",rsp.field);
              return -1;
          }

              
          return 0;
      } 


      if(rsp.msgtype == MSG_ERROR && rsp.cmdtype == msg.cmdtype){
          printf("%s\n",rsp.field);
          return -1;
      }

      return 0;
}

int usePolymerization(client* c, char* obj_src,char* obj_dst){
      message msg = {0};
      msg.msgtype = MSG_COMMAND;
      msg.cmdtype = CMD_USE;

      char* buff = malloc(strlen(obj_src)+strlen(obj_dst)+2);
      sprintf(buff,"%s %s", obj_src, obj_dst);
      strmalloc(&msg.field, buff);
      free(buff);

      message rsp = {0};

      int err = request(c,msg,&rsp);
      if(err != 0){
          return -1;
      }
        
      if(rsp.msgtype == MSG_SUCCESS && rsp.cmdtype == msg.cmdtype){
          printFile("./menus/roomCommands.txt");
          printf("%s\n",rsp.field);
          return 0;
      }
      if(rsp.msgtype == MSG_ERROR && rsp.cmdtype == msg.cmdtype){
          printf("%s\n",rsp.field);
          return -1;
      }

      return 0; 
}

int requestDrop(client* c, const char* item){
      message msg = {0};
      msg.msgtype = MSG_COMMAND;
      msg.cmdtype = CMD_DROP;
      strmalloc(&msg.field, item);
      
      message rsp = {0};

      int err = request(c,msg,&rsp);
      if(err != 0){
          return -1;
      }
        
      if(rsp.msgtype == MSG_SUCCESS && rsp.cmdtype == msg.cmdtype){
          return 0;
      }

      if(rsp.msgtype == MSG_ERROR && rsp.cmdtype == msg.cmdtype){
          printf("%s\n",rsp.field);
          return -1;
      }

      return 0;
}

int requestEnd(client* c){
      message msg = {0};
      msg.msgtype = MSG_COMMAND;
      msg.cmdtype = CMD_END;

      message rsp = {0};

      int err = request(c,msg,&rsp);
      if(err != 0){
          return -1;
      }
        
      if(rsp.msgtype == MSG_SUCCESS && rsp.cmdtype == msg.cmdtype){
          return 0;
      }

      if(rsp.msgtype == MSG_ERROR && rsp.cmdtype == msg.cmdtype){
          printf("%s\n",rsp.field);
          return -1;
      }

      return 0;
}

//TODO: fix when a disconnect happens the ports
int sayToRoom(client* c, char* buff){

    //richiedi le porte degli utenti
    message msgP = {0};
    msgP.msgtype = MSG_COMMAND;
    msgP.cmdtype = CMD_RGAMERS;
    
    message ports = {0};

    int err = request(c, msgP, &ports);
    if(err != 0){
        return -1;
    }

    char* tk = strtok(ports.field, " ");
    int i = 0;
    while(tk){
        sscanf(tk, "%d", &gamerChat[i]);
        i++;
        tk = strtok(NULL, " ");
    }


    for(int i=0; i<5; i++){
        client* cc = new_client("127.0.0.1", gamerChat[i]);
        message msg = {0};
        msg.msgtype = MSG_TEXT;
        msg.cmdtype = CMD_CHAT;
        msg.field = malloc(strlen(buff) + strlen(username) +1);
        sprintf(msg.field, "%s: %s", username, buff);
        
        message rsp = {0};

        err = request(cc, msg, &rsp);
        delete_client(cc);
    }
    return 0;
}

//------------------Auth-----------------
int login(client* c, const int c_s_port){
    int err = 0;
  //Read username and password
    char* usr = NULL;
    while(1){
      printf("Inserisci l'username (Senza spazi):\n");
      err = read_stdin_line(&usr);
      if (err != 0) {
          free(usr);
          return -1;
      }
      if(!strcmp(usr, "") || strchr(usr,' ')){
        printf("Username non valido\n");
        continue;
      }
      break;
    }
    
    char* pwd1 = NULL;

    while(1){
      printf("Inserisci la password (senza spazi):\n");
      
      err = read_stdin_line(&pwd1);
      if (err != 0) {
          return -1;
      }

      if (!strcmp(pwd1, "") || strchr(pwd1, ' ')) {
        printf("Password non valida\n");
        continue;
      }else{
        break;
      }
    }
    message credentials = {0};
    username = malloc(strlen(usr)+1);
    memset(username, 0, strlen(usr)+1);
    strcpy(username, usr);
 
    credentials.field = malloc(strlen(usr) + strlen(pwd1) +2);
    sprintf(credentials.field, "%s %s %d", usr, pwd1, c_s_port);
    credentials.msgtype = MSG_COMMAND;
    credentials.cmdtype = CMD_LOGIN;
    free(usr);
    free(pwd1);
    message rsp = {0};
    request(c, credentials, &rsp);
    
    if(rsp.msgtype == MSG_ERROR && rsp.cmdtype == CMD_LOGIN){
        return -1;
    }else if (rsp.msgtype == MSG_SUCCESS && rsp.cmdtype == CMD_LOGIN) {
                return 0;
    }
    
    if(rsp.msgtype == MSG_BAD_REQUEST){
      return -2;
    }

    return -1;
}

int signup(client* c, const int c_s_port){
    char* usr = NULL;
    int err = 0;

    while(1){
      printf("Inserisci l'username (Senza spazi):\n");
      err = read_stdin_line(&usr);
      if (err != 0) {
          return -1;
      }
      if(!strcmp(usr, "") || strchr(usr,' ')){
        printf("Username non valido\n");
        continue;
      }
      break;
    }
    
    char* pwd1 = NULL;
    char* pwd2 = NULL;

    while(1){
      printf("Inserisci la password (senza spazi):\n");
      err = read_stdin_line(&pwd1);
      if (err != 0) {
          return -1;
      }

      if (!strcmp(pwd1, "") || strchr(pwd1, ' ')) {
       printf("Password non valida\n");
        continue;
      }

      printf("Inseriscila ancora:\n");
      err = read_stdin_line(&pwd2);
      if (err != 0) {
          return -1;
      }
      if(!strcmp(pwd1, pwd2)){
        break;
      }
      printf("Password non uguali\n");
    }
    
    message credentials = {0};
    
    //Send username and password register command
  
    credentials.field = malloc(strlen(usr) + strlen(pwd1) +2);
    sprintf(credentials.field, "%s %s %d", usr, pwd1, c_s_port);
    credentials.msgtype = MSG_COMMAND;
    credentials.cmdtype = CMD_SIGNUP;
    
    message rsp = {0};

    request(c, credentials, &rsp);
    
    //Check if registration is successful
    if(rsp.msgtype == MSG_ERROR && rsp.cmdtype == CMD_SIGNUP){
        return -1;
    }else if (rsp.msgtype == MSG_SUCCESS && rsp.cmdtype == CMD_SIGNUP) {
        strmalloc(&username, usr);
        return 0;
    }
    if(rsp.msgtype == MSG_BAD_REQUEST && !strcmp(rsp.field, "closed")){
        return -2;
    }

    return -1;
}


