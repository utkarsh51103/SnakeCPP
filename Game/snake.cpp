#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <cstdlib>

using namespace std;

bool gameover;
const int width = 40;
const int height = 20;

int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100], ntail;
enum eDirection
{
    STOP = 0,
    UP,
    DOWN,
    LEFT,
    RIGHT
};
eDirection dir;

int kbhit()
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

char getch()
{
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void Setup()
{
    srand(time(0));
    gameover = false;
    x = width / 2;
    dir = STOP;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    ntail = 0;
}

void Draw()
{
    system("clear");

    for (int i = 0; i < width; i++)
    {
        cout << "#";
    }
    cout << endl;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j == 0 || j == width - 1)
            {
                cout << "#";
            }
            else if (i == y && j == x)
            {
                cout << "O";
            }
            else if (i == fruitY && j == fruitX)
            {
                cout << "*";
            }
            else
            {
                bool print = false;
                for (int k = 0; k < ntail; k++)
                {
                    if (i == tailY[k] && j == tailX[k])
                    {
                        cout << "o";
                        print = true;
                    }
                }
                if (!print)
                {
                    cout << " ";
                }
            }
        }
        cout << endl;
    }

    for (int i = 0; i < width; i++)
    {
        cout << "#";
    }
    cout << endl;
    cout << "Score: " << score << endl;
}

void Input()
{
    if (kbhit())
    {
        switch (getch())
        {
        case 'w':
            dir = UP;
            break;
        case 's':
            dir = DOWN;
            break;
        case 'a':
            dir = LEFT;
            break;
        case 'd':
            dir = RIGHT;
            break;
        case 'x':
            gameover = true;
            break;
        default:
            break;
        }
    }
}

void Logic()
{
    int prevX = tailX[0];
    int prevY = tailY[0];
    tailX[0] = x;
    tailY[0] = y;
    int prev2X, prev2Y;

    for (int i = 1; i < ntail; i++)
    {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (dir)
    {
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    }

    if (x < 0 || x >= width || y < 0 || y >= height)
    {
        gameover = true;
        cout << "Game Over" << endl;
    }

    for (int i = 0; i < ntail; i++)
    {
        if (x == tailX[i] && y == tailY[i])
        {
            gameover = true;
            cout << "Game Over" << endl;
        }
    }

    if (x == fruitX && y == fruitY)
    {
        score += 10;

        // Generate a new fruit at a valid position
        bool validPosition;
        do
        {
            fruitX = rand() % width;
            fruitY = rand() % height;
            validPosition = true;

            if (fruitX == width || fruitY == height || fruitX == 0 || fruitY == 0)
            {
                validPosition = false;
            }

            for (int i = 0; i < ntail; i++)
            {
                if (fruitX == tailX[i] && fruitY == tailY[i])
                {
                    validPosition = false;
                    break;
                }
            }

        } while (!validPosition || (fruitX == x && fruitY == y));

        ntail++;
    }
}

int main()
{
    Setup();
    while (!gameover)
    {
        Draw();
        Input();
        Logic();
        usleep(150000);
    }
    return 0;
}
