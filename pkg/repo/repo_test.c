#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>

#include "./../network/network.h"
#include "./../string/string.h"
#include "./../protocol/protocol.h"
#include "./../repo/repo.h"
#include "./../test.h"


void *testLogin_server_thread_func(void *arg) {
    server *s = (server *)arg;
    listen_server(s);
    return NULL;
}


// -----------------------------------------
void testLogin_accept_function(int sd) {}
void testTick_accept_function() {}
void testLogin_input_function(int sd, const char* inputText) {}
int testLogin_response_function(int sd, const message msg, message *rsp) {
    int err = 0;
    if(msg.msgtype == MSG_COMMAND){
        if (msg.cmdtype == CMD_LOGIN) {
            char* usr = malloc(strlen(msg.field)+1);
            char* pwd = malloc(strlen(msg.field)+1);
            int port = 0;
            err = get_usr_pwd_port(msg.field, usr, pwd, &port);
            if(err != 0){
                rsp->msgtype = MSG_ERROR;
                return err;             
            }

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
            int port = 0;
      
            err = get_usr_pwd_port(msg.field, usr, pwd, &port);
            if(err != 0){
                rsp->msgtype = MSG_ERROR;
                return err;
            }

            err = create_user("./tmp/cred.txt", usr, pwd);
            if(err != 0){
                rsp->msgtype = MSG_ERROR;
                return err;
            }
            return err;

        }
    }
    rsp->msgtype = MSG_BAD_REQUEST;
    return err;
}
void testLogin_disconnect_function(int sd) {}
// -----------------------------------------


#define REPO_PATH "./tmp/creds.txt"

int TestLogin(){

    // Init test files
    system("mkdir -p ./tmp");
    system("touch ./tmp/cred.txt");
    
    //init server
    server *s = new_server(testLogin_accept_function, testLogin_input_function, testLogin_response_function, testLogin_disconnect_function, testTick_accept_function);

    // start the server
    uint32_t port = 2500;
    int attempts = 0;
    while (bind_server(s, port) == -1) {
        port++;
        attempts++;
        ASSERT(attempts < 100);
    }

    // start the server in a separate thread
    pthread_t testLogin_server_thread;
    int res = pthread_create(&testLogin_server_thread, NULL, testLogin_server_thread_func, s);
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

    system("echo uu kek > ./tmp/cred.txt");

    msg_test.msgtype = MSG_COMMAND;
    msg_test.cmdtype = CMD_LOGIN;
    msg_test.field = "uwu kek";
    
    rsp = (message){0};

    res = request(c, msg_test, &rsp);
    
    ASSERT(res == 0);
    ASSERT(rsp.msgtype == MSG_ERROR);
    
    system("rm ./tmp/cred.txt");
    
    stop_server(s);
    
    pthread_join(testLogin_server_thread,NULL);

    delete_server(s);
    delete_client(c);
    return 0;
}
