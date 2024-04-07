#include <stdio.h>
#include <stdlib.h>

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
