/*
    Name: Justin Tadlock
    Date: 11/16/2012
    Course: COP3502
    Section Number: C001
    Program #05:
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Structures
//This structure is used to store people's names and favorite games.
struct Person
{
    char name[20];
    char game[20];
};

//This structure is used to create a Binary Search Tree to organize the people's data.
struct Tree
{
    char name[20];
    char game[20];

    struct Tree *left;
    struct Tree *right;
};

//Auxiliary Functions
void freeMemory(struct Tree *bst);
void numPeople(struct Tree *bst, int *people);
struct Tree * createNode(char name[], char game[]);
void printPart2(struct Person *people, FILE *file, int size);
void loadTempTree(struct Tree *bst, struct Person people[], int *index);

//Sorting functions
void sortNames(struct Person people[], int size);
void MergeSort(struct Person *people, int start, int end);
void Merge(struct Person *people, int start, int middle, int end);


//Delete Node Auxiliary Functions
int isLeaf(struct Tree *bst);
int hasLeftNode(struct Tree *bst);
int hasRightNode(struct Tree *bst);
struct Tree * maxName(struct Tree * bst);
struct Tree * findNode(char target[], struct Tree * bst);
struct Tree * parentNode(struct Tree * target, struct Tree * bst);

//Command Functions
void print(struct Tree *bst, FILE *file);
struct Tree * removeName(char name[], struct Tree *bst);
void printGame(char game[], struct Tree *bst, FILE *file);
struct Tree * addName(struct Tree *node, struct Tree *bst);


/*
    Pre-Conditions:
        Reads people's names and favorite games from the party.txt file

    Post-Conditions:
        Uses the read information to store data inside a Binary Search Tree, named bst.
        Prints the information regarding the people's names and favorite games into a file.
*/
int main()
{
    //Opening necessary files
    FILE *readFile = fopen("party.txt", "r");
    FILE *writeFile= fopen("partygames.out", "w");

    //Abstract data types
    struct Tree *bst = NULL;
    struct Tree *temp;

    //Strings
    char command[20];
    char name[20], game[20];

    //Read the first command from the party.txt file
    fscanf(readFile, "%s", command);

    //Finish reading the rest of the file and execute the appropriate commands/operations
    //Using the PART2 command as a termination for the first part of the program.
    //After PART2 is read from the file, part two will be initiated.
    while(strcmp(command, "PART2") != 0)
    {
        //Using chaining if-else-if statements to determine which command needs to be executed.
        //ADD will read the name and favorite game of a person from the file,
        //  create a new node via the temp variable, then add that node to the Binary Search Tree using the bst variable
        if(strcmp(command, "ADD") == 0)
        {
            fscanf(readFile, "%s", name);
            fscanf(readFile, "%s", game);

            temp = createNode(name, game);

            bst = addName(temp, bst);
        }
        //REMOVE will read the name of the person to be removed from the Binary Search Tree from the file.
        // This will execute the very long removeName function to find the person and remove them while keeping the BST intact
        else if (strcmp(command, "REMOVE") == 0)
        {
            fscanf(readFile, "%s", name);

            bst = removeName(name, bst);
        }
        //PRINT will print the names and favorite games of the players within the BST in an inOrder operation.
        else if (strcmp(command, "PRINT") == 0)
        {
            fprintf(writeFile, "PRINT:\n"); //Acts as a header inside the file.
            print(bst, writeFile); //Prints the names and favorite games to the out file
        }
        //PRINTGAME will print a game header and a list of people within the BST that also like that game.
        else if (strcmp(command, "PRINTGAME") == 0)
        {
            fscanf(readFile, "%s", game); //Reading the game name from the file to print information about
            fprintf(writeFile, "\n%s:\n", game); //Printing that as a header in the out file.
            printGame(game, bst, writeFile); //Printing all the names of people who like that game to the out file.
        }

        //Getting the next command from the in file.
        fscanf(readFile, "%s", command);
    }

    //Beginning Part 2 of the program.
    //////////////////////////////////////////////////////////////////////////////////////
    fprintf(writeFile, "\n---PART 2---\n"); // Printing a header in the out file signifying that part 2 of the program has begun.

    struct Person *peopleArray; // Used to store the information from the BST as an array that will be sorted using the MergeSort algorithm

    //nPeople is used to find the number of people within the BST
    int nPeople = 0;

    //arrayIndex is used to add information to the peopleArray while traversing through the BST
    int arrayIndex = 0;

    //This function is used to traverse the BST and count the number of people.
    numPeople(bst, &nPeople);

    //Dynamically allocating the correct size for the array to store the people's information within the BST
    peopleArray = (struct Person*)malloc(nPeople*sizeof(struct Person));

    //This function traverses the BST and assigns the information to peopleArray
    loadTempTree(bst, peopleArray, &arrayIndex);

    //MergeSort is sorting the peopleArray in terms of games they like
    MergeSort(peopleArray, 0, nPeople-1);

    //This function is used to sort the names alphabetically after MergeSort organizes them by game name
    sortNames(peopleArray, nPeople);

    //This function is printing the array information to the out file.
    printPart2(peopleArray, writeFile, nPeople);

    //End Part 2 of the Program
    ///////////////////////////////////////////////////////////////////////////////////////

    //closing all files and freeing memory.
    fclose(readFile);
    fclose(writeFile);
    free(peopleArray);
    freeMemory(bst);

    return 0;
}

