/*
    Name: Justin Tadlock
    Date: 12/03/2012
    Course: COP3502
    Section Number: C001
    Program #06:
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct game
{
    char name[30];
    struct game *next;
    //struct game ** cabinet
};

//Hash Functions
int hash(int key, int tableSize);
int createHashKey(char str[30]);
void freeLinkedList(struct game * root);
int numGamesChain(struct game *node);
void freeMemory(struct game** cabinet);
void queryCabinet(struct game ** cabinet);
struct game **loadCabinet(struct game ** cabinet);
int foundGame(struct game * node, char name[30]);
struct game *addGameNode(struct game *tempGame, struct game *listHead);

int main()
{
        struct game **cabinet;

        //loads up the game cabinet to use for the query
        cabinet = loadCabinet(cabinet);

        //uses the query file to check if the games are within the cabinet
        queryCabinet(cabinet);

        //Freeing the cabinet pointer
        freeMemory(cabinet);
        free(cabinet);
        return 0;
}

/*
    Pre-Conditions: Takes in the 2D game structure array called cabinet.
    Post-Conditions: Adds the games from the gamelist.txt file to the 2D cabinet array
                                        By creating hashkeys and inserting the games into the array based on their
                                        hash keys using separate chaining collision resolution.
                                        Returns the modified 2D cabinet array to main.
*/
struct game ** loadCabinet(struct game ** cabinet)
{
    FILE * gameFile = fopen("gamelist.txt", "r");
    struct game tempGame; //Used as a temporary place holder for information when adding nodes to the linked lists

    //Initializing variables
    int numGames = 0; //Number of games to be added to the cabinet
    int hashTableSize = 0; //The table size of the hash table (same as numGames)
    int hashKey = 0; //Used when creating hash keys based on the names of the games from the gamelist.txt file
    int i;

    //Gathering information from the gameslist.txt file.
    fscanf(gameFile, "%d", &numGames);
    hashTableSize = numGames; //Setting the hashtable size equal to the number of games
    cabinet = (struct game**)malloc(hashTableSize*sizeof(struct game*)); //Dynamically allocating memory for the cabinet array

    //Initializing the linked-lists to NULL
    for(i = 0; i < hashTableSize; i++)
    {
        cabinet[i] = NULL;
    }

    //This loop is meant to add the games to the 2D array by using hash keys of the game names
    for(i = 0; i < numGames; i++)
    {
        //Reading the name of the game into the temporary game pointer
        fscanf(gameFile, "%s", tempGame.name);

        //Creating the hashkey for inserting the current tempGame into the 2D array
        hashKey = createHashKey(tempGame.name); //Creates an integer based on the game's name
        hashKey = hash(hashKey, hashTableSize); //Hashes an insertion point based on the table size

        //Inserting the game in the 2D array based on the generated hashkey and replacing the head of the linked list if needed (separate chaining collision resolution).
        cabinet[hashKey] = addGameNode(&tempGame, cabinet[hashKey]);
    }

    fclose(gameFile); //closing the file

    return cabinet; //returning the modified 2D cabinet array to main so that information isn't lost.
}

/*
    Pre-Conditions: Takes in a string as the game's name named str.
    Post-Conditions: returns an integer based on the sum of ascii values within the game's name.
*/
int createHashKey(char str[30])
{
    int key = 0; //Sum of ascii values
    int i = 0;
    int strLen = strlen(str); // Getting the length of the game's name.

    //Creating a sum of the ascii values from the game's name.
    for(i = 0; i < strLen; i++)
    {
        key += (int)str[i];
    }

    return key; //Returning the sum as a hash key.
}

/*
    Pre-Conditions: takes in a hash key and the table size of the hash table.
    Post-Conditions: returns an insertion point for the hash table.
*/
int hash(int key, int tableSize)
{
    return key % tableSize;
}

/*
    Pre-Conditions: Takes in two game structure pointers.
    Post-Conditions: Creates a temp node that copies the new game's information, tempGame, then replaces the head
                                        of the linked list with temp and assigns the previous head to temp's next pointer.
*/
struct game *addGameNode(struct game *tempGame, struct game * listHead)
{
    //Creating a new node to replace the previous head node.
    struct game *temp = (struct game*)malloc(sizeof(struct game));

    strcpy(temp->name, tempGame->name); //Copying the new game's information to the temp node
    temp->next = listHead; //Assigning the previous head as temp's next node

