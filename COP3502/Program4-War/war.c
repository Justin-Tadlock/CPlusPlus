/*
    Name: Justin Tadlock
    Date: 10/31/2012
    Course: COP3502
    Section Number: C001
    Program #04:
*/
#include <stdio.h>
#include <stdlib.h>

struct card
{
    int value;
    char suit;
};

struct queue
{
    struct card playercards[52];
    int front;
    int numElements;
    int size;
};

struct stack
{
    struct card tablecards[52];
    int top;
};

//Behavior functions
void initQueue(struct queue * player, FILE * file, int stackSize);
void initStack(struct stack * table);

void enQueue(struct queue * player, struct card * tCard);
void enqueueStack(struct stack * table, struct card * tCard);

void deQueue(struct queue * player, struct card * tCard);
void dequeueStack(struct stack * table);

int isQueueEmpty(struct queue * player);
int isStackEmpty(struct stack * table);

/*
    Pre-Conditions:
        Vairables:
            For the queues I used p1Queue and p2Queue for player one and two respectively
            A stack variable labeled table for queueing the cards
            Two temporary cards for store houses for dequeing the player's hands into the table stack
            readFile for reading the player's hands from the games.txt file
            outFile for writing the players' games to the file war.out
            stackSize is for the size of the card stacks (52)
            numberOfGames will store the number of the games to be played from the games.txt file
            counter variable will be used to write the current game to the war.out before each game commences (spelling fail... whewww!)
*/
int main()
{
    struct queue * p1Queue = (struct queue*)malloc(sizeof(struct queue));
    struct queue * p2Queue = (struct queue*)malloc(sizeof(struct queue));
    struct stack * table   = (struct stack*)malloc(sizeof(struct stack));
    struct card  * tCard1  = (struct card* )malloc(sizeof(struct card ));
    struct card  * tCard2  = (struct card* )malloc(sizeof(struct card ));
    FILE * readFile = fopen("games.txt", "r");
    FILE * outFile  = fopen("war.out", "w");
    int stackSize = 52;
    int numberOfGames = 0, counter = 1;
    int winner;

    fscanf(readFile, "%d", &numberOfGames);

    while ( counter <= numberOfGames ) //main loop
    {
        //initializing the queues for player one and two and the table stack
        initQueue(p1Queue, readFile, stackSize);
        initQueue(p2Queue, readFile, stackSize);
        initStack(table);

        //writing the current game number to the war.out file
        fprintf(outFile, "Game #%d\n", counter);

        /*
            Internal breaking conditionals are used to break out of the game loops. So the condition was arbitrary
            Through each while loop, the game will check to see which player has run out of cards and declare the winner appropriately and print the winner to the war.out file
        */
        while( 1 )
        {
            //conditionals for checking for winners and to break out of the game loop and start a new game.
            if(p1Queue->numElements == 0)
            {
                fprintf(outFile, "Player 2 wins!\n");
                break;
            }
            else if(p2Queue->numElements == 0)
            {
                fprintf(outFile, "Player 1 wins!\n");
                break;
            }
            else if(p1Queue->numElements == 0 && p2Queue->numElements == 0)
            {
                fprintf(outFile, "The game was a tie.\n");
                break;
            }


            /*
                dequeueing the cards from each player's hand into the table stack pile
            */
            deQueue(p1Queue, tCard1);
            enqueueStack(table, tCard1);

            deQueue(p2Queue, tCard2);
            enqueueStack(table, tCard2);

            //printing the cards to be played out to the war.out file before they are compared for which player wins the "duel"
            fprintf(outFile, "%d%c \t %d%c \n", tCard1->value, tCard1->suit, tCard2->value, tCard2->suit);


            winner = 0; //resetting winner to avoid loop wholes. Don't want computers "hacking", do we?

            //series of conditionals checking for who wins the battle.
            if(tCard1->value > tCard2->value) //if player one won the "duel"
            {
                winner = 1;
            }
            if(tCard1->value == 1 && tCard2->value != 1) // checking for aces, since ther are trump cards
            {
                winner = 1;
            }
            if(tCard2->value > tCard1->value) // if player two won the "duel"
            {
                winner = 2;
            }
            if(tCard2->value == 1 && tCard1->value != 1) // again,checking for aces, whewt!
            {
                winner = 2;
            }

            if(tCard1->value == tCard2->value) // if there's a war, discard!
            {
                /*
                    this section keeps track of the wars that go on.
                    If a war is found, this portions "discards" a hand and waits for the next loop to decide the winner
                    of the war.
                */
                fprintf(outFile, "WAR\n");

                //dequeueing the cards from each player's hands into the table stack
                deQueue(p1Queue, tCard1);
                enqueueStack(table, tCard1);

                deQueue(p2Queue,tCard2);
                enqueueStack(table, tCard2);

                //printing the discarded cards to the war.out file as well.
                fprintf(outFile, "%d%c \t %d%c \n", tCard1->value, tCard1->suit, tCard2->value, tCard2->suit);
            }
            //checkers to see if there were any winners in the card flips
            //these while loops will continue to loop and enque more and more cards to the winning player
            //  while there is more cards on the "table"
            if(winner == 1)
            {
                while( isStackEmpty(table) == 0)
                {
                    enQueue(p1Queue, &table->tablecards[table->top]);
                    dequeueStack(table);
                }
            }
            if (winner == 2)
            {
                while( isStackEmpty(table) == 0 ) // using the isEmpty function just in case I'd get in trouble for not using it.
                {
                    enQueue(p2Queue, &table->tablecards[table->top]);
                    dequeueStack(table);
                }
            }
        }
        counter++; // keeping track of the game number for us.
    }

    //time for good practice, closing files and freeing dynamically allocated memory!
    fclose(readFile);
    fclose(outFile);

    free(p1Queue);
    free(p2Queue);
    free(table);
    free(tCard1);
    free(tCard2);

    return 0;
}

