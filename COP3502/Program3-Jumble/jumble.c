/*
    Name: Justin Tadlock
    Date: 10/12/2012
    Course: COP3502
    Section Number: C001
    Program #03:
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//convenient functions
char ** getDictionary(char ** dictionary, int * numWords);
char chooseToPlay();
void pause();
void getWord(char * word);
void freeMemory(char ** array, int size);
void printToFile(FILE * file, char * jumbleWord, char * permutation);
void swapLetters(char * word, int init, int swap);

//main functions
void binSearch(char * word, char * permutation, int length, char ** dictionary, FILE * file, int * counter);
void getPermutations(char * jumbleWord, char * permutations, char ** dictionary, int initial, FILE * file, int numWords, int * counter);

/*
    Pre-conditions:
        Variables:
            numWords contains the number of words in the dictionary file
            counter contains the number of permutations that were formed by the entered jumble puzzle by the user
            jumbleWord contains the original string of the jumble puzzle
            permutations is the "mutatable" string for the permutations function
            userInput records if the user wants to enter a word or exit the program
*/
int main()
{
    int numWords = 0, counter;
    char userInput = '0';
    char jumbleWord[19], permutations[19];
    char ** dictionary = NULL;
    FILE * file = fopen("jumbleout.txt", "w"); //writes the results to a file until closed

    dictionary = getDictionary(dictionary, &numWords); //setting up the 2D array for the dictionary

    printf("Welcome to the Jumble Puzzle Solver!\n"); //Welcome message for the user

    //Main loop for the program to run until the user decides to not enter another word.
    while(userInput != 'N')
    {
        userInput = chooseToPlay(); //Receive the input for the user's choice to enter another word or close the program
        switch(userInput)
        {
            case 'Y':
                getWord(jumbleWord); //asking user for the jumble puzzle
                strcpy(permutations, jumbleWord); //make a copy for the permutations function to modify

                //calling recursive function to check permutations for words in the dictionary
                counter = 0; //counter for the number of valid permutations

                //Gathering the permutations within the entered jumbled word.
                getPermutations(jumbleWord, permutations, dictionary, 0, file, numWords, &counter);

                //If no permutations were found, print a message to the screen and to the message to the file
                if(counter == 0)
                {
                    printf("Sorry, no permutations of %s form a word.\n", jumbleWord);
                    fprintf(file, "Sorry, no permutations of %s form a word.\n", jumbleWord);
                }
                break;
            case 'N': //break out of the loop and end the program
                break;
            default: //tell the user they entered a wrong input.
                printf("Error! Please enter either Y or N.\n");
                pause(); //press enter to continue message
                break;
        }
    }

    fclose(file); //closing the file

    freeMemory(dictionary, numWords); //free the memory of the 2D array, dictionary
    return 0;
}

/*
    Pre-Conditions:
        Takes in a 2D array and a reference number representing the number of words in the file
        Opens the dictionary.txt file to read words from.
    Post-Conditions:
        Allocates memory for the size of the 2D array
        Returns the 2D array filled with the words within the dictionary.txt file.
        numWords value is set to the number of words within the file.
        Closes the dictionary.txt file.
*/
char ** getDictionary(char ** dictionary, int * numWords)
{
    int i;

    //Opening the dictionary file to pull out the  words
    FILE * file = fopen("dictionary.txt", "r");
    fscanf(file, "%d", *&numWords);

    //Allocating the correct space in memory to store the words
    dictionary  = (char**)malloc(*numWords*sizeof(char*));

    //Allocating length 21 to all words to each string within the dictionary array
    for(i = 0; i < *numWords; i++)
    {
        dictionary[i] = (char*)malloc(19*sizeof(char)); //allocating a string size
        fscanf(file, "%s", dictionary[i]);
    }
    fclose(file); //closing file

    return dictionary; //returning the array pointer to main
}

/*
    Pre conditions: Asking user if they would like to enter a jumbled puzzle
    Post-Conditions: returns the first letter of a string input
*/
char chooseToPlay()
{
    char answer[20]; //creating a temp string to store user input

    //asking user for input
    printf("\nWould you like to enter a jumbled word?\n");
    scanf("%s", answer);
    getchar(); //clear the input buffer

    //catching string errors and converting the first letter of the string to upper case and returning the result
    answer[0] = toupper(answer[0]);
    return answer[0];
}

