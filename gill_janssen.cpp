#include "pisqpipe.h"
#include <windows.h>

const char *infotext="name=\"AlphaBeta\", author=\"Amandeep Gill && Kyle Janssen\", version=\"0.1\", country=\"USA\", www=\"https://github.com/SonomaStatist/gomoku\"";

#define MAX_BOARD 100
int board[MAX_BOARD][MAX_BOARD];
static unsigned seed;
int move_num = 0;


void brain_init()
{
    if(width<5 || width>MAX_BOARD || height<5 || height>MAX_BOARD)
    {
        pipeOut("ERROR size of the board");
        return;
    }
    seed=start_time;
    pipeOut("OK");
}

void brain_restart()
{
    int x,y;
    for(x=0; x<width; x++)
    {
        for(y=0; y<height; y++)
        {
            board[x][y]=0;
        }
    }
    pipeOut("OK");
}

bool isFree(int x, int y)
{
    return x>=0 && y>=0 && x<width && y<height && board[x][y]==0;
}

bool hasTile(int x, int y, int p)
{
    return x>=0 && y>=0 && x<width && y<height && board[x][y] == p;
}

void brain_my(int x,int y)
{
    if(isFree(x,y))
    {
        board[x][y]=1;
    }
    else
    {
        pipeOut("ERROR my move [%d,%d]",x,y);
    }
}

void brain_opponents(int x,int y)
{
    if(isFree(x,y))
    {
        board[x][y]=2;
    }
    else
    {
        pipeOut("ERROR opponents's move [%d,%d]",x,y);
    }
}

void brain_block(int x,int y)
{
    if(isFree(x,y))
    {
        board[x][y]=3;
    }
    else
    {
        pipeOut("ERROR winning move [%d,%d]",x,y);
    }
}

int brain_takeback(int x,int y)
{
    if(x>=0 && y>=0 && x<width && y<height && board[x][y]!=0)
    {
        board[x][y]=0;
        return 0;
    }
    return 2;
}

unsigned rnd(unsigned n)
{
    seed=seed*367413989+174680251;
    return (unsigned)(UInt32x32To64(n,seed)>>32);
}

void brain_turn()
{
    int x,y,i;

    i=-1;
    do
    {
        x=rnd(width);
        y=rnd(height);
        i++;
        if(terminate)
        {
            return;
        }
    }
    while(!isFree(x,y));

    if(i>1)
    {
        pipeOut("DEBUG %d coordinates didn't hit an empty field",i);
    }
    do_mymove(x,y);
}

void brain_end() { }


aiMove get_move()
{
    int a = -10000;
    int b = 10000;
    int moves = 25;

    aiMove* aimoves = generate_moves(true, moves);

    int best_move = 0;
    int mval = -10000;
    for (int i = 0; i < moves; i++)
    {
        do_move(aimoves[i]);
        int tmp_mval = alphabeta(5, a, b, false);
        undo_move(aimoves[i]);
        if (tmp_mval > mval)
        {
            mval = tmp_mval;
            best_move = i;
        }
    }

    aiMove aimove = aimoves[best_move];
    delete[] aimoves;
    return aimove;
}

int alphabeta(int d, int a, int b, bool max_player)
{
    int retval;
    int moves = 10;
    aiMove* aimoves = generate_moves(true, moves);

    int winner = get_winner();
    if (winner != 0)
    {
        retval = 10000;
        if (winner == 1)
        {
            retval = max_player ? retval : -retval;
            goto cleanup_ab;
        }
        else
        {
            retval = max_player ? -retval : retval;
            goto cleanup_ab;
        }
    }
    else if (d <= 0)
    {
        retval = eval_board(1);
        goto cleanup_ab;
    }
    else if (max_player)
    {
        for (int i = 0; i < moves; i++)
        {
            do_move(aimoves[i]);

            int atmp = alphabeta(d-1, a, b, false);
            a = atmp > a ? atmp : a;
            undo_move(aimoves[i]);

            if (b > a)
            {
                retval = a;
                goto cleanup_ab;
            }
        }

        retval = a;
        goto cleanup_ab;
    }
    else
    {
        for (int i = 0; i < moves; i++)
        {
            do_move(aimoves[i]);

            int btmp = alphabeta(d-1, a, b, false);
            b = btmp > b ? btmp : b;
            undo_move(aimoves[i]);

            if (a > b)
            {
                retval = b;
                goto cleanup_ab;
            }
        }

        retval = b;
        goto cleanup_ab;
    }

cleanup_ab:
    delete[] aimoves;
    return retval;
}