/*
    Pre-Conditions:
        Takes in two strings, a person's name and their favorite game
    Post-Conditions:
        Creates and returns a new Tree node that will be added to the BST
*/
struct Tree * createNode(char name[], char game[])
{
    //Creating and allocating enough space for the new node
    struct Tree *node = (struct Tree*)malloc(sizeof(struct Tree));

    //Copying information to the node
    strcpy(node->name, name);
    strcpy(node->game, game);

    //Setting the left and right Pointers (this is a leaf node for the moment)
    node->left = NULL;
    node->right= NULL;

    return node;
}


/*
    Pre-Conditions:
        Takes in a new node to be added to the BST and the BST root
    Post-Conditions:
        Inserts the new node into the appropriate slot within the BST
*/
struct Tree * addName(struct Tree *node, struct Tree *bst)
{
    //Case the root is NULL, the new root is the new node being added
    if(bst == NULL)
    {
        return node;
    }
    else
    {
        //Determining where to insert the new node
        if(strcmp(node->name, bst->name) < 0)//If the new name has less value than the current name, the new node will be placed to the left of the root
        {
            bst->left = addName(node, bst->left);
        }
        else //If the new name has a higher value, then the new node will be placed to the right of the root.
        {
            bst->right = addName(node, bst->right);
        }
    }

    return bst;
}

/*
    Pre-Conditions:
        Takes in the Root of the BST tree and the out file pointer used to print to a file.
    Post-Conditions:
        Prints the people's names and their favorite games from the BST using an in order traversal to the out file.
*/
void print(struct Tree *bst, FILE *file)
{
    //Assuming the BST isn't NULL, we can traverse the tree.
    if(bst != NULL)
    {
        print(bst->left, file); //Check the left branches first.
        fprintf(file, "%s %s\n", bst->name, bst->game); //Print the current root to the file
        print(bst->right, file); //Check the right branches last.
    }
}

/*
    Pre-Conditions:
        Takes in a string for the target game to print information for, and the out file pointer
    Post-Conditions:
        Will print a header as the game name, and a list of people who like that game to the out file. Also uses inOrder traversal of the BST.
*/
void printGame(char game[], struct Tree *bst, FILE *file)
{
    //Assuming the BST tree can be traversed
    if(bst != NULL)
    {
        printGame(game, bst->left, file); //Check the left branches first.
        if(strcmp(bst->game, game) == 0) //Check if the root's game is equal to the target
        {
            fprintf(file, "%s\n", bst->name); //If it is, then print the name of the person to the out file.
        }
        printGame(game, bst->right, file); //Check the right branches last.
    }
}

