#include <pthread.h>
#include <SFML/Graphics.hpp>
#include "Game.cpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define PLAYERS_NUMBER 2

int main()
{
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT + 40), "TETRIS ARENA");
    Game gamePlay(window);
    gamePlay.Play();
}