/*
    Pre-Conditions:
        Prints a message for the user to see
    Post-Conditions:
        Waits for the user to hit the enter key.
*/
void pause()
{
    printf("Press Enter to continue..");
    getchar();
}

/*
    Pre-Conditions:
        Takes in a pointer to a string that will be manipulated
    Post-Conditions:
        After receiving the input from the user, converts the string to an all upper-case string.
*/
void getWord(char * word)
{
    int i, maxSize = 20; //maxSize initialize arbitrary, checks during the while loop

    while(maxSize > 19)
    {
        //Ask the user for the jumble puzzle word
        printf("What word would you like scored?\n");
        scanf("%s", word);
        getchar(); //clearing the input buffer

        maxSize = strlen(word); //set maxSize to the length of the string to check for a valid entry

        if(maxSize > 19) //case the string is not <= 19 letters in length
        {
            printf("Error! Please enter a jumble word that is less than or equal to 19 letters long.\n");
            pause();
        }
    }

    //convert all characters to uppercase
    for(i = 0; i < strlen(word); i++)
    {
        word[i] = toupper(word[i]);
    }
}

/*
    Pre-conditions:
        Takes in a 2D array and its size
    Post-Conditions:
        Frees the memory allocated to the array.
*/
void freeMemory(char** array, int size)
{
    int i;

    for(i = 0; i < size; i++) //Free each element in the 2D array
        free(array[i]);
    free(array); //free the head of the array
}

/*
    Pre-Conditions:
        Takes in a file type, a word, and a permutated word.
    Post-Conditions:
        Print to the screen and to a file that the permutation was a valid word.
*/
void printToFile(FILE * file, char * jumbleWord, char * permutation)
{
    printf("A permutation of %s that is a valid word is %s.\n", jumbleWord, permutation);
    fprintf(file, "A permutation of %s that is a valid word is %s.\n", jumbleWord, permutation);
}

/*
    Pre-Conditions:
        Takes in a word, the original position, and the position to swap the letters with
    Post-Conditions:
        Swaps the letters in the initial and swap indexes within the word
*/
void swapLetters(char * word, int init, int swap)
{
    char temp = word[init];

    word[init] = word[swap];
    word[swap] = temp;
}

/*
    Pre-Conditions:
        Takes in a word, the number of words in the dictionary, and the 2D dictionary array
    Post-Conditions:
        Checks for a word in the dictionary, returns 1 if exists, 0 if it doesn't.
*/
void binSearch(char * word, char * permutation, int length, char ** dictionary, FILE * file, int * counter)
{
    int beg = 0, end = length-1; //starting and ending points

    while (beg <= end) //while there's more words to check in the dictionary..
    {
        int mid = (beg+end)/2; //find halfway point
        int compare = strcmp(permutation, dictionary[mid]);//get the value, -1,1 or 0

        if(compare < 0)//if the word searching for is before dictionary halfway point, shift down halfway point
        {
            end = mid-1;
        }
        else if(compare > 0)//if the word searching for is after, shift up halfway point
        {
            beg = mid+1;
        }
        else //found the word! YAY
        {
            printToFile(file, word, permutation);//print the original string and the permutation stating that it was a valid word.
            *counter +=1; // valid word found, do not print the invalid permutations checker in main.
            return;
        }
    }
    return; //didn't find the word.... boo.....
}

/*
    Pre-Conditions:
        Takes in the original word, permutations string, 2D array, string index, file object, number of words in dictionary, and a counter for valid words
    Post-Conditions:
        Recursively finds permutations of a string.
        If any of those permutations found are valid words within the dictionary, print a message to the string indicating that it was found.

*/
void getPermutations(char * jumbleWord, char * permutations, char ** dictionary, int initial, FILE * file, int numWords, int * counter)
{
    int swap;

    if(initial == strlen(jumbleWord))//found the end, now to see if all the permutations are valid words...
    {
        binSearch(jumbleWord, permutations, numWords, dictionary, file, counter); //check if it's a valid word
    }
    else
    {
        //let's find the permutations of the currently modified "permutations" string
        for(swap=initial; swap <strlen(jumbleWord); swap++)
        {
            swapLetters(permutations, initial, swap); //swapping the appropriate characters for the permutations

            //While there's more letters in jumbled word, get more permutations
            getPermutations(jumbleWord, permutations, dictionary, initial+1, file, numWords, counter);

            swapLetters(permutations, swap, initial); //swapping the letters back to their original index.
        }
    }
}
