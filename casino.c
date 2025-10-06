#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <ctype.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
// Platform-specific includes
#ifdef _WIN32
    #include <windows.h> // For Windows
#else
    #include <unistd.h>  // For Linux/Unix
#endif

#define SUITS 4
#define FACES 13
#define CARDS 52
#define RACETRACK 25 // Free to change
#define HORSES 5 // Free to change

typedef struct { 
    char name[50];
    int age;
    float balance;
} player_info;

// Global variables
int deck[SUITS][FACES] = {0}; 
const char *suits[SUITS] = {"Hearts", "Diamonds", "Clubs", "Spades"};
const char *faces[FACES] = {"Ace", "Deuce", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King"};

// Prototypes
// Main functions
// Function that takes a function pointer and a player_info pointer
void game_function (void (*game)(player_info*), player_info* ); // Ask user play again?
void player_profile(player_info*);
void black_jack (player_info*);
void baccarat (player_info*);
void slots (player_info*);
void horse_race (player_info*);

// Supporting functions
void shuffle_deck (void);
void deal_card (int hand[11], int current_card);
int evaluate_blackjack (int hand[11]); // Evaluate black jack hand
int evaluate_baccarat (int hand[11]); // Evaluate baccarat hand
int evaluate_slots (wchar_t slot_machine[3][3]); // Evaluate slots; Lose = 0, Small prize = 1, Big win = 2
float place_bet (player_info*); // Betting system
void horse_move (int*, char); // Random horse moves

// Visual effects
void delay_ms(int milliseconds); 
void slow_print (char[], int milliseconds); 
void display_profile (player_info*);
void set_locale (void);

// Input && Validation functions
void prompt(void); // Prompt user to press enter
void clear_buffer(void); // Flush buffer
void standard_input(char[], size_t); // Primary way to get input, size_t is size of array. 
void get_info (player_info*); // Get player info
void manage_balance (player_info*); // Deposit or withdraw money
int confirm(void); // Return 1 if confirmation success, else return 0
int is_digit(char[]); // Return 1 if string only contain digits, else return 0
int is_float (char []); // Return 1 if string is a valid float, else return 0
int is_empty(char input[]); // Return 1 if string is empty, else return 0
int is_zero(char[]); // Return 1 if input is 01, 03, 0XX, else return 0

int main (void) {
    set_locale(); 
    srand(time(NULL));
    player_info player = {0}; 
    char input[5] = "\0";
    int choice = -1;

    get_info (&player);

    while (choice != 5) {
        slow_print ("\n\n$$$ WELCOME TO ZHUN's CASINO $$$\n", 50);
        puts("Enter 0 to go to Player Profile");
        puts("Enter 1 to play Black Jack");
        puts("Enter 2 to play Baccarat");
        puts("Enter 3 to play Slots");
        puts("Enter 4 to play Horse Race Betting");
        puts("Enter 5 to Quit");
        printf("Please enter your choice: ");

        // Get input
        standard_input(input, sizeof(input));
        // Check if input is empty
        if (is_empty(input)) continue; // Get input again
        // Check if input is a number
        if (!is_digit(input)) continue; // Get input again
        // Check if a zero is followed by another number
        if (is_zero(input)) continue; // Get input again
        
        choice = atoi(input); // Convert input to number

        switch (choice) {
            default: puts("Invalid choice. Please enter a number between 0-5.");
                break;
            case 0: player_profile (&player);
                break;
            case 1: game_function (black_jack, &player);
                break;
            case 2: game_function (baccarat, &player);
                break;
            case 3: game_function (slots, &player);
                break;
            case 4: game_function (horse_race, &player);
                break;
            case 5: slow_print("~Thank you for playing~", 100);
                break;
        }
    }
}

// Main functions
// Function that takes a function pointer and a player_info pointer
void game_function(void (*game)(player_info*), player_info* player) {
    int choice = -1;
    char input[5];
    game(player); // Initiate first game
    while (choice != 0) { // Play again?
        puts("\nEnter 0 to exit game");
        puts("Enter 1 to play again");
        printf ("Please enter your choice: ");
        // Get input
        standard_input(input, sizeof(input));
        // Check if input is empty
        if (is_empty(input)) continue; // Get input again
        // Check if input is a number
        if (!is_digit(input)) continue; // Get input again
        // Check if a zero is followed by another number
        if (is_zero(input)) continue; // Get input again
        choice = atoi(input);

        switch (choice) {
            default: puts("Invalid choice. Please enter 0 or 1.");
                break;
            case 0: 
                puts("~Exiting game~");
                break;
            case 1:
                game(player);
                break;
        }
    }
    return;
}

void player_profile(player_info *player) {
    char input[5] = "\0";
    int choice = -1;

    while (choice != 2) {
        slow_print ("\n\n\033[0m=====PLAYER PROFILE=====\033[0m\n\n", 50);
        puts ("Enter 0 to view profile information");
        puts ("Enter 1 to deposit or withdraw money");
        puts ("Enter 2 to exit Player Profile");
        puts ("Please enter your choice: ");

        // Get input
        standard_input(input, sizeof(input));
        // Check if input is empty
        if (is_empty(input)) continue; // Get input again
        // Check if input is a number
        if (!is_digit(input)) continue; // Get input again
        // Check if a zero is followed by another number
        if (is_zero(input)) continue; // Get input again
        
        choice = atoi(input); // Convert input to number

        switch (choice) {
            default: puts("Invalid choice. Please enter a number between 0-2.");
                break;
            case 0: display_profile (player);
                break;
            case 1: manage_balance (player);
                break;
            case 2: slow_print("~Exiting player profile~", 100);
                break;
        }
    }
}

void black_jack(player_info *player) {
    int current_card = 1;
    int player_hand[11] = {0}; // Skip index 0, index 1 to 10 for {Ace,Deuce,..,Nine, (Ten, J, Q, K)}
    int dealer_hand[11] = {0};
    int player_sum;
    int dealer_sum;
    char input[5];
    int choice;

    slow_print("\n\033[0m=====BLACK JACK GAME=====\033[0m\n", 50);
    printf("\npress enter to start");
    prompt();

    float bet = place_bet(player);
    if (bet == 0) return;

    shuffle_deck();

    printf("\nDealer reveals first card: ");
    deal_card (dealer_hand, current_card++);
    puts("Second card faced down.");

    // Player plays
    printf("\npress enter to draw first card");
    prompt();
    printf("\nYou drew a ");
    deal_card (player_hand, current_card++);
    printf("press enter to draw second card");
    prompt();
    printf("\nYou drew a ");
    deal_card (player_hand, current_card++);

    player_sum = evaluate_blackjack(player_hand);

    while (player_sum < 21) {
        printf("Enter 0 to stand / 1 to hit: ");
        // Get input
        standard_input(input, sizeof(input));
        // Check if input is empty
        if (is_empty(input)) continue; // Get input again
        // Check if input is a number
        if (!is_digit(input)) continue; // Get input again
        // Check if a zero is followed by another number
        if (is_zero(input)) continue; // Get input again
        
        choice = atoi(input); // Convert input to number
        
        if (choice != 0 && choice != 1) {
            puts("Please enter a valid choice: 0 or 1");
            continue;
        }

        if (choice == 0) break;
        
        printf("\nYou drew a ");
        deal_card (player_hand, current_card++);
        player_sum = evaluate_blackjack(player_hand);
    }

    // Dealer reveal second card and proceed with standard rules
    printf("\nDealer reveals second card: ");
    deal_card (dealer_hand, current_card++);
    dealer_sum = evaluate_blackjack(dealer_hand);

    while (dealer_sum < 17 && player_sum <= 21) { // Dealer hits until 17
        printf("Dealer draws ");
        deal_card (dealer_hand, current_card++);
        dealer_sum = evaluate_blackjack(dealer_hand);
    }

    printf("You scored a hand value of %d\n", player_sum);
    printf("The dealer scored a hand value of %d\n", dealer_sum);

    // Check Results
    if (player_sum > 21) {
        slow_print("\nYou BUST, you lost\n", 100);
    }
    else if (dealer_sum > 21) {
        slow_print("\nDealer BUST, you won\n", 100);
        player->balance += 2 * bet;
    }
    else if (player_sum > dealer_sum) {
        slow_print("\nYou won\n", 100);
        player->balance += 2 * bet;
    }
    else if (player_sum < dealer_sum) {
        slow_print("\nYou lost\n", 100);
    } 
    else {
        slow_print("\nIts a draw\n", 100);
        player->balance += bet; // Return bet
    }
    printf("\nNew balance: %.2f\n\n", player->balance);
    return;
}

void baccarat (player_info *player) {
    int current_card = 1;
    int player_hand[11] = {0}; // Skip index 0, index 1 to 10 for {Ace,Deuce,..,Nine, (Ten, J, Q, K)}
    int banker_hand[11] = {0};
    int player_sum;
    int player_first2_sum; // Identify player's third card
    int player_third; // Player's third card
    int banker_sum;
    char input[5];
    int baccarat_bet;

    slow_print("\n\033[0m=====Baccarat Game=====\033[0m\n", 50);
    printf("press enter to start");
    prompt();

    while (1) { // Player place bet: banker, player, draw
        printf ("\nEnter 0 to bet on Banker hand\n"
                "Enter 1 to bet on Player hand\n"
                "Enter 2 to bet on draw\n"
                "Please enter your choice: ");
        // Get input
        standard_input (input, sizeof(input));
        // Check if input is empty
        if (is_empty(input)) continue; // Get input again
        // Check if input is a number
        if (!is_digit(input)) continue; // Get input again
        baccarat_bet = atoi(input);
        if (baccarat_bet > 2) {
            printf("Please enter a choice between 1 and 2\n");
            continue;
        }
        break;
    }

    float bet = place_bet(player);
    if (bet == 0) return;

    shuffle_deck();

    // Deal first 2 cards
    printf("\nBanker's first card: ");
    deal_card (banker_hand, current_card++);
    printf("Banker's second card: ");
    deal_card (banker_hand, current_card++);
    banker_sum = evaluate_baccarat (banker_hand);
    printf("Banker hand = %d\n", banker_sum);

    printf("\nPlayer's first card: ");
    deal_card (player_hand, current_card++);
    printf("player's second card: ");
    deal_card (player_hand, current_card++);
    player_sum = evaluate_baccarat (player_hand);
    player_first2_sum = player_sum;
    printf("Player hand = %d\n", player_sum);

    delay_ms (10000);
    // Check results
    if ((banker_sum == 8) || (banker_sum == 9) || (player_sum == 8) || (player_sum == 9)) { // Natural hand
        if (banker_sum > player_sum) {
            if (baccarat_bet == 0) {
                slow_print("\nYou won\n", 100);
                player->balance = player->balance + 0.95 * bet;
            }
            else slow_print("\nYou lost\n", 100);
        }
        if (player_sum > banker_sum) {
            if (baccarat_bet == 1) {
                slow_print("\nYou won\n", 100);
                player->balance += 2 * bet;
            }
            else slow_print("\nYou lost\n", 100);
        }
        if (player_sum == banker_sum) {
            if (baccarat_bet == 2) {
                slow_print("\nYou won\n", 100);
                player->balance += 2 * bet;
            }
            else slow_print("\nYou lost\n", 100);
        }
        printf("\nNew balance: %.2f\n\n", player->balance);
        return;
    }

    // Third card rules
    // Player hand
    if (player_sum <= 5) {
        printf("\nPlayer's third card: ");
        deal_card (player_hand, current_card++);
        player_sum = evaluate_baccarat (player_hand);
        printf("Player hand = %d\n", player_sum);
    }
    player_third = player_sum - player_first2_sum; // Calculate player's third card
    
    // Banker hand
    if (banker_sum <= 2) {
        printf("\nBanker's third card: ");
        deal_card (banker_hand, current_card++);
    }

    if (banker_sum == 3) {
        if (player_third != 8) {
            printf("\nBanker's third card: ");
            deal_card (banker_hand, current_card++);
        }
    }

    if (banker_sum == 4) {
        if (player_third >= 2 && player_third <= 7) {
            printf("\nBanker's third card: ");
            deal_card (banker_hand, current_card++);
        }
    }

    if (banker_sum == 5) {
        if (player_third >= 4 && player_third <= 7) {
            printf("\nBanker's third card: ");
            deal_card (banker_hand, current_card++);
        }
    }

    if (banker_sum == 6) {
        if (player_third == 6 || player_third == 7) {
            printf("\nBanker's third card: ");
            deal_card (banker_hand, current_card++);
        }
    }
    banker_sum = evaluate_baccarat (banker_hand);
    printf("Banker hand = %d\n", banker_sum);

    // Check results
    if (banker_sum > player_sum) {
        if (baccarat_bet == 0) {
            slow_print("\nYou won\n", 100);
            player->balance = bet + 0.95 * bet;
        }
        else slow_print("\nYou lost\n", 100);
    }
    if (player_sum > banker_sum) {
        if (baccarat_bet == 1) {
            slow_print("\nYou won\n", 100);
            player->balance += 2 * bet;
        }
        else slow_print("\nYou lost\n", 100);
    } 
    if (player_sum == banker_sum) {
        if (baccarat_bet == 2) {
            slow_print("\nYou won\n", 100);
            player->balance += 2 * bet;
        }
        else slow_print("\nYou lost\n", 100);
    }
    printf("\nNew balance: %.2f\n\n", player->balance);

    return;
}

void slots (player_info *player) {
    wchar_t slot_machine[3][3] = {0};
    const wchar_t slot_image[3] = {L'💵', L'💎', L'❌'};  
    int random_number;
    
    slow_print("\n\033[0m=====SLOTS=====\033[0m\n", 50);

    float bet = place_bet(player);
    if (bet == 0) return;
    
    for (int i = 0; i < 3; i++) { // Randomize slots
        for (int j = 0; j < 3; j++) {
            random_number = rand() % 10;
            switch (random_number) {
                case 0:
                case 1:
                case 2:
                    slot_machine[i][j] = slot_image[0];
                    break;
                case 3:
                case 4:
                    slot_machine[i][j] = slot_image[1];
                    break;
                default:    
                    slot_machine[i][j] = slot_image[2];
                    break;
            }
        }
    }
    
    printf("\npress enter to spin slots");
    prompt();
    printf("\n");

    for (int i = 0; i < 3; i++) { // Print slot machine
        for (int j = 0; j < 3; j++) {
            fflush(stdout);
            delay_ms(500);
            wprintf(L"%lc\t", slot_machine[i][j]);
        }
        printf("\n");
    }

    switch (evaluate_slots(slot_machine)) { // Check results
        case 0:
            slow_print("\nYou lost\n", 100);
            break;
        case 1:     
            slow_print("\nSMALL WIN!!!\n", 100);
            printf("You just 5x your bet\n");
            player->balance += 5 * bet;
            break;
        case 2:
            slow_print("\nBIG WIN!!!\n", 100);
            printf("You just 20x your bet\n");
            player->balance += 20 * bet;
            break;
    }
    printf("\nNew balance: %.2f\n\n", player->balance);
}

void horse_race (player_info *player) {
    char input[5];
    int horse; // Select horse
    int horse_position[HORSES] = {0}; // ALL horse at Starting line, position 1
    int finished = 0; // Mark number of finished horse
    int finisher[HORSES] = {0}; // Mark which horse finished
    int horse_choice; // Player select which horse to bet on

    for (int i = 0; i < HORSES; i++) { // Set all horse position to 1
        horse_position[i] = 1; 
    }
    
    slow_print("\n\033[0m=====Horse Race Betting=====\033[0m\n", 50);
    printf("press enter to start");
    prompt();

    while (1) { // Player select horse
        printf ("\nThere are %d horses\n", HORSES);
        printf ("Please select horse to bet on (1 to %d): ", HORSES);
        // Get input
        standard_input (input, sizeof(input));
        // Check if input is empty
        if (is_empty(input)) continue; // Get input again
        // Check if input is a number
        if (!is_digit(input)) continue; // Get input again
        horse_choice = atoi(input);
        if (horse_choice < 1 || horse_choice > HORSES) {
            printf("Please enter a choice between 1 and %d\n", HORSES);
            continue;
        }
        break;
    }
    
    float bet = place_bet(player);
    if (bet == 0) return;
    
    for (horse = 0; horse < HORSES; horse++) { // Print starting position
        printf("%d ", horse + 1);
        for (int i = 2; i <= RACETRACK; i++) {
            slow_print("_ ", 20);
        }
        puts("");
    }

    puts("\npress enter to start race");
    prompt();
    puts("BANG!!!!\nAND THEY ARE OFF!!!!\n");

    while (finished == 0) { // Initiate race
        for (horse = 0; horse < HORSES; horse++) {
            printf("Horse %d ", horse + 1);
            horse_move(&horse_position[horse], (char)('0' + horse + 1));
            delay_ms(500);
            if (horse_position[horse] == RACETRACK) {
                finisher[horse]++;
                finished++;
            }
        }
        printf("\n");
    }

    // Check Results
    // Check for draw 
    if (finished > 1) { // More than one finished
        for (horse = 0; horse < HORSES; horse++) { // Check if player wins or lose
            if (finisher[horse]) {
                if (horse_choice == horse + 1) { // Player draws
                slow_print("\nIt's a draw\n", 100);
                player->balance += bet; // Return bet
                printf("\nNew balance: %.2f\n\n", player->balance);
                return;
                }
            }
        }
    }

    // Player wins
    for (horse = 0; horse < HORSES; horse++) { // Check if player wins or lose
        if (finisher[horse]) {
            if (horse_choice == horse + 1) { // Player draws
                slow_print("\nYou won\n", 100); // Only one finish, win
                printf("You just %dx your bet", HORSES);
                player->balance += HORSES * bet; 
                printf("\nNew balance: %.2f\n\n", player->balance);
                return;
            }
        }
    }
    
    slow_print("\nYou lost\n", 100);                    
    printf("\nNew balance: %.2f\n\n", player->balance);
    return;
}

// Supporting functions
void shuffle_deck (void) { 
    int suit, face;
    memset(deck, 0, sizeof(deck)); // Clear deck
    for (int card = 1; card <= CARDS;) {
        suit = rand() % SUITS;
        face = rand() % FACES;
        if (deck[suit][face] == 0) {
            deck[suit][face] = card++;
        }
        else continue;
    }
}

void deal_card (int hand[11], int current_card) {
    char card_name[50];
    int suit;
    int face;
    int exit = 0;
    for (suit = 0; suit < SUITS; suit++) {
        if (exit) break;
        for (face = 0; face < FACES; face++) {
            if (deck[suit][face] == current_card) {
                strcpy(card_name, *(faces + face));
                strcat(card_name, " of ");
                strcat(card_name, *(suits + suit));
                strcat(card_name, "\n");
                slow_print(card_name, 200);
                exit = 1;
                break;
            }
        }
    }
    
    // Adds number of each card to a hand
    switch (face) {
        case 0: hand[1]++;
        break;
        case 1: hand[2]++;
        break;
        case 2: hand[3]++;
        break;
        case 3: hand[4]++;
        break;
        case 4: hand[5]++;
        break;
        case 5: hand[6]++;
        break;
        case 6: hand[7]++;
        break;
        case 7: hand[8]++;
        break;
        case 8: hand[9]++;
        break;
        case 9: hand[10]++;
        break;
        case 10: hand[10]++;
        break;
        case 11: hand[10]++;
        break;
        case 12: hand[10]++;
        break;
    }
}

int evaluate_blackjack (int hand[11]) {
    int total = 0;
    for (int i = 2; i <= 10; i++) { // Exclude Ace
        total += hand[i] * i;
    }

    if (total + hand[1] * 11 <= 21) total += hand[1] * 11; // Ace as 11
    else if (total + 11 - 1 + hand[1] <= 21) total = total + 10 + hand[1]; // 1 Ace as 11 and others as 1
    else total += hand[1]; // All Ace as 1

    return total;
}

int evaluate_baccarat (int hand[11]) {
    int total = 0;
    for (int i = 1; i <= 9; i++) { // Exclude 10, J, Q, K
        total += hand[i] * i;
    }
    return total % 10;
}

int evaluate_slots (wchar_t slot_machine[3][3]){
    const wchar_t slot_image[3] = {L'❌', L'💵', L'💎'}; 
    int small_prize = 0;

    // Check horizontal
    for (int i = 0; i < 3; i++) {
        if ((slot_machine[i][0] == slot_image[2]) && (slot_machine[i][1] == slot_image[2]) && (slot_machine[i][2] == slot_image[2])) return 2;
        if ((slot_machine[i][0] == slot_image[1]) && (slot_machine[i][1] == slot_image[1]) && (slot_machine[i][2] == slot_image[1])) small_prize++;
    }

    // Check vertical
    for (int j = 0; j < 3; j++) {
        if ((slot_machine[0][j] == slot_image[2]) && (slot_machine[1][j] == slot_image[2]) && (slot_machine[2][j] == slot_image[2])) return 2;
        if ((slot_machine[0][j] == slot_image[1]) && (slot_machine[1][j] == slot_image[1]) && (slot_machine[2][j] == slot_image[1])) small_prize++;
    }

    // Check diagonal
    if ((slot_machine[0][0] == slot_image[2]) && (slot_machine[1][1] == slot_image[2]) && (slot_machine[2][2] == slot_image[2])) return 2;
    if ((slot_machine[0][2] == slot_image[2]) && (slot_machine[1][1] == slot_image[2]) && (slot_machine[2][0] == slot_image[2])) return 2;
    if ((slot_machine[0][0] == slot_image[1]) && (slot_machine[1][1] == slot_image[1]) && (slot_machine[2][2] == slot_image[1])) small_prize++;
    if ((slot_machine[0][2] == slot_image[1]) && (slot_machine[1][1] == slot_image[1]) && (slot_machine[2][0] == slot_image[1])) small_prize++;

    if (small_prize) return 1; // Small prize
    return 0; // Lose
}


float place_bet (player_info *player) {
    char input [50];
    float money = 0;
    float bet = 0;
    while (1) {
        printf("\nEnter amount to bet (0 to exit): ");
        standard_input (input, sizeof(input));
        if (is_empty (input)) continue;
        if (!is_float (input)) continue;
        money = atof (input);
        if (money == 0) break;
        if (money < 10) {
            puts("Minimum bet is 10");
            continue;
        }
        if (money > player->balance) {
            puts("Insufficient balance");
            printf("Current balance: %.2f\n", player->balance);
            continue;
        }
        if (!confirm()) continue;
        bet = money;
        player->balance -= bet;
        break;
    }
    return bet;   
}

void horse_move (int *horsePtr, char horse_number) {
    int move_type = rand() % 3;
    switch(move_type){
        case 0: 
            puts("Walks");
            *horsePtr += 1;
            if(*horsePtr > RACETRACK) *horsePtr = RACETRACK;
            break;
        case 1:
            puts("Jogs");
            *horsePtr += 3;
            if(*horsePtr > RACETRACK) *horsePtr = RACETRACK;
            break;
        case 2:
            puts("Sprints");
            *horsePtr += 5;
            if(*horsePtr > RACETRACK) *horsePtr = RACETRACK;
            break;
    }

    for (int position = 1; position <= RACETRACK; position++){
        if (position == *horsePtr) {
            printf("%c ", horse_number);
        }
        else {
            slow_print("_ ", 20);        
        }
    }
    puts("");
}

// Visual effects
void set_locale (void) {
    // Windows-specific: Enable UTF-8 encoding in console
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // For Windows
    #endif

    setlocale(LC_ALL, "en_US.UTF-8"); // Set program to US english conventions with UTF-8 encoding
}

void delay_ms(int milliseconds) {
    #ifdef _WIN32
        Sleep(milliseconds); // Windows-specific
    #else
        usleep(milliseconds * 1000); // Linux/Unix: Convert microseconds to milliseconds 
    #endif
}

void slow_print (char string[], int milliseconds) {
    for (int i = 0; i < strlen(string); i++) {
        fflush(stdout);
        delay_ms(milliseconds);
        printf("%c", string[i]);
    }
}

void display_profile (player_info *player) {
    printf("\nName: %s\n"
            "Age: %d\n"
            "Balance: %.2f\n", player->name, player->age, player->balance);
}

// Input && Validation functions
void prompt(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF); // Always clear the input buffer
}

