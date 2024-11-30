#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <sys/wait.h>

// Game list
const char *games[] = {"game_snake", "game_platform", "game_blackjack", "Exit"};
int current_game = 0;
int child_id = 0;
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
    if (child_id != 0){
        kill(child_id, signal);
    }
    printf("Exiting gracefully...\n");
    exit(0);
}

// Signal handler for SIGINT/SIGTERM in the child process
void child_signal_handler(int signal) {
    // Gracefully exit child game process and return to the main screen
    printf("Exiting gracefully...\n");
    exit(0);  // This will terminate the game and return control to the main screen
}

void prevent_signal_handler(int sig) {
    // Handle signals in the parent process (prevent termination)
    printf("Parent process received signal %d, ignoring...\n", sig);
}



// Run the selected game as a child process
void run_game(const char *game) {
    pid_t pid = fork();

    if (pid == 0) {  // Child process
        // Register signal handlers for SIGINT and SIGTERM in the child process
        signal(SIGINT, child_signal_handler);
        signal(SIGTERM, child_signal_handler);
        reset_terminal_mode();
        

        // Launch the game
        execl(game, game, NULL);
        perror("Failed to launch game");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  // Parent process (main screen)
        child_id = pid;
        signal(SIGINT, prevent_signal_handler);
        signal(SIGTERM, handle_signal);
        wait(NULL);// Wait for the child process to finish
        child_id = 0;
        set_terminal_mode();
    } else {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
}

// Clear screen and display the main menu
void display_menu() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    while (1) {
        system("clear");
        printf("Video Game Console Main Screen\n");
        printf("==============================\n\n");
        printf("Use 'w'/'s' to navigate and 'Enter' to start.\n");
        printf("Press 'q' to quit.\n\n");

        for (int i = 0; i < total_games; i++) {
            printf("%s %s\n", (i == current_game) ? "->" : "  ", games[i]);
        }

        char input = getchar();
        switch (input) {
            case 'w': // Navigate up
                current_game = (current_game - 1 + total_games) % total_games;
                break;
            case 's': // Navigate down
                current_game = (current_game + 1) % total_games;
                break;
            case '\n': // Enter key to start game
                if (strcmp(games[current_game], "Exit") == 0) {
                    reset_terminal_mode();
                    printf("\nExiting gracefully...\n");
                    exit(0);
                } else {
                    run_game(games[current_game]);
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
}

int main() {
    set_terminal_mode();
    display_menu();
}
