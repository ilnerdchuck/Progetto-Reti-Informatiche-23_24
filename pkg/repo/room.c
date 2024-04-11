#define _DEFAULT_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>



#include "./../EscapeRoom/room.h"
#include "./../string/string.h"
#include "repo.h"

#define N_LOCATIONS 4

int log_port(const char * path, const int port){
  FILE* p_file;
  system("> ./port.txt ");
  system("touch ./port.txt");  
  p_file = fopen(path, "a");
  if (p_file == NULL) {
    return -2;
  }
  
  fprintf(p_file, "%i\n", port);
  fclose(p_file);
 
  return 0;
  
}

//------------Room Handling---------------

//@TODO: review handling 
int getAvalibleRooms(char **room_list){
    
    system("/bin/ls ./room_template | cut -f1  > ./tmp.txt");
     
    char buf[4096] = {0};
    FILE* fp;

    fp = fopen("tmp.txt", "r");
    if (fp == NULL) {
        printf("Error opening pipe!\n");
        return -1;
    }
    *room_list = malloc(4096);
  
    memset(*room_list, 0, 4096);

    while (fgets(buf, sizeof(buf), fp) != NULL) {
          strcat(*room_list, buf);
    }

    if (fclose(fp)) {
        printf("Command not found or exited with error status\n");
        system("rm ./tmp.txt");
        return -1;
    }
    system("rm ./tmp.txt");

    return 0;
  
}


int insertLocation(location** locations, const char* name, const char* desc){
    //inserimento in testa non mi interessa l'ordine
    location* new_location = (location*)malloc(sizeof(location));
    new_location->name = NULL;
    strmalloc(&new_location->name, name);
    new_location->desc_location = NULL;
    strmalloc(&new_location->desc_location, desc);
    new_location->items = NULL;
    new_location->next_location = NULL;
    
    //@TODO: add error handling (mostly null strings)
    if(!(*locations)){
        *locations = new_location;
        return 0;
    }
    
    new_location->next_location = *locations;
    *locations = new_location;

    return 0;
}

//inserisco in testa poiché la location in testa sará sempre quella corrente
int insertIteminLocation(location* locations, 
               const char* item_name,
               itemType item_type,
               int item_token,
               const char* item_unlocked,
               const char* item_answer,
               const char* item_succ_mess,
               const char* item_descl,
               const char* item_descu){
    if(locations == NULL){
        return -1;
    }  

    //inserimento in testa dell'item nella location
    item* new_item = (item*)malloc(sizeof(item));
    new_item->name = NULL;
    strmalloc(&new_item->name, item_name);
  
    new_item->itemType = item_type;
    new_item->token = item_token;
    
    //@TODO: add error handling (mostly null strings)
    strmalloc(&new_item->unloked_item, item_unlocked);
    strmalloc(&new_item->answer, item_answer);
    strmalloc(&new_item->success_message, item_succ_mess);
    strmalloc(&new_item->desc_locked, item_descl);
    strmalloc(&new_item->desc_unlocked, item_descu);
  
    new_item->next_item = NULL;

    if(!locations->items){
        locations->items = new_item;
        return 0;
    }

    new_item->next_item = locations->items;
    locations->items = new_item;

    return 0;
}


int getLocations(const char* map, location** locations, int* tokens){
    
    // char* command = malloc(4096);
    // 
    // strcat(command, "/bin.ls ./room_template/");
    // strcat(command, map);
    // strcat(command," | cut -f1 > tmp.txt");
    //
    // system(command);
    // 
    // char buf[4096] = {0};
    // FILE* fp;
    //
    // fp = fopen("tmp.txt", "r");
    // if (fp == NULL) {
    //     printf("Error opening pipe!\n");
    //     return -1;
    // }
    // 
    // int n_maps = 0;
    // while (fgets(buf, sizeof(buf), fp) != NULL) {
    //       n_maps++;
    // }
    //
    // if (fclose(fp)) {
    //     printf("Command not found or exited with error status\n");
    //     system("rm ./tmp.txt");
    //     return -1;
    // }   
    // 

    struct dirent **namelist;
    int n;
    char* path = malloc(1024);
    strcpy(path, "./room_template/");
    strcat(path, map);
    strcat(path, "/");
    
    n = scandir(path, &namelist, NULL, alphasort);

    FILE* fp;

    for (int i = 2; i<n; i++) {
        strcat(path,namelist[i]->d_name);
        
        fp = fopen(path, "r");
        if (fp == NULL) {
            printf("Error opening pipe!\n");
            return -1;
        }

        char tmp_line[4096] = {0};
        if(fgets(tmp_line, sizeof(tmp_line), fp) != NULL){
            //inserimento nuova location
            // con descrizione in tmp tmp_line
            insertLocation(locations,namelist[i]->d_name,tmp_line);
        }else {
            continue;
        } 

        while (fgets(tmp_line, sizeof(tmp_line), fp) != NULL) {
            
            char item_name[100] = {0}; 
            itemType item_type = 0;
            int item_token = 0;
            char item_unlocked[100] = {0};
            char item_answer[100] = {0};
            char item_succ_mess[100] = {0};
            char item_descl[300] = {0};
            char item_descu[300] = {0};
            
                  
            //aggiunge l'item alla location corrente 
            sscanf(tmp_line,"%[^_]_%d_%d_%[^_]_%[^_]_%[^_]_%[^_]_%[^_]",
                   item_name,
                   (int*)&item_type,
                   &item_token,
                   item_unlocked,
                   item_answer,
                   item_succ_mess,
                   item_descl,
                   item_descu
                  );
            if(item_token){
                *tokens = *tokens + 1;
            }
            insertIteminLocation(*locations, 
                                 item_name, 
                                 item_type, 
                                 item_token, 
                                 item_unlocked, 
                                 item_answer, 
                                 item_succ_mess, 
                                 item_descl, 
                                 item_descu);
 
        }

        if (fclose(fp)) {
            printf("Command not found or exited with error status\n");
            return -1;
        }
        strcpy(path, "./room_template/");
        strcat(path, map);
        strcat(path, "/");
    }
    return 0;
}

