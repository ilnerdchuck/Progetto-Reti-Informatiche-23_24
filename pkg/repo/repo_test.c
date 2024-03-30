#include "./../test.h"
#include "repo.h"
#include <stdlib.h>
//@ERROR Why it gets multiple definitions of fmt_Sprintf
//if i include it?

//#include "./../string/string.h"

#define REPO_PATH "./tmp/creds.txt"

int TestLogin(){
    system("echo maio kek > ./tmp/creds.txt");

    // login(banana e mazzi)

    system("echo maio miao > ./tmp/creds.txt");

    // qui la fai fallire

    system("rm ./tmp/creds.txt");
    return 0;
}
