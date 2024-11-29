#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

#define WIDTH 15
#define HEIGHT 15
#define PLATFORM_WIDTH 3
#define INITIAL_POINTS 20
#define TIME_DELAY 300000 // Microseconds

// Terminal settings
struct termios original, current;

// Function declarations for terminal handling
void initializeTermios();
void resetTermios();
int kbhit();
char getch();

// Game variables
int platform_pos = WIDTH / 2 - PLATFORM_WIDTH / 2;
int points = INITIAL_POINTS;
int game_over = 0;
int gold_present = 0;
char grid[HEIGHT][WIDTH];

// Function declarations for game logic
void init_game();
void display_grid();
void update_game();
void move_platform(char direction);
void drop_gold();

int main() {
    srand(time(NULL));
    char input;
    initializeTermios(); // Set terminal mode
    init_game();
    while (1) {
        system("clear");
        display_grid();
        printf("\nPoints: %d\n", points);
        printf("Press 'q' to quit, 'r' to restart\n");

        if (kbhit()) {
            input = getch();
            if (input == 'q') {
                printf("Exiting the game.\n");
                break;
            }
            else if (input == 'r') {
                init_game();
            }
            else if (input == 'a' || input == 'd' && !game_over) {
                move_platform(input);
            }
        }

        if (!game_over){
            if (!gold_present) {
                drop_gold();
            }
            update_game();
        }

        usleep(TIME_DELAY); // Delay for easier gameplay
    }

    resetTermios(); // Restore terminal settings
    return 0;
}

// Initialize the game grid and platform position
void init_game() {
    points = INITIAL_POINTS;
    game_over = 0;
    gold_present = 0;
    platform_pos = WIDTH / 2 - PLATFORM_WIDTH / 2;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            grid[i][j] = '.';
        }
    }

}

// Display the grid
void display_grid() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == HEIGHT - 1 && j >= platform_pos && j < platform_pos + PLATFORM_WIDTH) {
                printf("_");
            } else {
                printf("%c", grid[i][j]);
            }
        }
        printf("\n");
    }
}

// Drop a single gold at a random column
void drop_gold() {
    int col = rand() % WIDTH;
    grid[0][col] = 'G';
    gold_present = 1;
}

// Update the grid and check for collisions
void update_game() {
    for (int i = HEIGHT - 2; i >= 0; i--) {
        for (int j = 0; j < WIDTH; j++) {
            if (grid[i][j] == 'G') {
                grid[i][j] = '.';
                if (i + 1 == HEIGHT - 1 && j >= platform_pos && j < platform_pos + PLATFORM_WIDTH) {
                    points += 10;
                    gold_present = 0;
                }
                else if (i + 1 == HEIGHT - 1) {
                    points -= 10;
                    gold_present = 0;
                }
                else {
                    grid[i + 1][j] = 'G';
                }
            }
        }
    }
    if (points <= 0) {
        game_over = 1;
        printf("You lost! Press 'r' to restart or 'q' to quit.\n");
    } else if (points >= 100) {
        game_over = 1;
        printf("You won! Press 'r' to restart or 'q' to quit.\n");
    }
}

// Move the platform left or right
void move_platform(char direction) {
    if (direction == 'a' && platform_pos > 0) {
        platform_pos--;
    } else if (direction == 'd' && platform_pos < WIDTH - PLATFORM_WIDTH) {
        platform_pos++;
    }
}

// Initialize terminal in non-canonical mode
void initializeTermios() {
    tcgetattr(STDIN_FILENO, &original);
    current = original;
    current.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &current);
}

// Restore terminal to original state
void resetTermios() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original);
}

// Check if a key has been pressed
int kbhit() {
    struct timeval tv = {0, 0};
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    return select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &tv);
}

// Get a single character input
char getch() {
    char c;
    read(STDIN_FILENO, &c, 1);
    return c;
}
