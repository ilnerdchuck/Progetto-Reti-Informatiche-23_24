#include "./../test.h"
#include "./repo/repo.h"
#include "./../string/string.h"

#define REPO_PATH "/tmp/creds.txt"

int TestLogin(){
    system(fmt_Sprintf("echo maio kek > %s", REPO_PATH));

    // login(banana e mazzi)

    system(fmt_Sprintf("echo maio miao > %s", REPO_PATH));

    // qui la fai fallire

    system(fmt_Sprintf("echo maio kek > %s", REPO_PATH));
}