/*
    Pre-Conditions:
        Taking in a player queue, games.txt file pointer to get the hands for the queues, and the card deck stack size
    Post-Conditions:
        Initializing the player queue with the appropriate starting values.
        Front being the head of the array (in case circular aray will be implemented later on)
        numElements being the number of elements currently in the array, used for tracking the number of cards the player has
        size is used to make sure that the player cannot have more than the deck size, we don't need magicians in this game! D:
        the tCard is used as a placeholder object and used to enqueue the cards onto the player's deck.
*/
void initQueue(struct queue * player, FILE * file, int stackSize)
{
    int i;
    struct card * tCard = (struct card*)malloc(sizeof(struct card));

    //initializing variables
    player->front = 0;
    player->numElements = 0;
    player->size = stackSize;

    //gathering the cards from the games.txt file and enqueueing them to the player queue
    //stackSize / 2 is simply used to get 26 cards for each player's starting hand
    for(i = 0; i < stackSize / 2; i++)
    {
        fscanf(file, "%d %c ", &tCard->value, &tCard->suit);
        enQueue(player, tCard); // add the new card to the end of the player's queue
    }

    //free the dynamic memory
    free(tCard);
}

/*
    Pre-Conditions:
        Takes in the table stack
    Post-Conditions:
        initializes the top variable to -1 to indicate it's "empty" since 0 would be the beginning of the tablecards array
*/
void initStack(struct stack * table)
{
    if(table == NULL)
    {
        printf("Cannot find table stack.\n");
        return;
    }

    table->top = -1;
}

/*
    Pre-Conditions:
        takes in player queue and a placeholder card for assigning the queue's cards to the new card being added
    Pose-Conditions:
        "adds" a slot for the new card within the player's queue and assigns the new slot to the placeholder card
        also increases the number of elements within the array to affirm the new card was added to the queue
*/
void enQueue(struct queue * player, struct card * tCard)
{
    if(player == NULL)
    {
        printf("Cannot find player queue.\n");
        return;
    }

    if(player->numElements < player->size) //well, you see, if you're lower than the 52 cards in a deck, you can win more cards!
    {
        //these two lines here are resetting the old values to the new card.
        player->playercards[player->numElements].value = tCard->value;
        player->playercards[player->numElements].suit  = tCard->suit;
        player->numElements += 1; //increasing the number of elements for the array
    }
    else //yup, this queue is full, ain't nothin' you can add to it.
    {
        printf("Unable to enqueue into a full queue.\n");
        return;
    }
}

