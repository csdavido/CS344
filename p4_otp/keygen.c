#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char* MakeKey(int size) {
    //ALLOCATE TO HOLD KEY
    char* key = malloc(sizeof(char) * size);
    //LOOP THROUGH LENGTH
    int x;
    for(x = 0; x < size; x++) {
        //IF A 1 OR 9 GET PRODUCED
        if( (rand() % (10) + 1) == 1 || (rand() % (10) + 1) == 9) {
            //ADD A SPACE
            key[x] = ' ';
        }
        else {
            //ADD A LETTER
            key[x] = (rand() % (90 + 1 - 65) + 65);
        }
    }
    //ADD TO THE END
    key[x] = '\0';
    //RETURN KEY
    return key;
}


int main(int argc, char* argv[]) {
    //VARIABLES
    int length;
    char* key;
    //SIMPLE ERROR HANDLING
    if(argc == 1) {

        fprintf(stderr,"Error! Enter a key length...\n");
        exit(1);
    }

    //RANDOMIZE
    srand(time(NULL));
    length = atoi(argv[1]) + 1;

    //CREATE KEY
    key = MakeKey(length);
    //DISPLAY KEY
    printf("%s", key);
    //FREE MEMORY
    free(key);

    return 0;
}