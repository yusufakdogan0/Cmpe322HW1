#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <sys/wait.h>

// Game list
const char *games[] = {"game_snake", "game_platform", "game_blackcheck", "Exit"};
int current_game = 0;
int total_games = sizeof(games) / sizeof(games[0]);

struct termios orig_termios;

// Restore terminal settings on exit
void reset_terminal_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

// Set terminal to raw mode for key detection
void set_terminal_mode() {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(reset_terminal_mode);
    new_termios = orig_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO); // Disable line buffering and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

// Signal handler for graceful exit
void handle_signal(int signal) {
    reset_terminal_mode();
    printf("\nExiting gracefully...\n");
    exit(0);
}

// Clear screen and display the main menu
void display_menu() {
    system("clear");
    printf("Video Game Console Main Screen\n");
    printf("==============================\n\n");
    printf("Use 'w'/'s' to navigate, 'a'/'d' to select buttons, and 'Enter' to start.\n");
    printf("Press 'q' to quit.\n\n");

    for (int i = 0; i < total_games; i++) {
        if (i == current_game) {
            printf("-> %s\n", games[i]);
        } else {
            printf("   %s\n", games[i]);
        }
    }
}

// Run the selected game as a child process
void run_game(const char *game) {
    pid_t pid = fork();

    if (pid == 0) {  // Child process
        // Launch the game
        execl(game, game, NULL);
        perror("Failed to launch game");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  // Parent process (main screen)
        waitpid(pid, NULL, 0); // Wait for the child process to finish
    } else {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
}

int main() {
    set_terminal_mode();
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    char input;
    display_menu();

    while (1) {
        input = getchar();
        switch (input) {
            case 'w': // Navigate up
                current_game = (current_game - 1 + total_games) % total_games;
                display_menu();
                break;
            case 's': // Navigate down
                current_game = (current_game + 1) % total_games;
                display_menu();
                break;
            case 'a': case 'd': // Buttons left and right (not functional here but reserved)
                break;
            case '\n': // Enter key to start game
                if (strcmp(games[current_game], "Exit") == 0) {
                    reset_terminal_mode();
                    printf("\nExiting gracefully...\n");
                    exit(0);
                } else {
                    run_game(games[current_game]);
                    display_menu();  // Redisplay the menu after game exits
                }
                break;
            case 'q': // Quit the main screen
                reset_terminal_mode();
                printf("\nExiting gracefully...\n");
                exit(0);
                break;
            default:
                break;
        }
    }
    return 0;
}