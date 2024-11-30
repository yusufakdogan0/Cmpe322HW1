#include <unistd.h>  // Required for sleep()
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>



#define WINNING_SCORE 21
#define DECK_ROWS 4
#define DECK_COLS 13

int gameOver = 0;
int roundOver = 0;
int yourScore = 0;
int botScore = 0;
int yourAceCount = 0;
int botsAceCount = 0;
int yourMoney;
char deck[DECK_ROWS][DECK_COLS][3];
const char *suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};

void game();

void endOfGame(){
    if (gameOver == 0){ return;}
    if (gameOver == 1){
        printf("Only idiots would quit after betting the money");
    }
    if (gameOver == 2){
        printf("This place was not for cowards nor beggars!!! (You got beaten up and thrown out) \n");
    }
    if (gameOver == 3){
        printf("The other guy quit because he did not have enough money to bet \n");
    }
    printf("Enter r for replay, enter q for exit");
    char choice;
    scanf(" %c", &choice);
    if(choice == 'r'){
        game();
    }
    if ( choice == 'q'){
        return;
    }
}

void initialize_deck() {
    const char *faces[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    for (int i = 0; i < DECK_ROWS; i++) {
        for (int j = 0; j < DECK_COLS; j++) {
            strcpy(deck[i][j], faces[j]);
        }
    }
}

int get_card_value(const char *card) {
    if (strcmp(card, "A") == 0) return 11;
    if (strcmp(card, "K") == 0 || strcmp(card, "Q") == 0 || strcmp(card, "J") == 0) return 10;
    return atoi(card);
}


void update_hand_value(int *score, int *ace_count, const char *card) {
    int value = get_card_value(card);
    if (value == 11) (*ace_count)++;
    *score += value;
    while (*score > WINNING_SCORE && *ace_count > 0) {
        *score -= 10;
        (*ace_count)--;
    }
}
char* draw_card(char* msg) {
    int row, col;
    do {
        row = rand() % DECK_ROWS;
        col = rand() % DECK_COLS;
    } while (deck[row][col][0] == '\0'); // Skip already drawn cards
    char *card = strdup(deck[row][col]);
    printf("%s draws: %s of %s\n", msg, card, suits[row]);
    deck[row][col][0] = '\0'; // Mark card as used
    sleep(1);

    return card;
}

void player_turn() {
    while (1) {
        printf("Your current score: %d \n",yourScore);
        printf("Kurpier's current score: %d\n", botScore);
        printf("Do you want to [h]it, [s]tand, or [q]uit? ");
        char choice;
        scanf(" %c", &choice);
        if (choice == 'q') {
            gameOver = 1;
            roundOver = 1;
            return;
        }
        if (choice == 's') break;
        if (choice == 'h') {
            char *card = draw_card("Player");
            update_hand_value(&yourScore, &yourAceCount, card);
            if (yourScore > WINNING_SCORE) {
                printf("You bust! Your score: %d\n", yourScore);
                roundOver = 1;
                break;
            }
        }
    }
    sleep(2);
}

void bot_turn() {
    while (botScore < 17) {
        char *card = draw_card("Kurpier");
        update_hand_value(&botScore, &botsAceCount, card);
        printf("Kurpier gets %d score \n", botScore);
        sleep(2);
    }
}

void determine_winner(int bet) {
    if (gameOver != 0){
        return;
    }
    if (yourScore > WINNING_SCORE) {
        printf("You lost this round!\n");
    } else if (botScore > WINNING_SCORE || yourScore > botScore) {
        printf("You won this round!\n");
        yourMoney += 2 * bet;
    } else if (yourScore < botScore) {
        printf("You lost this round!\n");
    } else {
        printf("It's a tie!\n");
        yourMoney += bet;
    }
    sleep(2);
    system("clear");

    printf("Your Money: $%d\n", yourMoney);

}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);  // Read until newline or EOF
}

void game(){
    srand(time(NULL));
    sleep(2);
    system("clear");
    printf("Welcome to Blackjack with Betting!\n");
    printf("How much money do you want to exchange? $");
    scanf("%d", &yourMoney);


    while (gameOver==0 && yourMoney > 0) {
        system("clear");
        initialize_deck();
        printf("Enter your bet: ");
        int bet = 0;
        int valid = scanf("%d", &bet);
        if ( valid != 1) {
            printf("Invalid input, try again !!! \n");
            clear_input_buffer();
            continue;
        }
        else if (bet < 100){
            gameOver = 2;
            break;
        }
        else if (bet > yourMoney){
            printf("You can only bet the amount of coins u have !!! \n");
            continue;
        }
        else{
            yourMoney -= bet;
        }

        yourScore = botScore = yourAceCount = botsAceCount = 0;

        update_hand_value(&yourScore, &yourAceCount, draw_card("Player"));
        update_hand_value(&botScore, &botsAceCount, draw_card("Kurpier"));
        update_hand_value(&yourScore, &yourAceCount, draw_card("Player"));


        player_turn();
        if (!roundOver){
            update_hand_value(&botScore, &botsAceCount, draw_card("Kurpier"));
            bot_turn();
        }
        determine_winner(bet);
    }
    endOfGame();
}



int main() {
    //signal(SIGINT, handle_signal);
    //signal(SIGTERM, handle_signal);
    game();
    return 0;
}

