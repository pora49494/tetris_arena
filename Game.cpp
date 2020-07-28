#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <vector>
#include <thread>

// #include "gameMessage.cpp"
#include "GameEnv.cpp"
#include "Board.cpp"
#include "Client.cpp"

#define MAX_MESSAGE_SIZE 256
// DEBUG
#include <iostream>

class Game
{
public:
    Game(sf::RenderWindow &window) : mWindow(window)
    {
        LoadPieces();
        mBoard = Board(mPieces);
        oBoard = Board(mPieces);
    }

    int pPosX, pPosY;
    int pID, pRotation;
    int oPosX, oPosY;
    int oID{-1}, oRotation;
    sf::RenderWindow &mWindow;

    void DrawScene();
    void DrawOpponentScene();

    void CreateNewPiece();
    void Play();

private:
    int pNextPosX, pNextPosY;
    int pNextID, pNextRotation;
    bool inOnlineBattle{false};

    unsigned char buf[MAX_MESSAGE_SIZE];
    unsigned char rbuf[MAX_MESSAGE_SIZE];

    bool gameOver{false}, updateBoard{false};
    Board mBoard;
    Board oBoard;
    vector<Pieces> mPieces;

    void LoadPieces();
    void InitGame();

    int GetXPosInPixels(int pPos, int boardPosition);
    int GetYPosInPixels(int pPos);

    int BuildMessage(bool updateBoard, bool gameOver);
    void DrawPiece(int pPosX, int pPosY, int pID, int pRotation, int boardPosition);
    void DrawBoard(bool isPlayer1);
    void StoreAndCheck(int pID, int pPosX, int pPosY, int pRotation);

    void Action(Client &c);
    void OpponentAction(Client &c);
};

void Game::LoadPieces()
{
    for (int i = 0; i < PIECE_TYPES_COUNT; i++)
    {
        mPieces.push_back(Pieces(i));
    }
}

void Game::InitGame()
{
    srand((unsigned int)time(NULL));
    pID = rand() % PIECE_TYPES_COUNT;
    pRotation = rand() % PIECES_ROTATION;
    pPosX = (BOARD_WIDTH / 2) + mPieces[pID].GetXInitialPosition(pRotation);
    pPosY = mPieces[pID].GetYInitialPosition(pRotation);

    //  Next piece
    pNextID = rand() % PIECE_TYPES_COUNT;
    pNextRotation = rand() % PIECES_ROTATION;
    pNextPosX = BOARD_WIDTH + 1;
    pNextPosY = 0;

    gameOver = false;
    mBoard.InitBoard();
    oBoard.InitBoard();
}

void Game::CreateNewPiece()
{
    pID = pNextID;
    pRotation = pNextRotation;
    pPosX = (BOARD_WIDTH / 2) + mPieces[pID].GetXInitialPosition(pRotation);
    pPosY = mPieces[pID].GetYInitialPosition(pRotation);

    pNextID = rand() % PIECE_TYPES_COUNT;
    pNextRotation = rand() % PIECES_ROTATION;
}

int Game::GetXPosInPixels(int pPos, int boardPosition)
{
    return boardPosition - BLOCK_SIZE * (BOARD_WIDTH / 2) + (pPos * BLOCK_SIZE);
}

int Game::GetYPosInPixels(int pPos)
{
    return SCREEN_HEIGHT - BLOCK_SIZE * BOARD_HEIGHT + (pPos * BLOCK_SIZE);
}

void Game::DrawPiece(int pPosX, int pPosY, int pID, int pRotation, int boardPosition)
{
    int mPixelsX = GetXPosInPixels(pPosX, boardPosition);
    int mPixelsY = GetYPosInPixels(pPosY);
    for (int i = 0; i < PIECE_BLOCKS; i++)
        for (int j = 0; j < PIECE_BLOCKS; j++)
            if (mPieces[pID].GetBlockType(i, j, pRotation) != 0)
            {
                sf::RectangleShape rectangle(sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE));
                rectangle.move(mPixelsX + i * BLOCK_SIZE, mPixelsY + j * BLOCK_SIZE);
                rectangle.setFillColor(mPieces[pID].GetPieceColor());
                mWindow.draw(rectangle);
            }
}

