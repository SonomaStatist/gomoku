/** header with functions and variables for pipe AI */

/* information about a game - you should use these variables */
extern int width,height; /* the board size */
extern int info_timeout_turn;  /* time for one turn in milliseconds */
extern int info_timeout_match; /* total time for a game */
extern int info_time_left;  /* remaining time for a game */
extern int info_max_memory; /* maximum memory in bytes, zero if unlimited */
extern int info_game_type;  /* 0:human opponent, 1:AI opponent, 2:tournament, 3:network tournament */
extern int info_exact5;     /* 0:five or more stones win, 1:exactly five stones win */
extern int info_continuous; /* 0:single game, 1:continuous */
extern int terminate; /* return from brain_turn when terminate>0 */
extern unsigned start_time; /* tick count at the beginning of turn */
extern char dataFolder[256]; /* folder for persistent files */

typedef struct
{
    int n; /* the number of neighboring player tiles */
    int p; /* the player whose tile to use */
    int x;
    int y;
    bool dflag; /* a flag if the opponent has many tiles in a line here */
    bool gflag; /* a flag if the player has many tiles in a line here */
    inline aiMove(int p_=0, int x_=0, int y_=0, int n_=0) :
        n(n_), p(p_), x(x_), y(y_)
    {
        dflag = false;
        gflag = false;
    }
} aiMove;

/* you have to implement these functions */
void brain_init(); /* create the board and call pipeOut("OK"); */
void brain_restart(); /* delete old board, create new board, call pipeOut("OK"); */
void brain_turn(); /* choose your move and call do_mymove(x,y);
                      0<=x<width, 0<=y<height */
void brain_my(int x,int y); /* put your move to the board */
void brain_opponents(int x,int y); /* put opponent's move to the board */
void brain_block(int x,int y); /* square [x,y] belongs to a winning line (when info_continuous is 1) */
int brain_takeback(int x,int y); /* clear one square; return value: 0:success, 1:not supported, 2:error */
void brain_end();  /* delete temporary files, free resources */

/* AI's alphabeta pruning and helper functions */
aiMove get_move(); /* sets up call to alphabeta() */
int alphabeta(int d, int a, int b, bool max_player); /* depth, alpha, beta, eval for max/min player */
int get_winner(); /* returns 0 for no winner, 1 or 2 if player 1 or 2 has won */
void do_move(const aiMove &aim);
void undo_move(const aiMove &aim);
int eval_board(int player);
int check_dir(int x, int y, int dir, int p); /* checks the given location and direction for player's tiles */
aiMove[] generate_moves(bool max_player, int &moves);
/* generate moves for 1 (true) or 2 (false) generate up to &moves, and replace
   moves with the number of moves generated */

#ifdef DEBUG_EVAL
void brain_eval(int x,int y); /* display evaluation of square [x,y] */
#endif

#ifndef ABOUT_FUNC
extern const char *infotext; /* AI identification (copyright, version) */
#else
void brain_about(); /* call pipeOut(" your AI info ") */
#endif

/* these functions are in pisqpipe.cpp */
extern int pipeOut(char *fmt,...);
extern void do_mymove(int x,int y);
extern void suggest(int x,int y);