struct GAME_MESSAGE
{
    // piece info (2+3+8+8 = 21)
    unsigned char rotation : 2;
    unsigned char p_id : 3;
    char loc_x;
    char loc_y;

    // game state (1+1+1=3)
    unsigned char update_board : 1;
    unsigned char game_over : 1;
    unsigned char reset : 1;
};