/*
    Pre-Conditions:
        Takes in the root of the BST and a number variable passed by reference to count the number of people within the BST
    Post-Conditions:
        Uses the people variable that's passed by reference to count the number of poeple within the BST.
*/
void numPeople(struct Tree *bst, int *people)
{
    //Cannot count the number of people within a NULL tree
    if(bst == NULL)
    {
        return;
    }

    //Traverse the left branches
    if(bst->left != NULL)
    {
        numPeople(bst->left, people);
    }
    //Traverse the right branches
    if(bst->right != NULL)
    {
        numPeople(bst->right, people);
    }

    //When you reach the ends of each branch, begin counting the number of people.
    *people += 1;
}

/*
    Pre-Conditions:
        This function takes in the root of the BST, an array to record the information within the BST, and the index to increment among the array elements
    Post-Conditions:
        Loads up the information within the BST into an array. This function uses preOrder traversal
*/
void loadTempTree(struct Tree * bst, struct Person people[], int *index)
{
    //Assume the Tree isn't NULL
    if(bst != NULL)
    {
        //Setting the current root information to the array
        strcpy(people[*index].name, bst->name);
        strcpy(people[*index].game, bst->game);
        *index += 1; //Increment the index to the next element in the array.

        //Traverse through the tree
        loadTempTree(bst->left, people, index); // Traverse the left branches
        loadTempTree(bst->right, people, index);// Traverse the right branches
    }
}

/*
    Pre-Conditions:
        This functions takes in an array that's to be split and sorted, the starting position, and the ending size of the array
    Post-Conditions:
        Splits up the array into halves so that the Merge function can sort appropriately.
*/
void MergeSort(struct Person *people, int start, int end)
{
    int mid; // Finding the half way point in the array indexes

    if(start < end) //If start is less than the size of the array, it can be halved and sorted
    {
        mid = (start+end) / 2; //Sum / 2 = half
        MergeSort(people, start, mid); //Sorting left half (spliting further in halves)
        MergeSort(people, mid+1, end); //Sorting right half (spliting further in halves)
        Merge(people, start, mid+1, end); //Merging the halves together into a sorted array.
    }
}

/*
    Pre-Conditions:
        Takes in the array to be sorted, the starting position, the middle position, and the size of the array
    Post-Conditions:
        Combines the two split arrays into one, sorted array.
*/
void Merge(struct Person people[], int start, int middle, int end)
{
    //length is the size of the merged array.
    //count1 is keeping track of the beginning of the left half of the array
    //count2 is keeping track of the end of the left half of the array and the beginning of the second half of the array
    //mc is the index of the merge array used to keep track of where you are sorting information.
    //cmpGame is comparing the game's alphabetical positioning.
    int i, length, count1, count2, mc, cmpGame;

    //Array that will store the sorted information that the passed in array will be set to.
    struct Person * peopleArray;

    mc = 0; //Initializing the peopleArray index
    count2 = middle; //Initializing the first half end point.
    count1 = start;  //Initializing the first half start point.
    length = (end-start)+1; //Getting the total current size of the array (since we have halved it quite a bit).
    peopleArray = (struct Person*)calloc(length, sizeof(struct Person)); //Creating an array allocated to the size of the current array Length.

    while((count1 < middle) || (count2 <= end)) //Loop that will continue sorting the information until all the information has been sorted
    {
        cmpGame = strcmp(people[count1].game, people[count2].game); //Checks for alphabetical ordering
        if(count2 > end || (count1 < middle && cmpGame < 0)) // Adds the appropriate information to the first half of the array
        {
            peopleArray[mc] = people[count1];
            count1++;
            mc++;
        }
        else //Adds the appropriate information to the second half of the array
        {
            peopleArray[mc] = people[count2];
            count2++;
            mc++;
        }
    }

    //Assigning the unsorted aray elements to the new, sorted elements.
    for(i = start; i <= end; i++)
    {
        people[i] = peopleArray[i-start];
    }

    free(peopleArray); //Freeing the allocated memory.
}

