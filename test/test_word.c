/**********************************************************//**
 * @file test_word.c
 * @brief Testing program for words.
 **************************************************************/

 // Standard library
#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // strcmp

// This project
#include "debug.h"      // assert, eprintf
#include "word.h"       // WORD
#include "technique.h"  // TECHNIQUE_DATA
#include "word_table.h" // WORd_TABLE

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
        eprintf("Usage: %s word [level]?\n", argv[0]);
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
    
    // Get the word table
    if (!wordtable_Load("data/words/english.txt")) {
        eprintf("Failed to load word table.\n");
        return EXIT_FAILURE;
    }
    
    // Create the word.
    if (!word_Create(&word, text, level)) {
        eprintf("Failed to make the word.\n");
        return EXIT_FAILURE;
    }
    
    // Get the rank
    const char *rank;
    switch (word.rank) {
    case RANK_F:
        rank = "F";
        break;
    case RANK_D:
        rank = "D";
        break;
    case RANK_C:
        rank = "C";
        break;
    case RANK_B:
        rank = "B";
        break;
    case RANK_A:
        rank = "A";
        break;
    case RANK_S:
        rank = "S";
        break;
    default:
        rank = "?";
        break;
    }
    
    // Print the word data.
    if (word.isReal) {
        printf("%s (Level %d, Rank %s*)\n", word.text, word.level, rank);
    } else {
        printf("%s (Level %d, Rank %s)\n", word.text, word.level, rank);
    }
    
    // Print the stats
    printf("Hp: %d/%d (%d)\n", word.hp, word.stat[MAXHP], word.base[MAXHP]);
    printf("Attack: %d (%d)\n", word.stat[ATTACK], word.base[ATTACK]);
    printf("Defend: %d (%d)\n", word.stat[DEFEND], word.base[DEFEND]);
    printf("Speed: %d (%d)\n", word.stat[SPEED], word.base[SPEED]);
    printf("BST: %d\n", word.base[MAXHP]+word.base[ATTACK]+word.base[DEFEND]+word.base[SPEED]);
    
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