/*
    Pre-Conditions: (so many functions! T_T)
        takes in the table stack and a card object
    Post-Conditions:
        adds a slot to the table and assigns the new slot to the card object, adding a card to the table stack
*/
void enqueueStack(struct stack * table, struct card * tCard)
{
    if(table == NULL || tCard == NULL)
    {
        printf("Cannot find table stack or card.\n");
        return;
    }

    if(table->top < 52)
    {
        table->top += 1; //We have space for one more card!

        //and these two lines assign the old card to the new card that was passed in! Yup, it's magic.
        table->tablecards[table->top].value = tCard->value;
        table->tablecards[table->top].suit = tCard->suit;
    }
}

/*
    Pre-Conditions:
        takes in a player queue and a card object
    Post-Conditions:
        Assigns the card object to the head card and then shifts all of the cards to the left by one.
        This "pops" the front card off and the card object pased in is then used to add that card to the table stack in main. Pass by reference epicness!
*/
void deQueue(struct queue * player, struct card * tCard)
{
    int i;

    if(player == NULL || tCard == NULL)
    {
        printf("Cannot find player or card.\n");
        return;
    }

    if(player->numElements > 0) //so long as it's not zero, we can dequeue more cards! Whewt!
    {
        //setting the front card equal to the pointer card to send back to main for enqueueing into the stack!
        tCard->value = player->playercards[player->front].value;
        tCard->suit  = player->playercards[player->front].suit;

        //shifting all of the cards in the queue to the leeeeeeft!
        for(i = 0; i < player->numElements-1; i++) //had to make sure that there wasn't an out of bounds error, hence, numElements -1;
        {
            player->playercards[i] = player->playercards[i+1]; //because of this equation!
        }
        player->numElements -= 1; //once all of the cards have been shifted, subtract the number of elements by one indicating that the removal was successful.
    }
    else //And if the queue is empty, don't do anything! Tell the player to Shoo!
    {
        printf("Unable to dequeue from an empty queue.\n");
        return;
    }
}

/*
    Pre-Conditions:
        takes in a table stack
    Post-Conditions:
        Simply moves the variable, top, as the index of the array down one to "pop" or "remove" a card from the table stack
*/
void dequeueStack(struct stack * table)
{
    if(table == NULL)
    {
        printf("The table stack was not found.\n");
        return;
    }

    if(table->top > -1) // moving the index of the table stack to "remove" the card
    {
        table->top -= 1;
    }
    else //Bah, the stack's empty man! Need a new basket!
    {
        printf("Cannot dequeue from an empty stack.\n");
        return;
    }
}

/*
    Pre-Conditions:
        takes in a player queue
    Post-Conditions:
        returns a value indicating if the queue if empty or not. I find using the numElements directly to be more "controllable" than using this function, however.
*/
int isQueueEmpty(struct queue * player)
{
    if(player == NULL)
    {
        printf("Cannot find the player queue.\n");
        return;
    }

    if(player->numElements == 0) // if the array has not elements. It's Empty! ERMAHGERDZ ERHMPTERH
    {
        return 1;
    }
    else //else it's ... well, it's just not empty!
    {
        return 0;
    }
}

/*
    Pre-Conditions:
        takes in a table stack
    Post-Conditions:
        returns a value indicating if the stack is empty or not. I still think using the top index to be more
        "controllable" and preferred as means to check for being empty or not instead of using this function
*/
int isStackEmpty(struct stack * table)
{
    if(table == NULL)
    {
        printf("Cannot find the table stack.\n");
        return;
    }

    if(table->top >= 0) //if top index is >= 0, the array isn't empty, if it's -1 on the other hand.. hmmmmm..
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
