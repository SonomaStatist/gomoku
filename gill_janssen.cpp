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
    int moves = 100;

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

    return aimoves[best_move];
}

int alphabeta(int d, int a, int b, bool max_player)
{
    // this is not correct, it needs to be fixed
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
