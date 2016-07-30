/*
    Name: Justin Tadlock
    Date: 9/21/2012
    Course: COP3502
    Section Number: C001

    Program #02:

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct property
{
    char name[30];
    int user;
    int num_houses;
    int num_hotels;
    struct property * next;
};

//convenient functions
int askNumPlayers();
int printMenu(int player);
int askBuild();
int checkOwner(int player, char * tempName,  struct property * list);
void printSale(struct property * tempNode);
void cont();
struct property * freeMemory(struct property * list);

//main functions
struct property * buyProperty(int player, struct property * list);
struct property * improveProperty(int player, struct property * list);
struct property * sellProperty(int player, struct property * list);
void viewProperties(struct property * list);
void endTurn(int * player, int numPlayers);
void endGame(int * isPlaying, struct property * list);

int main()
{
    int isPlaying = 1, player = 1, numPlayers = 0;
    struct property * properties = NULL;

    numPlayers = askNumPlayers();

    while(isPlaying == 1)
    {
        //switch for executing different functions based on user's choice
        switch(printMenu(player))
        {
            case 1:
                properties = buyProperty(player, properties);
                break;
            case 2:
                properties = improveProperty(player, properties);
                break;
            case 3:
                properties = sellProperty(player, properties);
                break;
            case 4:
                viewProperties(properties);
                break;
            case 5:
                endTurn(&player, numPlayers);
                break;
            case 6:
                endGame(&isPlaying, properties);
                break;
            default:
                printf("Error! Please enter a number between 1 and 6\n");
                cont();
                break;
        }
    }

    properties = freeMemory(properties);

    return 0;
}

/*
    Used for "press enter to continue..." messages
*/
void cont()
{
    printf("Press enter to continue...\n");
    getchar();
}

/*
    Asking the user for the number of players that will play between 1 and 6
    Returns the number of players to main.
*/
int askNumPlayers()
{
    int numPlayers = 0;

    while(numPlayers < 1 || numPlayers > 6) // checker for correct # of players
    {
        printf("How many players are there (1-6)? ");
        scanf("%d", &numPlayers);
        getchar(); //clearing \n out of buffer

        if(numPlayers < 1 || numPlayers > 6)
        {
            printf("Error! Please enter a number between 1 and 6.\n");
            cont();
        }
    }

    return numPlayers;
}

/*
    Printing a menu for the user and returning the choice to the main for the main's switch
    Takes in the player number and prints a menu for what that player would like to do
    Returns the choice of that player to main.
*/
int printMenu(int player)
{
    int choice;

    printf("\nPlayer %d, what would you like to do?\n", player);
    printf("1) Buy Property\n");
    printf("2) Improve Property\n");
    printf("3) Sell Property\n");
    printf("4) View Properties\n");
    printf("5) End Turn\n");
    printf("6) End Game\n");
    printf("Choice: ");

    scanf("%d", &choice);
    getchar(); //clearing buffer

    return choice;
}

/*
    Ask the user how many houses/hotels to build on the property and return the amount entered by the user.
    Returns the number of structures that the player would like to build.
*/
int askBuild()
{
    int amount = -1;

    while(amount < 0)
    {
        printf("How many would you like to build? ");
        scanf("%d", &amount);
        getchar(); //clearing buffer

        if(amount < 0)
        {
            printf("Please enter a positive number.\n");
            cont();
        }
    }

    return amount;
}

/*
    Printing the value of the property sale
    Houses = 25 ea.
    Hotels = 50 ea.
    property base = 100
*/
void printSale(struct property * tempNode)
{
    int price = 100 + ( tempNode->num_hotels * 50 ) + (tempNode->num_houses * 25 );

    printf("You sold %s for $%.2f!\n", tempNode->name, (float)price);
    cont();
}

/*
    Recursive function to remove all of the nodes within a list
    Takes in the list of nodes
    returns the freed pointer.
*/
struct property * freeMemory(struct property * list)
{
    if(list == NULL)
    {
        return NULL;
    }

    if(list->next != NULL)
    {
        list->next = freeMemory(list->next);
    }

    free(list);
    return NULL;
}

/*
    Ask user for the name of the property to buy.
    Compares name of property with properties of other players.
    Prints a message if another player (or the player buying) already owns that property
    If it's not owned, player x buys the property and it is initialized to 0 houses and hotels
    Takes in the current player number and the list of nodes
    Returns the node that the player wishes to buy and adds it to the list of nodes.
*/
struct property * buyProperty(int player, struct property * list)
{
    char tempName[30];
    struct property * tempList = list;
    struct property * temp;

    printf("What's the name of the property you wish to buy?\n");
    scanf("%s", tempName);
    getchar(); //clearing buffer

    //initializing temp node values
    temp = (struct property*)malloc(sizeof(struct property));
    temp->num_houses = 0;
    temp->num_hotels = 0;
    temp->next = NULL;

    if(list == NULL) //creating a new head for the list
    {
        strcpy(temp->name, tempName);
        temp->user = player;

        return temp;
    }

