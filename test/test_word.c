/**********************************************************//**
 * @file test_word.c
 * @brief Testing program for words.
 **************************************************************/

 // Standard library
#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // strcmp

// This project
#include "word.h"       // WORD
#include "technique.h"  // TECHNIQUE_DATA

/**********************************************************//**
 * @brief test driver method.
 **************************************************************/
int main(int argc, char **argv) {
    
    // Data
    WORD word;
    int level = 0;
    const char *text = NULL;
    
    // Arguments check
    if (argc == 1 || !strcmp(argv[1], "-h")) {
        printf("Usage: %s word [level]?\n", argv[0]);
        return EXIT_FAILURE;
    }
    if (argc > 1) {
        text = argv[1];
    }
    if (argc > 2) {
        level = atoi(argv[2]);
    } else {
        level = 100;
    }
    
    // Create the word.
    if (!word_Create(&word, text, level)) {
        printf("Failed to make the word.\n");
        return EXIT_FAILURE;
    }
    
    // Print the word data.
    printf("%s (Level %d)\n", word.text, word.level);
    
    // Print the stats
    printf("Hp: %d/%d\n", word.hp, word.stat[MAXHP]);
    printf("Attack: %d\n", word.stat[ATTACK]);
    printf("Defend: %d\n", word.stat[DEFEND]);
    printf("Speed: %d\n", word.stat[SPEED]);
    
    // Print the techniques
    const TECHNIQUE_DATA *data;
    if (word.nTechs == 0) {
        printf("<No Techniques>\n");
    } else {
        printf("Techniques (%d)\n", word.nTechs);
    }
    for (int i = 0; i < word.nTechs; i++) {
        data = technique_GetData(word.techs[i]);
        printf("+ %s\n", data->name);
    }
    
    return EXIT_SUCCESS;
}

/*============================================================*/
