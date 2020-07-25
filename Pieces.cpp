#include <iostream>
#include <vector>
#include <fstream>

#define PIECES_ROTATION 4
#define BLOCK_SIZE 5
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
    vector<vector<vector<int>>> myBlock;
    vector<struct Loc> initLoc;
    int pieceType;

private:
    void LoadPieceInfo();
    string GetPieceName();

public:
    int GetBlockType(x, y);
    Pieces(int _pieceType);
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
        char fileName = (char)'0' + i;
        ifstream pieceFile(pieceFolder + fileName);
        if (pieceFile.is_open())
        {
            int j{0};
            while (j < BLOCK_SIZE && getline(pieceFile, line))
            {
                for (int k = 0; k < BLOCK_SIZE; k++)
                    myBlock[i][j][k] = (int)line[k] - '0';

                j++;
            }
            pieceFile >> x >> y;
            initLoc[i] = Loc(x, y);
            pieceFile.close();
        }
        else
        {
            cout << pieceFolder + fileName;
        }
    }
}

int Piece::GetBlockType(int x, int y, int rotation)
{
    return myBlock[rotation][x][y]
}