void Game::DrawBoard(bool isPlayer1)
{
    int boardPosition = (isPlayer1) ? BOARD_POSITION : BOARD_OPPONENT_POSITION;
    int mX1 = boardPosition - (BLOCK_SIZE * (BOARD_WIDTH / 2));
    int mX2 = boardPosition + (BLOCK_SIZE * (BOARD_WIDTH / 2));
    int offsetY = SCREEN_HEIGHT - BLOCK_SIZE * BOARD_HEIGHT;

    int mY = offsetY - BORDER_WIDTH + BORDER_WIDTH;

    sf::RectangleShape leftBorder(sf::Vector2f(BORDER_WIDTH, BLOCK_SIZE * BOARD_HEIGHT));
    leftBorder.move(mX1 - BORDER_WIDTH, mY);
    leftBorder.setFillColor(sf::Color::White);
    mWindow.draw(leftBorder);

    sf::RectangleShape rightBorder(sf::Vector2f(BORDER_WIDTH, BLOCK_SIZE * BOARD_HEIGHT));
    rightBorder.move(mX2, mY);
    rightBorder.setFillColor(sf::Color::White);
    mWindow.draw(rightBorder);

    sf::RectangleShape bottomBorder(sf::Vector2f(mX2 - mX1 + 2 * BORDER_WIDTH, BORDER_WIDTH));
    bottomBorder.move(mX1 - BORDER_WIDTH, SCREEN_HEIGHT);
    bottomBorder.setFillColor(sf::Color::White);
    mWindow.draw(bottomBorder);

    sf::RectangleShape warningLine(sf::Vector2f(mX2 - mX1 + 2 * BORDER_WIDTH, 2));
    warningLine.move(mX1 - BORDER_WIDTH, offsetY + BLOCK_SIZE);
    warningLine.setFillColor(sf::Color::Red);
    mWindow.draw(warningLine);

    Board &dBoard = (isPlayer1) ? mBoard : oBoard;
    for (int i = 0; i < BOARD_WIDTH; i++)
        for (int j = 0; j < BOARD_HEIGHT; j++)
            if (!dBoard.IsFreeBlock(i, j))
            {
                sf::RectangleShape block(sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE));
                block.move(mX1 + i * BLOCK_SIZE, mY + j * BLOCK_SIZE);
                block.setFillColor(sf::Color::White);
                mWindow.draw(block);
            }
}

void Game::StoreAndCheck(int pID, int pPosX, int pPosY, int pRotation)
{
    mBoard.StorePiece(pID, pPosX, pPosY, pRotation);
    mBoard.DeletePossibleLines();
    CreateNewPiece();
    updateBoard = true;
    gameOver = mBoard.IsGameOver();
}

void Game::DrawScene()
{
    DrawBoard(true);
    DrawPiece(pPosX, pPosY, pID, pRotation, BOARD_POSITION);
    DrawPiece(pNextPosX, pNextPosY, pNextID, pNextRotation, BOARD_POSITION);
}

void Game::DrawOpponentScene()
{
    DrawBoard(false);
    if (oID != -1)
        DrawPiece(oPosX, oPosY, oID, oRotation, BOARD_OPPONENT_POSITION);
}

int Game::BuildMessage(bool updateBoard, bool gameOver)
{
    struct GAME_MESSAGE *message = (struct GAME_MESSAGE *)&buf;
    message->rotation = pRotation;
    message->p_id = pID;
    message->loc_x = pPosX;
    message->loc_y = pPosY;
    message->update_board = updateBoard;
    message->game_over = gameOver;
    message->reset = 0;

    if (updateBoard)
    {
        unsigned char *board = &buf[sizeof(GAME_MESSAGE)];
        int shift = 0;
        int v = 0;
        for (int i = 0; i < BOARD_WIDTH; i++)
            for (int j = 0; j < BOARD_HEIGHT; j++)
            {
                v |= (!mBoard.IsFreeBlock(i, j)) << shift;
                shift++;
                if (shift == 8)
                {
                    *(board++) = v;
                    shift = 0;
                    v = 0;
                }
            }
        return 29;
    }
    return 4;
}

