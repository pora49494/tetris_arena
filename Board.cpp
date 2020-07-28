#include "GameEnv.cpp"
#include "Pieces.cpp"

class Board
{
public:
    Board(){};
    Board(vector<Pieces> &pPieces) : mPieces(pPieces)
    {
        InitBoard();
    };

    int GetXPosInPixels(int pPos);
    int GetYPosInPixels(int pPos);

    bool IsFreeBlock(int pX, int pY);
    bool IsPossibleMovement(int pX, int pY, int pID, int pRotation);

    void StorePiece(int pX, int pY, int pPiece, int pRotation);
    void DeletePossibleLines();

    void UpdateFromMessage(unsigned char *buf);

    void InitBoard();
    bool IsGameOver();

private:
    enum
    {
        POS_FREE,
        POS_FILLED
    };
    int mScreenHeight;

    int mBoard[BOARD_WIDTH][BOARD_HEIGHT];
    vector<Pieces> mPieces;

    void DeleteLines(vector<int> &pYList);
};

void Board::InitBoard()
{
    for (int i = 0; i < BOARD_WIDTH; i++)
        for (int j = 0; j < BOARD_HEIGHT; j++)
            mBoard[i][j] = POS_FREE;
}

void Board::StorePiece(int pID, int pX, int pY, int pRotation)
{
    for (int i = 0; i < PIECE_BLOCKS; i++)
        for (int j = 0; j < PIECE_BLOCKS; j++)
            if (mPieces[pID].GetBlockType(i, j, pRotation) != 0)
                mBoard[i + pX][j + pY] = POS_FILLED;
}

bool Board::IsGameOver()
{
    for (int i = 0; i < BOARD_WIDTH; i++)
        if (mBoard[i][0] == POS_FILLED)
            return true;
    return false;
}

void Board::DeleteLines(vector<int> &pYList)
{
    int k = BOARD_HEIGHT - 1;
    int yi = 0, n = pYList.size();
    for (int j = BOARD_HEIGHT - 1; j >= 0; j--)
    {
        if (yi < n && pYList[yi] == j)
        {
            yi++;
        }
        else
        {
            if (j != k)
                for (int i = 0; i < BOARD_WIDTH; i++)
                    mBoard[i][k] = mBoard[i][j];
            k--;
        }
    }
    while (k >= 0)
    {
        for (int i = 0; i < BOARD_WIDTH; i++)
            mBoard[i][k] = POS_FREE;
        k--;
    }
}

void Board::UpdateFromMessage(unsigned char *board)
{
    int shift = 0;
    for (int i = 0; i < BOARD_WIDTH; i++)
    {
        for (int j = 0; j < BOARD_HEIGHT; j++)
        {
            mBoard[i][j] = (int)((*board >> shift) & 1);
            if (++shift == 8)
            {
                shift = 0;
                board++;
            }
        }
    }
}

void Board::DeletePossibleLines()
{
    vector<int> pYList;
    for (int j = BOARD_HEIGHT - 1; j >= 0; j--)
    {
        int i = 0;
        while (i < BOARD_WIDTH)
        {
            if (mBoard[i][j] != POS_FILLED)
                break;
            i++;
        }
        if (i >= BOARD_WIDTH)
            pYList.push_back(j);
    }
    DeleteLines(pYList);
}

bool Board::IsFreeBlock(int pX, int pY)
{
    return (mBoard[pX][pY] == POS_FREE) ? true : false;
}

bool Board::IsPossibleMovement(int pX, int pY, int pID, int pRotation)
{
    for (int i = 0; i < PIECE_BLOCKS; i++)
        for (int j = 0; j < PIECE_BLOCKS; j++)
        {
            if (i + pX < 0 || i + pX > BOARD_WIDTH - 1 || j + pY > BOARD_HEIGHT - 1)
                if (mPieces[pID].GetBlockType(i, j, pRotation) != 0)
                    return false;

            if (j + pY >= 0)
                if (mPieces[pID].GetBlockType(i, j, pRotation) != 0 && !IsFreeBlock(i + pX, j + pY))
                    return false;
        }

    return true;
}
