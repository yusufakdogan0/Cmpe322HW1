#ifndef SNAKE_H
#define SNAKE_H

#define WIDTH 15
#define HEIGHT 15

typedef struct {
    int x, y;
} Point;

// Global Variables
extern Point snake[225];
extern int snakeLength;
extern Point food;
extern int gameOver;
extern int direction[2];
extern int eaten;
extern char board[WIDTH][HEIGHT];
extern int gameStop;

// Function Prototypes
void initializeGame();
void drawBoard();
void moveSnake();
void checkCollision();
void getInput();

#endif // SNAKE_H
