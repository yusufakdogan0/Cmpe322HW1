#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For usleep
#include <termios.h> // For non-blocking input
#include <time.h>
#include <sys/wait.h>
#include <signal.h>

#define WIDTH 15
#define HEIGHT 15

// Initialize board size
char board[HEIGHT][WIDTH];
// Global variables and structures
typedef struct {
    int x, y;
} Point;

Point snake[225];
int snakeLength = 1;
Point food;
int gameOver = 0;
int direction[2];
int eaten = 0;
int gameStop = 0;

// Terminal input configuration
struct termios original, current;





void initializeTermios() {
    tcgetattr(STDIN_FILENO, &original); // Get current terminal attributes
    current = original;
    current.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &current); // Apply new attributes
}

void resetTermios() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original); // Restore original attributes
}

int kbhit() {
    struct timeval tv = {0, 0};
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    return select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &tv);
}

char getch() {
    char c;
    read(STDIN_FILENO, &c, 1);
    return c;
}

/***
void handle_signal(int signal) {
    printf("exiting gracefully....");
    sleep(2);
    resetTermios();
    exit(0);
}***/


void generateFood(){
    food.x = rand() % WIDTH;
    food.y = rand() % HEIGHT;
    for(int i = 0; i < snakeLength; i++){
        if(snake[i].x == food.x && snake[i].y == food.y){
            return generateFood();
        }
    }
}

void initializeGame() {
    // Initialize the snake's starting position
    snake[0].x = WIDTH / 2;
    snake[0].y = HEIGHT / 2;
    generateFood();
    direction[0] = 0;
    direction[1] = 1;
}

void drawBoard() {
    // Fill the board with empty spaces
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            board[i][j] = '.'; // Empty space
        }
    }
    board[snake[0].y][snake[0].x] = '0';
    // Place the snake on the board
    for (int i = 1; i < snakeLength; i++) {
        board[snake[i].y][snake[i].x] = '#';
    }

    // Place the food on the board
    board[food.y][food.x] = 'X'; // 'X' for food

    // Print the board to the screen
    system("clear"); // Clear screen in Linux
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            printf("%c", board[i][j]); // Print each character
        }
        printf("\n");
    }
}

void moveSnake() {
    if (gameStop) {
        return;
    }
    Point prev = snake[0], temp;
    snake[0].y += direction[0];
    snake[0].x += direction[1];
    for (int i = 1; i < snakeLength; i++) {
        temp = snake[i];
        snake[i] = prev;
        prev = temp;
    }
    if (eaten != 0) {
        snake[snakeLength] = prev;
        snakeLength++;
        eaten--;
    }
}

Point nextLocation() {
    Point point = snake[0];
    point.x += direction[1];
    point.y += direction[0];
    return point;
}

void checkCollision() {
    gameStop = 0;
    Point next = nextLocation();
    // Wall collision
    if (next.x < 0 || next.x > WIDTH - 1 ||
        next.y < 0 || next.y > HEIGHT - 1) {
        gameStop = 1;
    }

    // Self collision
    for (int i = 1; i < snakeLength; i++) {
        if (next.x == snake[i].x && next.y == snake[i].y) {
            gameStop = 1;
        }
    }

    // Food collision
    if (snake[0].x == food.x && snake[0].y == food.y) {
        eaten++;
        generateFood();
    }
}

void getInput() {
    usleep(100000); // 100 milliseconds
    if (kbhit()) { // Check if a key is pressed
        switch (getch()) {
            case 'w':
                direction[0] = -1;
                direction[1] = 0;
                break;
            case 's':
                direction[0] = 1;
                direction[1] = 0;
                break;
            case 'a':
                direction[0] = 0;
                direction[1] = -1;
                break;
            case 'd':
                direction[0] = 0;
                direction[1] = 1;
                break;

            case 'q':
                gameOver = 1;
        }
    }
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    srand(time(0));
    initializeTermios(); // Initialize terminal for non-blocking input
    atexit(resetTermios); // Reset terminal settings on exit
    initializeGame();

    while (!gameOver) {
        drawBoard();
        getInput();
        checkCollision();
        moveSnake();
    }

    printf("Game Over!\n");
    return 0;
}