    return temp; //Returning the new head of the list.
}

/*
    Pre-Conditions: Takes in the 2D array that represents the game cabinet
    Post-Conditions: Prints information to a queryresults.out file using the query.txt input file
                                        To query the cabinet to determine what games are within the cabinet or not.
*/
void queryCabinet (struct game ** cabinet)
{
    //File pointers
    FILE * queryFile = fopen("query.txt", "r");
    FILE * outFile     = fopen("queryresults.out", "w");

    //Getting the hash table size
    int hashTableSize = 0;
    FILE * getTableSize = fopen("gamelist.txt", "r");
        fscanf(getTableSize, "%d", &hashTableSize);
    fclose(getTableSize);

    //Initializing Variables
    int numQueries;
    int i,j;
    int type; //Used to define which query to perform
    int hashKey; //Used for creating a hashkey for querying the cabinet 2D array
    char name[30]; //Game's Name

    fscanf(queryFile, "%d", &numQueries); // Getting the number of queries to perform

    //Getting the types of queries and the game's name to perform a query on the cabinet array.
    for(i = 0; i < numQueries; i++)
    {
        //Reading the information from the query.txt file.
        fscanf(queryFile, "%d", &type);
        fscanf(queryFile, "%s", name);

        //Type 1 Query: Checks to see if the game is found within the cabinet.
        if(type == 1)
        {
            int found = 0; //Checks to see if a game was found within the cabinet

            //Checking the 2D array linked lists to see if the game is within any of the linked lists.
            for(j = 0; j < hashTableSize; j++)
            {
                if(foundGame(cabinet[j],name) == 1) //If the game was found, print out to the file that it was found
                {
                    fprintf(outFile, "Query #%d: %s was found in location %d.\n\n", i+1, name, j);
                    found = 1;
                }
            }
            if(found == 0) //If the game wasn't found within the cabinet, print that it was not in the cabinet to the out file.
            {
                fprintf(outFile, "Query #%d: %s was not found in the game cabinet.\n\n", i+1, name);
            }
        }
        //Type 2 Query: Calculates the hashKey of the given name and then prints the number of games stored in that linked list.
        else if(type == 2)
        {
            //Creating the hash key
            hashKey = createHashKey(name); //Creates an integer based on the game's name
            hashKey = hash(hashKey, hashTableSize); //Hashes an insertion point based on the table size

            //NumGames calculates the number of games within a linked list
            int numGames = numGamesChain(cabinet[hashKey]);

            //Formatting based on the number of games within the linked list
            if(numGames > 1)
            {
                fprintf(outFile, "Query #%d: There are %d games stored at location %d.\n\n",  i+1, numGames, hashKey);
            }
            else
            {
                fprintf(outFile, "Query #%d: There are %d games at location %d.\n\n",  i+1, numGames, hashKey);
            }
        }
    }

    //Closing the files that are currently open.
    fclose(queryFile);
    fclose(outFile);
}

/*
    Pre-Conditions: Takes in the root node and the name of the game to search for
    Post-Conditions: Returns 1 or 0, 1 if the game was found, 0 otherwise.
*/
int foundGame(struct game * node, char name[30])
{
     if(node == NULL)
    {
        return 0;
    }
    if( strcmp(node->name, name) == 0) //Game found!
    {
        return 1;
    }
    else if(node->next != NULL)
    {
        return foundGame(node->next, name);
    }
}

/*
    Pre-Conditions: Takes in a game structure pointer
    Post-Conditions: Returns the number of games in the current linked list
*/
int numGamesChain(struct game *node)
{
    if(node == NULL)
    {
        return 0;
    }
    else
    {
        return 1 + numGamesChain(node->next);
    }
}

/*
    Pre-Conditions: Takes in the 2D array for the game cabinet
    Post-Conditions: Frees all the nodes of the 2D array
*/
void freeMemory(struct game ** cabinet)
{
    FILE * tableSize = fopen("gamelist.txt", "r");
    int size, i;
    fscanf(tableSize, "%d", &size);

    for(i = 0; i < size; i++)
    {
        freeLinkedList(cabinet[i]);
    }
}

/*
    Pre-Conditions: Takes in the head of each linked list
    Post-Conditions: Frees all the nodes within  the linked list
*/
void freeLinkedList(struct game * root)
{
    if(root == NULL)
    {
        return;
    }
    else
    {
        freeLinkedList(root->next);
    }

    free(root);
}
