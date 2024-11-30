#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// Game list
const char *games[] = {"game_snake", "game_platform", "game_blackjack", "Exit"};
int current_game = 0;
int total_games = sizeof(games) / sizeof(games[0]);

// Signal handler for graceful exit
void handle_signal(int signal) {
    printf("\nExiting gracefully...\n");
    system("clear");
    exit(0);
}

// Signal handler for SIGINT/SIGTERM in the child process
void child_signal_handler(int signal) {
    printf("Exiting game gracefully...\n");
    exit(0);
}

void prevent_signal_handler(int sig) {
    printf("Parent process received signal %d, ignoring...\n", sig);
}

// Run the selected game as a child process
void run_game(const char *game) {
    pid_t pid = fork();
    if (pid == 0) {  // Child process
        signal(SIGINT, child_signal_handler);
        signal(SIGTERM, child_signal_handler);
        execl(game, game, NULL);
        perror("Failed to launch game");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  // Parent process
        signal(SIGINT, prevent_signal_handler);
        signal(SIGTERM, prevent_signal_handler);
        wait(NULL);
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
            case 'w':
                current_game = (current_game - 1 + total_games) % total_games;
                break;
            case 's':
                current_game = (current_game + 1) % total_games;
                break;
            case '\n':
                if (strcmp(games[current_game], "Exit") == 0) {
                    printf("\nExiting gracefully...\n");
                    exit(0);
                } else {
                    run_game(games[current_game]);
                }
                break;
            case 'q':
                printf("\nExiting gracefully...\n");
                exit(0);
                break;
            default:
                break;
        }
    }
}

int main() {
    display_menu();
}