    do
    {
        //Checking for the property being owned by other players
        if(strcmp(tempList->name, tempName) == 0 && tempList->user == player)
        {
            printf("You already own that property!\n");
            cont();

            free(temp);
            return list;
        }
        //Checking for current player owning the property entered
        else if(strcmp(tempList->name, tempName) == 0 && tempList->user != player)
        {
            printf("That property is already owned by Player %d\n", tempList->user);
            cont();

            free(temp);
            return list;
        }

        if(tempList->next != NULL)
        {
            tempList = tempList->next;
        }
    }while(tempList->next != NULL); //checking owning of the property

    //adding the new node to the list of properties
    strcpy(temp->name, tempName);
    temp->user = player;

    tempList->next = temp;

    return list;
}

/*
    Ask the user for the property to improve
    The player must own the property, else print a message stating that they do not own that property
    Ask if the user wants to build houses or hotels. Only one type at a time.
    Ask the number of houses/hotels they want to buy.
    Takes in the current player and the list of nodes
    Returns the head of the list of nodes.
*/
struct property * improveProperty(int player, struct property * list)
{
    struct property * tempList = list;
    char tempName[30];
    int buildNum, owned = 0; //owned is a boolean flag to see if the property is owned by current player

    printf("What's the name of the property you wish to improve?\n");
    scanf("%s", tempName);

    while(tempList != NULL)
    {
        if(strcmp(tempList->name,tempName) == 0 && tempList->user == player)
        {
            buildNum = 0;
            owned = 1;
            while(buildNum == 0) //checker for input to build houses/hotels or nothing
            {
                printf("Choose one of the following:\n");
                printf("1) Build a house\n");
                printf("2) Build a hotel\n");
                printf("3) End\n");
                scanf("%d", &buildNum);

                //switch for asking the number of buildings the player wishes to build
                switch(buildNum)
                {
                    case 1:
                        tempList->num_houses += askBuild();
                        break;
                    case 2:
                        tempList->num_hotels += askBuild();
                        break;
                    case 3:
                        break;
                    default:
                        printf("Error! Please enter a number between 1 and 3.\n");
                        buildNum = 0;
                        break;
                }
            }
        }
        tempList = tempList->next;
    }

    if(owned == 0)
    {
        printf("You do not own that property.\n");
        cont();
    }

    return list;
}

/*
    Ask the user for the name of the property to sell.
    Check if he/she/it owns the property. If he/she/it doesn't own it, print a message that they don't own it.
    Else, sell the property for 100 + numHouses*25 + numHotels*50
        "You sold <Property> for $X.XX!"

    Takes in the player and the list of nodes
    Returns the head of the now modified list of nodes (if a node was deleted)
*/
struct property * sellProperty(int player, struct property * list)
{
    char tempName[30];
    int price, ownedFlag = 0;
    struct property * tempList = list;
    struct property * tempNode = (struct property*)malloc(sizeof(struct property));

    printf("Which property would you like to sell?\n");
    scanf("%s", tempName);
    getchar(); //clearing buffer

    if(list == NULL)//making sure the list isn't empty
    {
        printf("You do not own that property!\n");
        cont();

        free(tempNode);
        return list;
    }

    while(tempList != NULL) //While it's not empty, check if the node already exists.
    {
        //re-linking head node
        if((strcmp(tempList->name, tempName) == 0 && tempList->user == player))
        {
            ownedFlag = 1; //property found within the list of properties

            //linking up nodes appropriately
            tempNode = tempList;
            list = tempList->next;

            printSale(tempNode); //printing the value of selling the property

            free(tempNode);
            return list;
        }
        //linking other nodes
        else if(tempList->next != NULL && strcmp(tempList->next->name, tempName) == 0 && tempList->next->user == player)
        {
            ownedFlag = 1;

            tempNode = tempList->next;
            tempList->next = tempNode->next;

            printSale(tempNode); //printing the value of selling the property

            free(tempNode);
            return list;
        }

        tempList = tempList->next;
    }

    if(ownedFlag == 0) //property not found within the list of properties
    {
        printf("You do not own that property!\n");
        cont();
    }

    free(tempNode);
    return list;
}

/*
    Print the list of peroperties.
    <Property Name>, owned by <Player Number>
    Takes in the list of nodes to print out
*/
void viewProperties(struct property * list)
{
    struct property * tempList = list;

    printf("Properties list:\n");
    if(list == NULL) //case there aren't any nodes in the list.
    {
        printf("\nNo properties were owned.\n");
    }
    else
    {
        //While the node isn't the end of the list, print the node information
        while(tempList != NULL)
        {
            printf("\nProperty, %s, owned by Player %d\n", tempList->name, tempList->user);

            tempList = tempList->next;
        }
    }

    cont();
}

/*
    Proceed to the next player's turn.
    Takes in the player and the number of players in the game.
*/
void endTurn(int * player, int numPlayers)
{
    if(*player >= 1 && *player < numPlayers)
    {
        *player += 1;
    }
    else
    {
        *player = 1;
    }
}

/*
    If the game ends, print a list of all properties (option 4, view properties) and end the game loop.
    Takes in the boolean for the main playing while loop in main and the list of nodes.
    Changes isPlaying to false if the player utilizes this function and ends the game.
*/
void endGame(int * isPlaying, struct property * list)
{
    viewProperties(list);
    *isPlaying = 0;
}
