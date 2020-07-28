#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>

#include "GameEnv.cpp"

using namespace std;

#define PORT 22618
#define MAXLINE 1024

class Client
{
private:
    int sockfd{0};
    struct sockaddr_in sa;
    unsigned char serverIP[20] = "127.0.0.1";

public:
    Client(){};
    void Initialize();
    void Send(unsigned char *buf, int message_size);
    int Listen(unsigned char *rbuf);
};

void Client::Initialize()
{
    if (inet_pton(AF_INET, (char *)serverIP, &sa.sin_addr) <= 0)
    {
        perror("inet_pton()");
        exit(EXIT_FAILURE);
    }
    sa.sin_family = AF_INET;
    sa.sin_port = htons(PORT);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&sa, sizeof(sa)) < 0)
    {
        perror("connect()");
        exit(EXIT_FAILURE);
    }
}

void Client::Send(unsigned char *buf, int message_size)
{
    sendto(sockfd, buf, message_size,
           MSG_CONFIRM, (const struct sockaddr *)&sa,
           sizeof(sa));
}

#include "gameMessage.cpp"

int Client::Listen(unsigned char *rbuf)
{
    int n;
    if ((n = read(sockfd, rbuf, 29)) <= 0)
    {
        std::cout << "Disconnected" << std::endl;
        return n;
    }
    return n;
}
