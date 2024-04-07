
struct gamer{
    int sd;
    char* username;
    int port; // used only for in-game chat 
    int room_id;
    struct gamer* next_gamer;
};

typedef struct gamer gamer;

gamer* gamer_list;

int login(const int sd, const char* usr, const char* pwd);
int signup(const char* usr, const char* pwd);