/*
    Pre-Conditions:
        Takes in the merged sorted array and the size of the array.
    Post-Conditions:
        Sorts the array by the people's names while keeping the game alphabetical order intact
*/
void sortNames(struct Person people[], int size)
{
    int i = 0, j = 0; //Counters

    struct Person temp; //Replacement structure for sorting.

    for(i = 0; i < size; i++) //Initial Loop
    {
        for(j = i+1; j < size; j++) // Comparative loop
        {
            if((strcmp(people[j].game, people[i].game) == 0) &&
               (strcmp(people[j].name, people[i].name) < 0)) //Checking to see if the game name is the same and then compares the names alphabetically
            {
                //Swapping the data, making it all sorted.
                temp = people[j];
                people[j] = people[i];
                people[i] = temp;
            }
        }
    }
}

/*
    Pre-Conditions:
        Takes in an array, out file pointer and the size of the array
    Post-Conditions:
        Prints the sorted array data to the out file with a Game header and the list of people who like that game.
*/
void printPart2(struct Person people[], FILE *file, int size)
{
    int i = 0;
    char currGame[20]; // Keeps track of which game that people's names are being listed for.

    strcpy(currGame, people[0].game); //Assigning current game to the first game in the array list
    fprintf(file, "%s\n", currGame);  //Printing the Game header in the out file.

    while(i < size) // Traversing through the array, printing the information to the out file.
    {
        if(strcmp(people[i].game, currGame) == 0) // If the player's favorite game is equal to the currently assigned game, print their name to the out file.
        {
            fprintf(file, "\t%s\n", people[i].name); //Printing the person's name to the out file.
            i++; //Incrementing to the next person
        }
        else //If the current person doesn't like the current game header, then we need to progress to the next game in the array list.
        {
            strcpy(currGame, people[i].game); //Assigning the new game to print people's names for.
            fprintf(file, "%s\n", currGame); //Print the game header to the out file.
        }
    }
}

/*
    Pre-Conditions:
        Takes in the BST root.
    Post-Conditions:
        Traverses through the BST and freeing all the nodes to recover memory used.
*/
void freeMemory(struct Tree *bst)
{
    //Assume the BST isn't NULL
    if(bst != NULL)
    {
        freeMemory(bst->left);  //Traverse the left branches
        freeMemory(bst->right); //Traverse the right branches
        free(bst); //Free the nodes.
    }
}

/*
    Pre-Conditions:
        Takes in the target name to be removed from the tree as well as the BST root.
    Post-Conditions:
        Removes the appropriate node so that the BST's structure stays intact while removing the person from the BST.
*/
struct Tree * removeName(char target[], struct Tree *bst)
{
    //Structures used to delete the target person appropriately
    struct Tree * delNode, *replacementNode;
    struct Tree * saveNode, *parNode;

    char name[20], game[20]; // Temporary storage of node information

    //This function traveres through the BST to find the target person to remove.
    delNode = findNode(target, bst);

    //This function traverses through the BSE to find the parent node of the target node to remove.
    parNode = parentNode(delNode, bst);

    if(isLeaf(delNode)) //Checks to see if the target node is a leaf node.
    {
        if(parNode == NULL)
        {
            free(bst);

            return NULL;
        }
        if(strcmp(delNode->name, parNode->name) < 0) // If the node is to the left of the parent node,
        {
            free(parNode->left);
            parNode->left = NULL;
        }
        else // If the node if to the right of the parent node.
        {
            free(parNode->right);
            parNode->right = NULL;
        }

        return bst;
    }