void clear_buffer(void){
    while(getchar() != '\n');
}

void standard_input(char input[], size_t size) {
    fgets(input, size, stdin); // Directly read into the passed array
    if (!strchr(input, '\n')) { // Check if the newline was not present (overflow occurred)
        clear_buffer(); // Clear the buffer
    }
    else {
        input[strcspn(input, "\n")] = '\0'; // Remove newline character if present
    }
}

void get_info (player_info *player) {
    char input[50] = "\0";
    int age;

    while (1) { // Get name
        printf("Please enter your username: ");
        standard_input (input, sizeof (input) );
        if (is_empty (input) ) continue;
        if (strlen (input) > 30) {
            puts("Please enter a shorter username (max 30 characters)");
            continue;
        }
        strcpy (player->name, input);
        break;
    }

    while (1) { // Get age
        printf("Please enter your age: ");
        standard_input (input, sizeof (input) );
        if (is_empty (input)) continue;
        if (!is_digit (input)) continue;
        if (is_zero (input)) continue;
        age = atoi (input);
        if (age > 130) {
            puts("Please enter a realistic age.");
            continue;
        }
        if (age < 21) {
            puts("You are below legal age of gambling.");
            exit(0); // Exit program
        }
        player->age = age;
        break;
    }

    while (1) { // Get deposit money
        printf("Please enter amount to deposit: ");
        standard_input (input, sizeof(input));
        if (is_empty (input)) continue;
        if (!is_float (input)) continue;
        player->balance = atof (input);
        break;
    }
}

