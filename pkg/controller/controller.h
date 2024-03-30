// int find_credentials(char* usr, char* pwd){
//   FILE* crd_file;
//   char tmp_line[4096];
//   char* f_usr = NULL; 
//   char* f_pwd = NULL;

//   crd_file = fopen("credentials.txt", "r");
//   if(crd_file == NULL){
//     printf("Errore di apertura del file di credenziali\n");
//     return 0;
//   }
  
//   while(fgets(tmp_line, sizeof(tmp_line), crd_file) != NULL){
//     sscanf(tmp_line, "%s %s", f_usr, f_pwd);
//     if(!strcmp(f_usr, usr)){
//       if(!strcmp(f_pwd, pwd)){
//         printf("Utente Registrato Evviva\n");
//         fclose(crd_file);
//         return 1;
//       }
//     }
//   }

//   printf("Utente non Registrato\n");
  
//   fclose(crd_file);
//   return 0;
// };

// int login(char* usr, char* pwd){
//   if(!find_credentials(usr, pwd)){
//     printf("Utente Non Registrato\n");
//     return -1;
//   }
  
//   printf("Utente Registrato\n");
//   return 0;
// };


// int resgister(char* usr, char* pwd){
//   //leggere da file e confrontare con utente e password
//   if (!find_credentials(usr, pwd)) {
//     printf("Utente giá registrato:\n");
//     return -1;
//   }
//   //scrivi l'untente nel file
//   FILE* crd_file;
  
//   crd_file = fopen("credentials.txt", "a");
//   if (crd_file == NULL) {
//     printf("errore nell'apertura del file\n");
//     return -1;
//   }
  
//   fprintf(crd_file, "%s %s", usr, pwd);
//   fclose(crd_file);

//   if(login(usr,pwd)){
//     printf("Errore durante la registrazione dell'utente\n");
//     return -1;
//   }
  
//   printf("Utente registrato correttamente\n");
//   return 0;
// };



