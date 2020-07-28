#include <iostream>
#include <vector>
#include <fstream>
#include <SFML/Graphics.hpp>

#include "GameEnv.cpp"
using namespace std;

struct Loc
{
    int x;
    int y;
    Loc(){};
    Loc(int _x, int _y) : x(_x), y(_y) {}
};

class Pieces
{
private:
    vector<vector<vector<int>>> myBlock;
    vector<struct Loc> initLoc;
    int pieceType;

    void LoadPieceInfo();
    string GetPieceName();

public:
    Pieces(int _pieceType);

    int GetBlockType(int x, int y, int rotation);
    int GetXInitialPosition(int rotation);
    int GetYInitialPosition(int rotation);
    sf::Color GetPieceColor();
};

Pieces::Pieces(int _pieceType)
{
    pieceType = _pieceType;
    myBlock = vector<vector<vector<int>>>(4, vector<vector<int>>(5, vector<int>(5, 0)));
    LoadPieceInfo();
}

string Pieces::GetPieceName()
{
    switch (pieceType)
    {
    case 0:
        return "I";
    case 1:
        return "L";
    case 2:
        return "L-mirrored";
    case 3:
        return "N";
    case 4:
        return "N-mirrored";
    case 5:
        return "Square";
    case 6:
        return "T";
    default:
        return "I";
    }
}

void Pieces::LoadPieceInfo()
{
    string pieceFolder = "pieces/" + Pieces::GetPieceName() + "/";
    string line;
    int x, y;
    for (int i = 0; i < PIECES_ROTATION; i++)
    {
        char fileName = (char) '0'+i;
        ifstream pieceFile(pieceFolder + fileName);
        if (pieceFile.is_open())
        {
            int j{0};
            while (j < PIECE_BLOCKS && getline(pieceFile, line))
            {
                for (int k = 0; k < PIECE_BLOCKS; k++){
                    myBlock[i][j][k] = (int)line[k]-'0';
                }

                j++;
            }
            pieceFile >> x >> y;
            initLoc.push_back(Loc(x, y));
            pieceFile.close();
        }
        else
        {
            cout << pieceFolder + fileName + " Not Found";
        }
    }
}

sf::Color Pieces::GetPieceColor()
{
    switch (pieceType)
    {
    case 0:
        return sf::Color::Red;
    case 1:
        return sf::Color::Blue;
    case 2:
        return sf::Color::Green;
    case 3:
        return sf::Color::Yellow;
    case 4:
        return sf::Color::Magenta;
    case 5:
        return sf::Color::Cyan;
    case 6:
        return sf::Color(123, 23, 10);
    default:
        return sf::Color::Red;
    }
}

int Pieces::GetBlockType(int x, int y, int rotation)
{
    return myBlock[rotation][x][y];
}

int Pieces::GetXInitialPosition(int rotation)
{
    return initLoc[rotation].x;
}

int Pieces::GetYInitialPosition(int rotation)
{
    return initLoc[rotation].y;
}
