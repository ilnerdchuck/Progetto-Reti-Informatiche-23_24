
#include "./../network/network.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>

//#include "./../string/string.h"
#include "./../protocol/protocol.h"
#include "./../repo/repo.h"
#include "./../test.h"


void *kserver_thread_func(void *arg) {
    server *s = (server *)arg;
    listen_server(s);
    return NULL;
}

// -----------------------------------------
void kaccept_function(int sd) {}
void kinput_function(int sd, const message msg) {}
int kresponse_function(int sd, const message msg, message *rsp) {
    int err = 0;
    if(msg.msgtype == MSG_COMMAND){
        if (msg.cmdtype == CMD_LOGIN) {
            char* usr = malloc(strlen(msg.field)+1);
            char* pwd = malloc(strlen(msg.field)+1);

            sscanf(msg.field,"%s %s", usr, pwd);

            err = get_user("./tmp/cred.txt", usr, pwd);
            if (err != 0) {
                rsp->msgtype = MSG_ERROR;
                return 0;
            }
            return err;
        }
        if (msg.cmdtype == CMD_SIGNUP) {
            char* usr = malloc(strlen(msg.field)+1);
            char* pwd = malloc(strlen(msg.field)+1);

            sscanf(msg.field,"%s %s", usr, pwd);
            err = create_user("./tmp/cred.txt", usr, pwd);
            if(err != 0){
                rsp->msgtype = MSG_ERROR;
                return err;
            }
            return err;

        }
    }
  return err;
}
// -----------------------------------------


#define REPO_PATH "./tmp/creds.txt"

int TestLogin(){

    // Init test files
    system("mkdir -p ./tmp");
    system("touch ./tmp/cred.txt");
    
    //init server
    server *s = new_server(kaccept_function, kinput_function, kresponse_function);

    // start the server
    uint32_t port = 2500;
    int attempts = 0;
    while (bind_server(s, port) == -1) {
        port++;
        attempts++;
        ASSERT(attempts < 100);
    }

    // start the server in a separate thread
    pthread_t kserver_thread;
    int res = pthread_create(&kserver_thread, NULL, kserver_thread_func, s);
    ASSERT(res == 0);   

    // wait for the server to startup 
    sleep(1);

    client *c = new_client("127.0.0.1", port);
    
    //Test SIGUP
    message msg_test = {
        .msgtype = MSG_COMMAND, 
        .cmdtype = CMD_SIGNUP,
        .field = "hello"
    };
    msg_test.field = "cazzillo fotonico";
    message rsp = {0};
    
    res = request(c, msg_test, &rsp);
    //test registration
    ASSERT(res == 0);
    ASSERT(rsp.msgtype != MSG_ERROR);
    
    //Test LOGIN
    msg_test.msgtype = MSG_COMMAND;
    msg_test.cmdtype = CMD_LOGIN;
    msg_test.field = "cazzillo fotonico";
    rsp = (message){0};  

    res = request(c, msg_test, &rsp);

    ASSERT(res == 0);
    ASSERT(rsp.msgtype != MSG_ERROR);

    system("echo palle kek > ./tmp/cred.txt");

    msg_test.msgtype = MSG_COMMAND;
    msg_test.cmdtype = CMD_LOGIN;
    msg_test.field = "pale kek";
    
    rsp = (message){0};

    res = request(c, msg_test, &rsp);
    
    ASSERT(res == 0);
    ASSERT(rsp.msgtype == MSG_ERROR);
    
    system("rm ./tmp/cred.txt");
    
    stop_server(s);
    
    pthread_join(kserver_thread,NULL);

    delete_server(s);
    delete_client(c);
    return 0;
}
