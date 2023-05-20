#include <stdio.h>
#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static int DELAY = 20;
static int ROTATE_CHANCE_DEBUFF = 60;

enum Direction
{
    Left = 0,
    Rigth = 1,
    Down = 2,
    Up = 3
};

struct Point
{
    int y;
    int x;
};

struct Pipe
{
    enum Direction dir_frb;
    enum Direction dir_cur;
    struct Point pos;
    short clr_p;
};

int arand()
{
    return abs(rand());
}

enum Direction dir_not(enum Direction dir)
{
    return dir % 2 == 0 ? dir + 1 : dir - 1;
}

void pipe_refresh(struct Pipe *this)
{
    this->dir_frb = arand() % 4;
    this->dir_cur = dir_not(this->dir_frb);
    switch (this->dir_frb)
    {
    case Left:
        this->pos.y = arand() % LINES;
        this->pos.x = 0;
        break;
    case Rigth:
        this->pos.y = arand() % LINES;
        this->pos.x = COLS - 1;
        break;
    case Up:
        this->pos.y = 0;
        this->pos.x = arand() % COLS;
        break;
    case Down:
        this->pos.y = LINES - 1;
        this->pos.x = arand() % COLS;
        break;
    default:
        break;
    }
    this->clr_p = arand() % 7;

    color_set(this->clr_p, NULL);
    move(this->pos.y, this->pos.x);
}

bool pipe_mvp(struct Pipe *this)
{
    enum Direction dir_prev = this->dir_cur;
    unsigned short diff;
    switch (this->dir_cur)
    {
    case Left:
        diff = (double)this->pos.x / COLS * 100;
        break;
    case Rigth:
        diff = (double)(COLS - this->pos.x) / COLS * 100;
        break;
    case Down:
        diff = (double)this->pos.y / LINES * 100;
        break;
    case Up:
        diff = (double)(LINES - this->pos.y) / LINES * 100;
        break;
    default:
        break;
    }
    if (arand() % 100 + ROTATE_CHANCE_DEBUFF < diff)
    {
        while (1)
        {
            enum Direction dir_new = arand() % 4;
            if (dir_new != this->dir_frb && dir_new != dir_not(this->dir_cur))
            {
                this->dir_cur = dir_new;
                break;
            }
        }
    }
    chtype symb;
    if (this->dir_cur == dir_prev)
    {
        if (this->dir_cur == Left || this->dir_cur == Rigth)
            symb = ACS_HLINE;
        else
            symb = ACS_VLINE;
    }
    else
    {
        if (dir_prev == Left && this->dir_cur == Down)
            symb = ACS_ULCORNER;
        else if (dir_prev == Up && this->dir_cur == Rigth)
            symb = ACS_ULCORNER;
        else if (dir_prev == Down && this->dir_cur == Rigth)
            symb = ACS_LLCORNER;
        else if (dir_prev == Left && this->dir_cur == Up)
            symb = ACS_LLCORNER;
        else if (dir_prev == Rigth && this->dir_cur == Down)
            symb = ACS_URCORNER;
        else if (dir_prev == Up && this->dir_cur == Left)
            symb = ACS_URCORNER;
        else if (dir_prev == Rigth && this->dir_cur == Up)
            symb = ACS_LRCORNER;
        else if (dir_prev == Down && this->dir_cur == Left)
            symb = ACS_LRCORNER;
        else
            symb = ACS_BULLET; // error-recognition value
    }
    addch(symb);
    switch (this->dir_cur)
    {
    case Left:
        this->pos.x -= 1;
        break;
    case Rigth:
        this->pos.x += 1;
        break;
    case Down:
        this->pos.y += 1;
        break;
    case Up:
        this->pos.y -= 1;
        break;
    default:
        break;
    }
    if (0 <= this->pos.y && this->pos.y < LINES && 0 <= this->pos.x && this->pos.x < COLS)
        move(this->pos.y, this->pos.x);
    else
        return false;
    return true;
}

void process_arg(int argn, char **argv)
{
    if (strcmp(argv[argn], "--help") == 0)
    {
        printf("--delay [m]\tdelay every iteration for [m] milliseconds; default is 20\n");
        printf("--rotatedf [n]\tnumber from 0 to 100 which stands for rotation chance debuff; default is 60\n");
        exit(0);
    }
    else if(strcmp(argv[argn], "--delay") == 0)
    {
        int tmp;
        if(sscanf(argv[argn + 1], "%d", &tmp))
            DELAY = abs(tmp);
    }
    else if(strcmp(argv[argn], "--rotatedf") == 0)
    {
        int tmp;
        if(sscanf(argv[argn + 1], "%d", &tmp))
            ROTATE_CHANCE_DEBUFF = abs(tmp) % 100;
    }
    else
    {
        printf("Did you mean '--help?'\n");
        exit(1);
    }
}

int main(int argc, char **argv)
{
    for (int i = 0; i < argc; i++)
        if(strncmp(argv[i], "--", 2) == 0)
            process_arg(i, argv);

    srand(time(NULL));
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    start_color();
    keypad(stdscr, TRUE);
    refresh();

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);

    struct Pipe pip;
    while (true)
    {
        pipe_refresh(&pip);
        while (pipe_mvp(&pip))
        {
            refresh();
            usleep(DELAY * 1000);
        }
    }

    endwin();
    return 0;
}