#include <stdio.h>
#include <unistd.h>

int read_stdin_line(char* buff){
  if(fgets(buff, 4096, STDIN_FILENO) == NULL){
    return -1;
  }
  return 0;
}
