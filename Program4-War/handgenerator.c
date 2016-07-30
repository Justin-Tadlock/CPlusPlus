#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUMGAMES 2

struct card {
    int value;
    char suit;
};

int main() {
    int i, j, k, temp;
    struct card deck[52];
    struct card player1[26], player2[26];
    FILE * ofp = fopen("games.txt", "w");

    srand(time(0));

    fprintf(ofp, "%d\n", NUMGAMES);

    for(k=0; k<NUMGAMES; k++) {

        for (i=0; i<4; i++)
            for (j=0; j<13; j++) {
                deck[(i*13)+j].value = j+1;
                if (i==0)
                    deck[(i*13)+j].suit = 'H';
                else if (i==1)
                    deck[(i*13)+j].suit = 'D';
                else if (i==2)
                    deck[(i*13)+j].suit = 'C';
                else
                    deck[(i*13)+j].suit = 'S';
            }

        for (i=0; i<26; i++) {
            temp = rand() % 52;
            while(deck[temp].value == 0) {
                temp = rand() % 52;
            }
            player1[i] = deck[temp];
            deck[temp].value = 0;

            temp = rand() % 52;
            while(deck[temp].value == 0) {
                temp = rand() % 52;
            }
            player2[i] = deck[temp];
            deck[temp].value = 0;
        }

        for (i=0; i<26; i++)
            fprintf(ofp, "%d %c ", player1[i].value, player1[i].suit);

        fprintf(ofp, "\n");

        for (i=0; i<26; i++)
            fprintf(ofp, "%d %c ", player2[i].value, player2[i].suit);

        for (i=0; i<4; i++)
            for (j=0; j<13; j++)
                deck[(i*13)+j].value = j+1;

        fprintf(ofp, "\n");

    }

    fclose(ofp);

    return 0;
}