void manage_balance (player_info *player) {
    char input[5] = "\0";
    int choice = -1;

    while (choice != 2) {
        slow_print ("\n\n=====MANAGE BALANCE=====\n\n", 50);
        puts ("Enter 0 to deposit money");
        puts ("Enter 1 to withdraw money");
        puts ("Enter 2 to exit manage balance");
        puts ("Please enter your choice: ");

        // Get input
        standard_input(input, sizeof(input));
        // Check if input is empty
        if (is_empty(input)) continue; // Get input again
        // Check if input is a number
        if (!is_digit(input)) continue; // Get input again
        // Check if a zero is followed by another number
        if (is_zero(input)) continue; // Get input again
        
        choice = atoi(input); // Convert input to number

        switch (choice) {
            default: puts("Invalid choice. Please enter a number between 0-2.");
                break;
            case 0: 
                while (1) { // Get deposit money
                        printf("Current balance: %.2f\n", player->balance);
                        printf("Please enter amount to deposit: ");
                        standard_input (input, sizeof(input));
                        if (is_empty (input)) continue;
                        if (!is_float (input)) continue;
                        player->balance += atof (input);
                        printf("New balance: %.2f\n", player->balance);
                        break;
                }
                break;
            case 1: 
                while (1) { // Withdraw money
                        printf("Current balance: %.2f\n", player->balance);
                        printf("Please enter amount to withdraw: ");
                        standard_input (input, sizeof(input));
                        if (is_empty (input)) continue;
                        if (!is_float (input)) continue;
                        if (player->balance < atof (input)) {
                            puts("Insufficient amount to withdraw");
                            continue;
                        }
                        player->balance -= atof (input);
                        printf("New balance: %.2f\n", player->balance);
                        break;
                }
                break;
            case 2: slow_print("~Exiting manage balance~\n", 100);
                break;
        }
    }
}