void Game::Action(Client &c)
{
    mWindow.clear();
    DrawScene();
    DrawOpponentScene();
    int message_size = BuildMessage(updateBoard, gameOver);
    c.Send(buf, message_size);
    updateBoard = false;
}

void Game::OpponentAction(Client &c)
{
    int n;
    while (!gameOver)
    {
        if ((n = c.Listen(rbuf)) <= 0)
        {
            std::cout << "disconnected" << std::endl;
            break;
        }
        struct GAME_MESSAGE *header = (struct GAME_MESSAGE *)&rbuf;

        oRotation = (int)header->rotation;
        oID = (int)header->p_id;
        oPosX = (int)header->loc_x;
        oPosY = (int)header->loc_y;
        bool updateBoard = (int)header->update_board;
        gameOver = (bool)header->game_over;

        if ((bool)header->reset)
        {
            cout << "reset" << endl;
            inOnlineBattle = true;
            InitGame();
        }

        cout << "oRotation " << oRotation << endl;
        cout << "oID " << oID << endl;
        cout << "oPosX " << oPosX << endl;
        cout << "oPosY " << oPosY << endl;
        cout << "updateBoard " << updateBoard << endl;
        cout << "gameOver " << gameOver << endl;

        if (updateBoard)
        {
            oBoard.UpdateFromMessage(&rbuf[sizeof(struct GAME_MESSAGE)]);
        }
    }
}

void Game::Play()
{
    InitGame();

    int message_size;
    Client c;
    c.Initialize();

    std::thread t([this, &c] { this->OpponentAction(c); });
    t.detach();

    Action(c);
    sf::Clock clock, clock_render;
    sf::Time elapsed;
    sf::Time elapsed_render;

    while (mWindow.isOpen())
    {
        elapsed = clock.getElapsedTime();
        elapsed_render = clock_render.getElapsedTime();
        if (!gameOver && elapsed_render.asMilliseconds() > 300)
        {
            mWindow.clear();
            DrawScene();
            DrawOpponentScene();
            clock_render.restart();
        }
        if (!gameOver && elapsed.asSeconds() > 1)
        {
            if (mBoard.IsPossibleMovement(pPosX, pPosY + 1, pID, pRotation))
                pPosY++;
            else
            {
                StoreAndCheck(pID, pPosX, pPosY, pRotation);
            }
            Action(c);
            clock.restart();
        }
        sf::Event event;
        while (mWindow.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::KeyPressed:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                {
                    if (mBoard.IsPossibleMovement(pPosX - 1, pPosY, pID, pRotation))
                    {
                        pPosX--;
                        Action(c);
                    }
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                {
                    if (mBoard.IsPossibleMovement(pPosX, pPosY + 1, pID, pRotation))
                    {
                        pPosY++;
                        Action(c);
                    }
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                {
                    while (mBoard.IsPossibleMovement(pPosX, pPosY + 1, pID, pRotation))
                        pPosY++;
                    StoreAndCheck(pID, pPosX, pPosY, pRotation);
                    clock.restart();
                    Action(c);
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                {
                    if (mBoard.IsPossibleMovement(pPosX + 1, pPosY, pID, pRotation))
                    {
                        pPosX++;
                        Action(c);
                    }
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
                {
                    pRotation = (pRotation + 1) % PIECES_ROTATION;
                    Action(c);
                }
                break;
            case sf::Event::Closed:
                mWindow.close();
                break;
            default:
                break;
            }
        }
        if (gameOver && inOnlineBattle)
        {
            break;
        }
        else if (gameOver && !inOnlineBattle)
        {
            InitGame();
        }
        mWindow.display();
    }
}
