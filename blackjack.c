#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// define card structure
typedef struct {
    char *suit;
    char *rank;
    int value;
} Card;

// define deck structure
typedef struct {
    Card *cards;
    int numCards;
} Deck;

Deck initializeDeck() {
    Deck deck;
    char *suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
    char *ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace"};
    int values[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11};

    deck.numCards = 52;
    deck.cards = (Card *)malloc(deck.numCards * sizeof(Card));

    int index = 0;
    for (int suit = 0; suit < 4; suit++) {
        for (int rank = 0; rank < 13; rank++) {
            deck.cards[index].suit = suits[suit];
            deck.cards[index].rank = ranks[rank];
            deck.cards[index].value = values[rank];
            index++;
        }
    }

    return deck;
}

void shuffleDeck(Deck *deck) {
    for (int i = deck->numCards - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }
}

Card dealCard(Deck *deck) {
    Card card = deck->cards[deck->numCards - 1];
    deck->numCards--;

    return card;
}

void printCard(Card card) {
    printf("%s of %s\n", card.rank, card.suit);
}

void clearScreen() {
    for (int i = 0; i < 50; ++i) {
        printf("\n");
    }
}

void printHands(Card *playerHand, int playerHandSize, Card *dealerHand, int dealerHandSize, bool showDealerHand) {
    printf("Your hand:\n");
    for (int i = 0; i < playerHandSize; i++) {
        printCard(playerHand[i]);
    }
    printf("\n");

    if (showDealerHand) {
        printf("Dealer's hand:\n");
        for (int i = 0; i < dealerHandSize; i++) {
            printCard(dealerHand[i]);
        }
        printf("\n");
    } else {
        printf("Dealer's hand:\n");
        printf("Hidden Card\n");
        printCard(dealerHand[1]);
        printf("\n");
    }
}

int calculateHandValue(Card *hand, int numCards) {
    int value = 0;
    int numAces = 0;

    for (int i = 0; i < numCards; i++) {
        value += hand[i].value;
        if (strcmp(hand[i].rank, "Ace") == 0) {
            numAces++;
        }
    }

    // adjust for aces
    while (value > 21 && numAces > 0) {
        value -= 10;
        numAces--;
    }

    return value;
}

void playBlackjack() {
    // create deck of cards
    Deck deck = initializeDeck();
    shuffleDeck(&deck);

    // create player and dealer hands
    Card playerHand[10], dealerHand[10];
    int playerHandSize = 0, dealerHandSize = 0;
    int playerScore = 0, dealerScore = 0;

    // deal starting cards
    playerHand[playerHandSize++] = dealCard(&deck);
    dealerHand[dealerHandSize++] = dealCard(&deck);
    playerHand[playerHandSize++] = dealCard(&deck);
    dealerHand[dealerHandSize++] = dealCard(&deck);

    // print starting hands
    clearScreen();
    printHands(playerHand, playerHandSize, dealerHand, dealerHandSize, false);

    // check for blackjack(s)
    playerScore = calculateHandValue(playerHand, 2);
    dealerScore = calculateHandValue(dealerHand, 2);

    if (playerScore == 21 || dealerScore == 21) {
        clearScreen();
        printHands(playerHand, playerHandSize, dealerHand, dealerHandSize, true);

        if (playerScore == 21 && dealerScore == 21) {
            printf("It's a draw!\n");
        } else if (playerScore == 21) {
            printf("Blackjack! You win!\n");
        } else {
            printf("Dealer has Blackjack! Dealer wins!\n");
        }

        printf("-------------------------\n");
        free(deck.cards);
        return;
    }

    // player's turn
    while (1) {
        char choice;

        printf("Enter 'h' to hit or 's' to stand: ");
        scanf(" %c", &choice);

        if (tolower(choice) == 'h') {
            // hit
            clearScreen();

            playerHand[playerHandSize++] = dealCard(&deck);
            printf("You drew:\n");
            printCard(playerHand[playerHandSize - 1]);
            printf("==========\n");

            playerScore = calculateHandValue(playerHand, playerHandSize);

            // check if player has 21
            if (playerScore == 21) {
                printHands(playerHand, playerHandSize, dealerHand, dealerHandSize, true);
                printf("You win!\n");
                printf("-------------------------\n");
                free(deck.cards);
                return;
            }

            // check if player busts
            if (playerScore > 21) {
                printHands(playerHand, playerHandSize, dealerHand, dealerHandSize, true);
                printf("Busted! You lose.\n");
                printf("-------------------------\n");
                free(deck.cards);
                return;
            }

            // print hands after hit
            printHands(playerHand, playerHandSize, dealerHand, dealerHandSize, false);
        } else if (tolower(choice) == 's') {
            // stand
            clearScreen();
            break;
        } else {
            printf("Invalid input. Please enter 'h' or 's'.\n");
        }
    }

    // dealer's turn after player stands
    bool dealerDrew = false;

    printf("\nDealer's turn:\n");
    printf("==========\n");
    printf("Dealer's hidden card: ");
    printCard(dealerHand[0]);

    while (calculateHandValue(dealerHand, dealerHandSize) < 17) {
        dealerDrew = true;
        dealerHand[dealerHandSize++] = dealCard(&deck);
    }

    if (dealerDrew) {
        printf("Dealer drew:\n");
        for (int i = dealerHandSize - 1; i >= 0; i--) {
            printCard(dealerHand[i]);
        }
    }
    printf("-------------------------\n");

    // print final hands
    printHands(playerHand, playerHandSize, dealerHand, dealerHandSize, true);

    // determine winner
    playerScore = calculateHandValue(playerHand, playerHandSize);
    dealerScore = calculateHandValue(dealerHand, dealerHandSize);

    printf("Your score: %d\n", playerScore);
    printf("Dealer's score: %d\n", dealerScore);

    if (playerScore > 21 || (dealerScore <= 21 && dealerScore > playerScore)) {
        printf("Dealer wins!\n");
    } else if (dealerScore > 21 || playerScore > dealerScore) {
        printf("You win!\n");
    } else {
        printf("It's a draw!\n");
    }
    printf("-------------------------\n");

    free(deck.cards);
}

bool playAgain() {
    char playAgain;

    printf("Do you want to play again? (y/n): ");
    scanf(" %c", &playAgain);

    return (tolower(playAgain) == 'y');
}

int main() {
    srand(time(NULL));

    do {
        playBlackjack();
    } while (playAgain());

    return 0;
}