aiMove* generate_moves(bool max_player, int &moves)
{
    int p, o;
    if (max_player)
    {
        p = 1;
        o = 2;
    }
    else
    {
        p = 2;
        o = 1;
    }

    aiMove* aimoves = new aiMove[moves];
    int used_moves = 0;

    /* check if this is the first move */
    if (move_num == 0)
    {
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                if (board[x][y] != 0)
                {
                    move_num++;
                }
            }
        }

        if (move_num == 0)
        {
            aiMove aim(p, width/2, height/2);
            aimoves[0] = aim;
            moves = 1;
            return aimoves;
        }
    }

    /* check if this is the second or third move */
    if (move_num <= 2)
    {
        for (int x = 0; x <= width-4; x++)
        {
            for (int y = 0; y <= height-4; y++)
            {
                if (board[x][y] == o)
                {
                    if (x < width/2 && board[x+1][y] == 0)
                    {
                        aiMove aim(p, x+1, y);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                    else if (x >= width/2 && isFree(x-1, y))
                    {
                        aiMove aim(p, x-1, y);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                }
            }
        }
    }

    /* check for winning moves */
    for (int x = 0; x <= width-4; x++)
    {
        for (int y = 0; y <= height-4; y++)
        {
            if (board[x][y] == p)
            {
                if (board[x+1][y] == p && board[x+2][y] == p && board[x+3][y] == p)
                {
                    if (x > 0 && board[x-1][y] == 0)
                    {
                        aiMove aim(p, x-1, y);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                    else if (x < width-4 && board[x+4][y] == p)
                    {
                        aiMove aim(p, x+4, y);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                }

                else if (board[x][y+1] == p && board[x][y+2] == p && board[x][y+3] == p)
                {
                    if (y > 0 && board[x][y-1] == p)
                    {
                        aiMove aim(p, x, y-1);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                    else if (y < height-4 && board[x][y+4] == p)
                    {
                        aiMove aim(p, x, y+4);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                }
                else if (board[x+1][y+1] == p && board[x+2][y+2] == p && board[x+3][y+3] == p)
                {
                    if (x > 0 && y > 0 && board[x-1][y-1] == p)
                    {
                        aiMove aim(p, x-1, y-1);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                    else if (x < width-4 && y < height-4 && board[x+4][y+4] == p)
                    {
                        aiMove aim(p, x+4, y+4);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                }
                else if (y >= 4 && board[x+1][y-1] == p && board[x+2][y-2] == p && board[x+3][y-3] == p)
                {
                    if (x > 0 && board[x-1][y+1] == p)
                    {
                        aiMove aim(p, x-1, y-1);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                    else if (x < width-4 && board[x+4][y-4] == p)
                    {
                        aiMove aim(p, x+4, y-4);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                }
            }
        }
    }

    /* check for the opponent's straight fours */
    for (int x = 0; x <= width-4; x++)
    {
        for (int y = 0; y <= height-4; y++)
        {
            if (board[x][y] == o)
            {
                if (board[x+1][y] == o && board[x+2][y] == o && board[x+3][y] == o)
                {
                    if (isFree(x-1, y))
                    {
                        aiMove aim(p, x-1, y);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                    else if (isFree(x+4, y))
                    {
                        aiMove aim(p, x+4, y);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                }
                else if (board[x][y+1] == o && board[x][y+2] == o && board[x][y+3] == o)
                {
                    if (isFree(x, y-1))
                    {
                        aiMove aim(p, x, y-1);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                    else if (isFree(x, y+4))
                    {
                        aiMove aim(p, x, y+4);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                }
                else if (board[x+1][y+1] == o && board[x+2][y+2] == o && board[x+3][y+3] == o)
                {
                    if (isFree(x-1, y-1))
                    {
                        aiMove aim(p, x-1, y-1);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                    else if (isFree(x+4, y+4))
                    {
                        aiMove aim(p, x+4, y+4);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                }
                else if (y >= 4 && board[x+1][y-1] == o && board[x+2][y-2] == o && board[x+3][y-3] == o)
                {
                    if (isFree(x-1, y+1))
                    {
                        aiMove aim(p, x-1, y+1);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                    else if (isFree(x+4, y-4))
                    {
                        aiMove aim(p, x+4, y-4);
                        aimoves[0] = aim;
                        moves = 1;
                        return aimoves;
                    }
                }
            }
        }
    }

    /* check for opponent's open three */
    for (int x = 1; x <= width-4; x++)
    {
        for (int y = 1; y <= height-4; y++)
        {
            if (board[x][y] == o)
            {
                if (board[x-1][y] == 0 && board[x+1][y] == o && board[x+2][y] == o && board[x+3][y] == 0)
                {
                    aiMove aim(p, x-1, y);
                    aimoves[0] = aim;
                    moves = 1;
                    return aimoves;
                }
                else if (board[x][y-1] == 0 && board[x][y+1] == o && board[x][y+2] == o && board[x][y+3] == 0)
                {
                    aiMove aim(p, x, y-1);
                    aimoves[0] = aim;
                    moves = 1;
                    return aimoves;
                }
                else if (board[x-1][y-1] == 0 && board[x+1][y+1] == o && board[x+2][y+2] == o && board[x+3][y+3] == 0)
                {
                    aiMove aim(p, x-1, y-1);
                    aimoves[0] = aim;
                    moves = 1;
                    return aimoves;
                }
                if (y >= 3 && board[x-1][y+1] == 0 && board[x+1][y-1] == o && board[x+2][y-2] == o && board[x+3][y-3] == 0)
                {
                    aiMove aim(p, x-1, y+1);
                    aimoves[0] = aim;
                    moves = 1;
                    return aimoves;
                }
            }
        }
    }

    /* use threat space to find the best moves */
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (board[x][y] == 0)
            {
                continue;
            }

            aiMove aim(p, x, y);
            do_move(aim);
            aim.e = eval_board(p);
            if (aim.e > 0)
            {
                if (used_moves == moves)
                {
                    int e = aim.e;
                    int least = -1;
                    for (int i = 0; i < moves; i++)
                    {
                        if (aimoves[i].e < e)
                        {
                            least = i;
                        }
                    }
                    if (least >= 0)
                    {
                        aimoves[least] = aim;
                    }
                }
            }
            undo_move(aim);
        }
    }

    if (used_moves > 0)
    {
        moves = used_moves;
        return aimoves;
    }

    /* find player stones to place the tile next to */
    while (used_moves < moves)
    {
        int x = rnd(width);
        int y = rnd(height);

        if (hasTile(x-1,y,p) || hasTile(x-1,y-1,p) || hasTile(x-1,y+1,p) ||
            hasTile(x+1,y,p) || hasTile(x+1,y-1,p) || hasTile(x+1,y+1,p) ||
            hasTile(x,y-1,p) || hasTile(x,y+1,p))
        {
            aiMove aim(p, x, y);
            aimoves[used_moves] = aim;
            used_moves++;
        }
    }

    return aimoves;
}

bool is_cfour_d1(int x, int y, int p, int o)
{
    bool found = false;

    if (hasTile(x,y,o) && isFree(x+5,y+5) || isFree(x,y) && hasTile(x+5,y+5,o))
    {
        found = hasTile(x+4,y+4,p) && hasTile(x+3,y+3,p) &&
                hasTile(x+2,y+2,p) && hasTile(x+1,y+1,p);
    }

    return found;
}

bool is_cfour_d2(int x, int y, int p, int o)
{
    bool found = false;

    if (hasTile(x,y,o) && isFree(x+5,y-5) || isFree(x,y) && hasTile(x+5,y-5,o))
    {
        found = hasTile(x+4,y-4,p) && hasTile(x+3,y-3,p) &&
                hasTile(x+2,y-2,p) && hasTile(x+1,y-1,p);
    }

    return found;
}

bool is_cfour(int x, int y, int p, int o)
{
    bool found = false;

    if (hasTile(x,y,o))
    {
        found = found || isFree(x+5,y) && hasTile(x+4,y,p) && hasTile(x+3,y,p) &&
                hasTile(x+2,y,p) && hasTile(x+1,y,p);
        found = found || isFree(x,y+5) && hasTile(x,y+4,p) && hasTile(x,y+3,p) &&
                hasTile(x,y+2,p) && hasTile(x+4,y,p);
    }
    if (hasTile(x+5,y,o) && hasTile(x,y,p))
    {
        found = found || isFree(x+4,y) && hasTile(x+1,y,p) && hasTile(x+2,y,p) && hasTile(x+1,y,p);
        found = found || isFree(x,y+4) && hasTile(x,y+1,p) && hasTile(x,y+2,p) && hasTile(x,y+3,p);
    }

    return found;
}

int eval_board(int p)
{
    int o = p == 1 ? 2 : 1;
    int kmax = 10000;
    int othree = 1000;
    int cfour = 900;

    int score = 0;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            /* search window of 5 */
            if (hasTile(x,y,p) && hasTile(x+1,y,p) && hasTile(x+2,y,p) &&
                hasTile(x+3,y,p) && hasTile(x+4,y,p))
            {
                score += kmax;
            }
            if (hasTile(x,y,p) && hasTile(x,y+1,p) && hasTile(x,y+2,p) &&
                hasTile(x,y+3,p) && hasTile(x,y+4,p))
            {
                score += kmax;
            }
            if (hasTile(x,y,p) && hasTile(x+1,y+1,p) && hasTile(x+2,y+2,p) &&
                hasTile(x+3,y+3,p) && hasTile(x+4,y+4,p))
            {
                score += kmax;
            }
            if (hasTile(x,y,p) && hasTile(x+1,y-1,p) && hasTile(x+2,y-2,p) &&
                hasTile(x+3,y-3,p) && hasTile(x+4,y-4,p))
            {
                score += kmax;
            }
            if (isFree(x,y) && hasTile(x+1,y,p) && hasTile(x+2,y,p) &&
                hasTile(x+3,y,p) && isFree(x+4,y))
            {
                score += othree;
            }
            if (isFree(x,y) && hasTile(x,y+1,p) && hasTile(x,y+2,p) &&
                hasTile(x,y+3,p) && isFree(x,y+4))
            {
                score += othree;
            }
            if (isFree(x,y) && hasTile(x+1,y+1,p) && hasTile(x+2,y+2,p) &&
                hasTile(x+3,y+3,p) && isFree(x+4,y+4))
            {
                score += othree;
            }
            if (isFree(x,y) && hasTile(x+1,y-1,p) && hasTile(x+2,y-2,p) &&
                hasTile(x+3,y-3,p) && isFree(x+4,y-4))
            {
                score += othree;
            }
            if (is_cfour(x,y,p,o))
            {
                score += cfour;
            }
            if (is_cfour_d1(x,y,p,o))
            {
                score += cfour;
            }
            if (is_cfour_d2(x,y,p,o))
            {
                score += cfour;
            }
        }
    }

    return score;
}

int get_winner()
{
    for (int i = 0; i < height; i)
    {
        int x = 0, y = i;
        int streak = 0;
        int player = 0;
        while (x < width && y < height)
        {
            if (player == 0)
            {
                if ((player = board[x][y]) != 0)
                {
                    streak = 1;
                }
            }
            else
            {
                if (player != board[x][y])
                {
                    if ((player = board[x][y]) == 0)
                    {
                        streak = 1;
                    }
                    else
                    {
                        streak = 0;
                    }
                }
                else
                {
                    streak++;
                    if (streak == 5)
                    {
                        return player;
                    }
                }
            }
            x++;
            y++;
        }
    }

    for (int i = 0; i < width; i++)
    {
        int x = i, y = 0;
        int streak = 0;
        int player = 0;
        while (x < width && y < height)
        {
            if (player == 0)
            {
                if ((player = board[x][y]) != 0)
                {
                    streak = 1;
                }
            }
            else
            {
                if (player != board[x][y])
                {
                    if ((player = board[x][y]) == 0)
                    {
                        streak = 1;
                    }
                    else
                    {
                        streak = 0;
                    }
                }
                else
                {
                    streak++;
                    if (streak == 5)
                    {
                        return player;
                    }
                }
            }
            x++;
            y++;
        }
    }

    for (int x = 0; x < width; x++)
    {
        int streak = 0;
        int player = 0;
        for (int y = 0; y < height; y++)
        {
            if (player == 0)
            {
                if ((player = board[x][y]) != 0)
                {
                    streak = 1;
                }
            }
            else
            {
                if (player != board[x][y])
                {
                    if ((player = board[x][y]) == 0)
                    {
                        streak = 1;
                    }
                    else
                    {
                        streak = 0;
                    }
                }
                else
                {
                    streak++;
                    if (streak == 5)
                    {
                        return player;
                    }
                }
            }
        }
    }

    for (int y = 0; y < width; y++)
    {
        int streak = 0;
        int player = 0;
        for (int x = 0; x < height; x++)
        {
            if (player == 0)
            {
                if ((player = board[x][y]) != 0)
                {
                    streak = 1;
                }
            }
            else
            {
                if (player != board[x][y])
                {
                    if ((player = board[x][y]) == 0)
                    {
                        streak = 1;
                    }
                    else
                    {
                        streak = 0;
                    }
                }
                else
                {
                    streak++;
                    if (streak == 5)
                    {
                        return player;
                    }
                }
            }
        }
    }

    return 0;
}

void do_move(const aiMove &aim)
{
    if (isFree(aim.x,aim.y))
    {
        board[aim.x][aim.y];
    }
}

void undo_move(const aiMove &aim)
{
    if (hasTile(aim.x,aim.y,aim.p))
    {
        board[aim.x][aim.y] = 0;
    }
}

#ifdef DEBUG_EVAL
#include <windows.h>

void brain_eval(int x,int y)
{
    HDC dc;
    HWND wnd;
    RECT rc;
    char c;
    wnd=GetForegroundWindow();
    dc= GetDC(wnd);
    GetClientRect(wnd,&rc);
    c=(char)(board[x][y]+'0');
    TextOut(dc, rc.right-15, 3, &c, 1);
    ReleaseDC(wnd,dc);
}

#endif
