#include "pisqpipe.h"
#include <windows.h>

const char *infotext="name=\"AlphaBeta\", author=\"Amandeep Gill && Kyle Janssen\", version=\"0.1\", country=\"USA\", www=\"https://github.com/SonomaStatist/gomoku\"";

#define MAX_BOARD 100
int board[MAX_BOARD][MAX_BOARD];
static unsigned seed;

aiMove get_move()
{
    int a = -10000;
    int b = 10000;
    int moves = 25;

    aiMove[] aimoves = generate_moves(true, moves);

    int best_move = 0;
    int mval = -10000;
    for (int i = 0; i < moves; i++)
    {
        do_move(aimoves[i]);
        int tmp_mval = alphabeta(5, a, b, false);
        undo_move(aimoves[i]);
        if (tmp_mval > mvl)
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
    int moves = 25;
    aiMove[] aimoves = generate_moves(true, moves);

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
                goto ceanup_ab;
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

aiMove[] generate_moves(bool max_player, int &moves)
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

    aiMove[] aimoves = new aiMove[moves];
    int used_moves = 0;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (board[x][y] == 0)
            {
                aiMove aimove(p, x, y);
                int otiles = 0;
                int ptiles = 0;
                for (int d = 0; d < 3; d++)
                {
                    int ot = check_dir(x, y, d, o);
                    int pt = check_dir(x, y, d, p);
                    aimove.dflag = aimove.dflag || ot >= 4;
                    aimove.gflag = aimove.gflag || pt >= 4;
                    otiles += ot;
                    ptiles += pt;
                }

                if (used_moves < moves && (otiles > 0 || ptiles > 0))
                {
                    aimoves[used_moves] = aimove;
                    used_moves++;
                }
            }
        }
    }

    if (used_moves == 0)
    {
        int x = -1, y = -1;
        while (x < 0 && y < 0 && x >= width && y >= height)
        {
            x = rnd(width);
            y = rnd(height);
        }
        aimoves[0] = aiMove(p, x, y);
        moves = 1;
    }

    return aimoves;
}

int check_dir(int x, int y, int dir, int p)
{
    int o = p == 1 ? 2 : 1;
    int r = 0, l = 0;
    bool rc = true, lc = true;
    switch (d)
    {
    case 0:
        for (int i = 1; i < 4; i++)
        {
            if (rc && x + i < width)
            {
                if (board[x+i][y] == p)
                {
                    r++;
                }
                else if (board[x+i][y] == o)
                {
                    rc = false;
                }
            }

            if (lc && x - i >= 0)
            {
                if (board[x-i][y] == p)
                {
                    r++;
                }
                else if (board[x-i][y] == o)
                {
                    rc = false;
                }
            }
        }
        break;

    case 1:
        for (int i = 1; i < 4; i++)
        {
            if (rc && y + i < height)
            {
                if (board[x][y+i] == p)
                {
                    r++;
                }
                else if (board[x][y+i] == o)
                {
                    rc = false;
                }
            }

            if (lc && y - i >= 0)
            {
                if (board[x][y-i] == p)
                {
                    r++;
                }
                else if (board[x][y-i] == o)
                {
                    rc = false;
                }
            }
        }
        break;

    case 3:
        for (int i = 1; i < 4; i++)
        {
            if (rc && x + i < width && y + i < height)
            {
                if (board[x+i][y+i] == p)
                {
                    r++;
                }
                else if (board[x+i][y+i] == o)
                {
                    rc = false;
                }
            }

            if (lc && x - i >= 0 && y - i >= 0)
            {
                if (board[x-i][y-i] == p)
                {
                    r++;
                }
                else if (board[x-i][y-i] == o)
                {
                    rc = false;
                }
            }
        }
        break;
    }

    return r + l;
}

int eval_board(int p)
{
    int score = 0;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (board[x][y] == p)
            {
                for (int i = 0; i < 3; i++)
                {
                    int tmp = check_dir(x, y, i, p);
                    score += tmp * tmp * tmp;
                }
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

    }

    return 0;
}

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

int isFree(int x, int y)
{
    return x>=0 && y>=0 && x<width && y<height && board[x][y]==0;
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
