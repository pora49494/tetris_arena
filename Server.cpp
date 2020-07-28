#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <condition_variable>
#include <thread>
#include <chrono>
#include <vector>

// DEBUG
#include <iostream>
using namespace std;
// DEBUG

#include "gameMessage.cpp"

#define PORT 22618
#define MAXLINE 128
#define MAXPLAYERS 2
#define BACKLOG 5
#define MAXGAMES 100

int players_number{0};
int connfd[MAXGAMES][MAXPLAYERS];

void init_game(int p1, int p2) {
    unsigned char buf[MAXLINE];
    struct GAME_MESSAGE *message = (struct GAME_MESSAGE *)&buf;
    message->rotation = 0;
    message->p_id = 0;
    message->loc_x = 0;
    message->loc_y = 0;
    message->update_board = 0;
    message->game_over = 0;
    message->reset = 1 ;
    send(p1, buf, 4, MSG_DONTWAIT);
    send(p2, buf, 4, MSG_DONTWAIT);
    return;
}

void game_start(int player, int opponent )
{
    unsigned char buf[MAXLINE];
    bool isGameOver{false};
    struct GAME_MESSAGE *header;

    cout << "Initate player " << player << " against player " << opponent << endl;
    int n;

    while (!isGameOver)
    {
        if ( ( n = read(player, buf, sizeof(buf)) ) <= 0  ) {
            cout << "Disconnected" << endl ;
            break;
        }

        header = (struct GAME_MESSAGE *)&buf;
        cout << "recieved from " << player << endl ;
        cout << "rotation " << (int)header->rotation  << endl;
        cout << "pid " << (int)header->p_id << endl;
        cout << "locx " << (int)header->loc_x << endl;
        cout << "loxy " << (int)header->loc_y << endl;
        cout << "update " << (int)header->update_board << endl;
        cout << "over " << (int)header->game_over << endl;

        int shift = 0;
        int v = 0;
        unsigned char* board = &buf[sizeof(struct GAME_MESSAGE)] ;
        if (header->update_board)
        {
            for (int i=0;i<10;i++) {
                for (int j = 0; j < 20; j++)
                {
                    cout << (int)((*board >> shift) & 1);
                    shift++;
                    if (shift == 8)
                    {
                        board++;
                        shift = 0;
                    }
                }
                cout << endl;
            }
                
        }
        send(opponent, buf, 29, MSG_DONTWAIT);
    }
    return;
}

int main()
{
    int sd;
    int opt = 1;
    int games_counter = 0;

    char buf[MAXLINE];
    struct sockaddr_in sa;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd <= 0)
    {
        perror("socket()");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        perror("setsockopt()");
        exit(EXIT_FAILURE);
    }

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(PORT);

    if (bind(sd, (struct sockaddr *)&sa, sizeof(sa)) < 0)
    {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    if (listen(sd, BACKLOG) < 0)
    {
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    int games_count = 0;
    while (true)
    {
        while (players_number < MAXPLAYERS)
        {
            cout << "waiting..." << endl;
            
            int player_id = players_number;
            
            if ((connfd[games_count][player_id] = accept(sd, NULL, NULL)) < 0)
            {
                perror("accept()");
                exit(EXIT_FAILURE);
            }

            cout << "Player " << player_id << " joined" << endl;
            players_number++;
        }
        cout << "TETRIS ARENA START!" << endl;
        init_game(connfd[games_count][0], connfd[games_count][1]);
        
        std::thread t1(game_start, connfd[games_count][1], connfd[games_count][0]);
        t1.detach();
        std::thread t2(game_start, connfd[games_count][0], connfd[games_count][1]);
        t2.detach();

        games_count = (games_count + 1) % MAXGAMES;
        players_number = 0;
    }

    return 0;
}