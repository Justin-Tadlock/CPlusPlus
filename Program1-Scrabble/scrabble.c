/*
    Name: Justin Tadlock
    Date: 9/14/2012
    Course: COP3502
    Section Number: 0001

    Program #01:
    Create a scrabble game with a custom dictionary storing available words within a text file.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
  Grabbing the list of words the player will be able to use within the game.
*/
char ** getDictionaryWords(char ** dictionary, int * numWords, int * flag)
{
    int i;
    FILE * file = fopen("dictionary.txt", "r");
    fscanf(file, "%d", *&numWords);
    if(*numWords > 10000)
    {
        *flag = 0;
        fclose(file);
        return;
    }

    //Dynamically allocate the necessary memory for the dictionary array.
    dictionary  = (char**)malloc(*numWords*sizeof(char*));

    //Populate the dictionary array
    for(i = 0; i < *numWords; i++)
    {
        dictionary[i] = (char*)malloc(8*sizeof(char));
        fscanf(file, "%s", dictionary[i]);
    }
    fclose(file);

    return dictionary;
}

/*
  Create the list of available letters the player can use.
*/
void generateHand(char letters[8])
{
    int i, randomNumber = 0, counter;
    char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    //Amount of each letter within the alphabet available to distribute to the hand
    int numberOfLetters[] = {9,2,2,4,12,2,3,2,9,1,1,4,2,6,8,2,1,6,4,6,4,2,2,1,2,1};

    //Hand size is 8 letters.
    for(i = 0; i < 7; i++)
    {
        randomNumber = rand()%26;
        if(numberOfLetters[randomNumber] > 0)
        {
            letters[i] = alphabet[randomNumber];
            numberOfLetters[randomNumber] -= 1;
        }
        //redistribute if current letter is no longer available
        else if(numberOfLetters[randomNumber] <= 0)
            --i;
    }
    letters[7] = '\0';
}

int printMenu(int choice)
{
    printf("\nWhat would you like to do?\n");
    printf("\t 1 - Enter a Word\n");
    printf("\t 2 - Quit\n");
    printf("Choice: ");
    scanf("%d", &choice);

    return choice;
}

/*
  Ask for a word for the player to enter.
*/
void getWord(char * word)
{
    int i;

    printf("Enter a word: ");
    scanf("%s", word);
    getchar(); //getting the \n char out of the buffer

    //Convert letters to uppercase
    for(i = 0; i < strlen(word); i++)
        word[i] = toupper(word[i]);
}

/*
  Verify the word the player entered is within the dictionary of available words.
*/
int checkValid(char * word, char ** dictionary, int size)
{
    int beg = 0, end = size-1;

    while( beg <= end )
    {
        int mid = (beg+end) / 2;
        int cmp = strcmp(word, dictionary[mid]);

        if(cmp < 0)
            end = mid - 1;
        else if(cmp > 0)
            beg = mid + 1;
        else
            return 1;
    }
    return 0;
}

/*
  Calculate the number of points for the word entered by the player.
*/
void checkScore(char * word, char * bestWord, int * score)
{
    int i;
    int scores[] = {1,3,3,2,1,4,2,4,1,8,5,1,3,1,1,3,10,1,1,1,1,4,4,8,4,10};
    int letter, diff;
    int points = 0;

    for(i = 0; i < strlen(word); i++)
    {
        letter = word[i];
        diff = letter - 'A'; //value from 0-25
        points += scores[diff];
    }

    printf("That word is worth %d points.", points);
    if(points > *score)
    {
        *score = points;
        strcpy(bestWord, word);
    }
}

//Begin Main
int main()
{
    srand(time(0));

    int numWordsInDictionary, i, flag; //flag, when dictionary > 10,000
    int choice, score, isPlaying;
    char word[8], letters[8], bestWord[8];
    char ** dictionary = getDictionaryWords(dictionary, &numWordsInDictionary, &flag);

    //initializing values
    strcpy(bestWord, ""); //clearing garbage
    score = 0;
    isPlaying = 1;

    if(flag != 0)
    {
        printf("Welcome to the Scrabble Practice Program!\n\n");

        generateHand(letters);
        printf("Here are your letters: %s", letters);

        while(isPlaying == 1)
        {
            choice = printMenu(choice);
            if(choice == 1)
            {
                getWord(word);
                if( checkValid(word, dictionary, numWordsInDictionary) == 1 )
                    checkScore(word, bestWord, &score);
                else
                    printf("That word is not found within our current dictionary!\n");
            }
            else
                break;
        }

        printf("Your best word was %s worth %d points.\n", bestWord, score);
        for(i = 0; i < numWordsInDictionary; i++)
            free(dictionary[i]);
        free(dictionary);
    }
    else
    {
        printf("Error! The dictionary cannot be more than 10,000 words!\n");
        free(dictionary);
    }

    return 0;
}
