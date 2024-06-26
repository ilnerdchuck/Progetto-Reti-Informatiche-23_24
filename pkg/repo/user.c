#include <stdio.h>
#include <string.h>

//Gets user and password from a file 
int get_user(const char * path, const char* usr, const char* pwd){
  FILE* crd_file;
  char tmp_line[4096] = {0};
  char f_usr[2048] = {0}; 
  char f_pwd[2048] = {0};

  crd_file = fopen(path, "r");
  if(crd_file == NULL){
    return -2;
  }
  
  while(fgets(tmp_line, sizeof(tmp_line), crd_file) != NULL){
    sscanf(tmp_line, "%s %s", f_usr, f_pwd);
    if(!strcmp(f_usr, usr)){
      if(!strcmp(f_pwd, pwd)){
        fclose(crd_file);
        return 0;
      }
    }
  } 

  fclose(crd_file);
  return -1;
};

//Writes the user to the credentials file 
int create_user(const char * path, const char* usr, const char* pwd){
  FILE* crd_file;
  
  crd_file = fopen(path, "a");
  if (crd_file == NULL) {
    return -2;
  }
  
  fprintf(crd_file, "%s %s\n", usr, pwd);
  fclose(crd_file);

  if(get_user(path,usr,pwd)){
    return -1;
  }
 
  return 0;
};