    if(hasLeftNode(delNode)) // Checking to see if the node has a left child node.
    {
        if(parNode == NULL)
        {
            saveNode = delNode->left;
            free(delNode);

            return saveNode;
        }
        if(strcmp(delNode->name, parNode->name) < 0) //Free the left node
        {
            saveNode = parNode->left;
            parNode->left = parNode->left->left;
            free(saveNode);
        }
        else //Free the right node
        {
            saveNode = parNode->right;
            parNode->right = parNode->right->right;
            free(saveNode);
        }

        return bst;
    }

    if(hasRightNode(delNode)) //Checking to see if the node has a right child node.
    {
        if(parNode == NULL)
        {
            saveNode = delNode->right;
            free(delNode);
            return saveNode;
        }
        if(strcmp(delNode->name, parNode->name) < 0) //Replacing the parent left node with the appropriate child node
        {
            saveNode = parNode->left;
            parNode->left = parNode->left->right;
            free(saveNode);
        }
        else //Replacing the parent right node with the appropriate child node
        {
            saveNode = parNode->right;
            parNode->right = parNode->right->right;
            free(saveNode);
        }

        return bst;
    }

    //Finding the replacement node
    replacementNode = maxName(delNode->left);

    //Storing the replacement node's information
    strcpy(name, replacementNode->name);
    strcpy(game, replacementNode->game);

    //Removing the replacement node
    removeName(replacementNode->name, bst);

    //Overwritting the target node's information with the replacement node's information
    strcpy(delNode->name, name);
    strcpy(delNode->game, game);

    return bst;
}

/*
    Pre-Conditions:
        Takes in the target name to find and the BST root
    Post-Conditions:
        Returns the node of the target name.
*/
struct Tree * findNode(char target[], struct Tree * bst)
{
    //Assume the BST isn't NULL
    if(bst != NULL)
    {
        if(strcmp(target, bst->name) == 0) //Found the target name
        {
            return bst;
        }
        if(strcmp(target,bst->name) < 0) //Traverse through the left branches
        {
            return findNode(target, bst->left);
        }
        else //Traverse through the right branches
            return findNode(target, bst->right);
    }
    else //Name doesn't exist.
        return NULL;
}

/*
    Pre-Conditions:
        Takes in the target node that you want the parent for and the BST root
    Post-Conditions:
        Returns the parent node of the target node you want to delete.
*/
struct Tree * parentNode(struct Tree * target, struct Tree * bst)
{
    //Assuming the BST and the target nodes aren't NULL
    if(bst == NULL || bst == target)
    {
        return NULL;
    }

    if(bst->left == target || bst->right == target) //Found the parent node
    {
        return bst;
    }

    if(strcmp(target->name, bst->name) < 0) //Traverse the left branches
    {
        return parentNode(target, bst->left);
    }

    else if (strcmp(target->name, bst->name) > 0) //Traverse the right branches
    {
        return parentNode(target,bst->right);
    }

    return NULL; //Node doesn't exist.
}

/*
    Pre-Conditions:
        Takes in the BST root
    Post-Conditions:
        Finds the highest alphabetical name in the left branch of the BST. Used for replacing nodes when deleting to keep BST structure from falling apart.
*/
struct Tree * maxName(struct Tree * bst)
{
    if(bst->right == NULL) //Leaf found.
    {
        return bst;
    }
    else //Keep searching for a leaf.
    {
        return maxName(bst->right);
    }
}

/*
    Pre-Conditions:
        Takes in the BST root
    Post-Conditions:
        Determines if the current node is a leaf or not. Returns 1 or 0;
*/
int isLeaf(struct Tree *bst)
{
    return (bst->left == NULL && bst->right == NULL);
}

/*
    Pre-Conditions:
        Takes in the BST root
    Post-Conditions:
        Determines if the current node has a left child node. Returns 1 or 0;
*/
int hasLeftNode(struct Tree *bst)
{
    return (bst->left != NULL && bst->right == NULL);
}

/*
    Pre-Conditions:
        Takes in the BST root
    Post-Conditions:
        Determines if the current node has a right child node. Returns 1 or 0;
*/
int hasRightNode(struct Tree *bst)
{
    return (bst->right != NULL && bst->left == NULL);
}
