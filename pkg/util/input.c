#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int read_stdin_line(char* buff){
  if(fgets(buff, 4096, STDIN_FILENO) == NULL){
    return -1;
  }
  return 0;
}

//reads an integer form a file and returns it
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
