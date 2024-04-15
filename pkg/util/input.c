#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int read_stdin_line(char** buff){
  size_t len = 0;
  len = getline(buff, &len, stdin); 
  if (len < 0){
    return -1;
  }
  
  (*buff)[len-1] = '\0';
  return 0;
}

//Helper function to print a file 
int printFile(const char* path){
  FILE* crd_file;
  char tmp_line[2048] = {0};
  
  crd_file = fopen(path, "r");
  if(crd_file == NULL){
    return -2;
  }
  
  
  while(fgets(tmp_line, sizeof(tmp_line), crd_file) != NULL){
      printf("%s",tmp_line);
  }

  if(fclose(crd_file)){
      return -1;
  }
  return 0; 
}

//Reads an integer form a file and returns it
int read_port(const char* path){
  FILE* crd_file;
  char tmp_line[1024] = {0};
  int port = 0;
  
  crd_file = fopen(path, "r");
  if(crd_file == NULL){
    return -2;
  }
  
  fgets(tmp_line, sizeof(tmp_line), crd_file);
  sscanf(tmp_line, "%d", &port);
  if (port != 0) {
      return port;
  }

  fclose(crd_file);
  return -1;  
}