int confirm (void) {
    char confirm[5] = "\0"; // Confirmation to place bet
    puts("Are you sure you want to place your bet?");
    while(strcmp(confirm, "yes") != 0 && strcmp(confirm, "no") != 0) {
        printf("Enter yes or no: ");
        standard_input (confirm, sizeof(confirm));
        if (is_empty(confirm)) continue; // Get confirmation again
        for(int i = 0; i < strlen(confirm); i++){
            confirm[i] = tolower (confirm[i]); // Convert to lower case
        }
    }
    if (strcmp(confirm, "yes") == 0) return 1; // Confirmation success
    else puts("Cancelling bet");
    return 0;// Confirmation failed   
}

int is_digit (char input[]) {
    for (int i = 0; i < strlen(input); i++) {
        if (!isdigit(input[i])) {
            puts("Invalid input. Input must only contain digits.");
            return 0; // Test failed
        }
    }
    return 1; // Test pass
}

int is_float (char input[]) {
    char *endptr;

    // Attempt to convert the string to a float
    strtof(input, &endptr);

    while (*endptr != '\0') { // If after endptr contains non-space char, its invalid
        if (*endptr != '\0' && !isspace(*endptr)) {
            puts("Please enter a valid amount.");
            return 0; // Invalid float
        }   
        endptr++;
    }

    return 1; // Valid float
}

int is_empty (char input[]) {
    if (strlen(input) == 0) {
        puts("Invalid input. Input can not be empty.");
        return 1; // Empty, return 1
    }
    return 0; // Not empty, return 0
}

int is_zero (char input[]) {
    if (input[0] == '0' && input[1] != '\0') {
        puts("0 can not have following numbers after it."); // If input starts with 0 like 01, 03, get input again
        return 1;
    }
    return 0;
}
