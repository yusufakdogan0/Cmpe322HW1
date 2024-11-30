#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_GAMES 100

// Game list
char *games[MAX_GAMES];
int current_game = 0;
int child_id = 0;
int total_games = 0;
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
    if (child_id != 0) {
        kill(child_id, signal);
    }
    printf("Exiting gracefully...\n");
    exit(0);
}

// Signal handler for SIGINT/SIGTERM in the child process
void child_signal_handler(int signal) {
    printf("Exiting gracefully...\n");
    exit(0);
}

void prevent_signal_handler(int sig) {
    printf("Parent process received signal %d, ignoring...\n", sig);
}

// Dynamically populate the games array with executables starting with "game_"
void scan_games_directory() {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;

    dir = opendir(".");
    if (!dir) {
        perror("Failed to open directory");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        // Check if the file starts with "game_" and is executable
        if (strncmp(entry->d_name, "game_", 5) == 0) {
            stat(entry->d_name, &file_stat);
            if (file_stat.st_mode & S_IXUSR && !S_ISDIR(file_stat.st_mode)) {
                games[total_games] = strdup(entry->d_name);
                total_games++;
                if (total_games >= MAX_GAMES - 1) {
                    break;
                }
            }
        }
    }
    closedir(dir);

    games[total_games++] = strdup("Exit"); // Add the Exit option
}

// Run the selected game as a child process
void run_game(const char *game) {
    pid_t pid = fork();
    if (pid == 0) {  // Child process
        signal(SIGINT, child_signal_handler);
        signal(SIGTERM, child_signal_handler);
        reset_terminal_mode();
        execl(game, game, NULL);
        perror("Failed to launch game");
        exit(EXIT_FAILURE);
    } 
    else if (pid > 0) {  // Parent process
        child_id = pid;
        signal(SIGINT, prevent_signal_handler);
        signal(SIGTERM, handle_signal);
        wait(NULL);
        child_id = 0;
        reset_terminal_mode();
        set_terminal_mode();
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
                    reset_terminal_mode();
                    printf("\nExiting gracefully...\n");
                    exit(0);
                } else {
                    run_game(games[current_game]);
                }
                break;
            case 'q':
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
    scan_games_directory();
    set_terminal_mode();
    display_menu();
}
