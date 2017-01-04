/**********************************************************//**
 * @file word_table.c
 * @brief Implementation of word tables.
 **************************************************************/

// Standard library
#include <stddef.h>     // size_t
#include <stdbool.h>    // bool
#include <stdlib.h>     // malloc, free
#include <stdio.h>      // FILE
#include <string.h>     // strcmp

// This project
#include "debug.h"      // assert, eprintf
#include "word_table.h" // WORD_TABLE

/*============================================================*
 * Loading the table
 *============================================================*/
bool wordtable_Load(WORD_TABLE *words, const char *filename) {
    
    // Load the file
    FILE *file = fopen(filename, "r");
    if (!file) {
        eprintf("Failed to open file.\n");
        return false;
    }
    
    // Set up the lookup table
    for (int i = 0; i < N_LETTERS; i++) {
        words->lookup[i] = LOOKUP_EMPTY;
    }
    
    // Read the entire file
    int tableSize = 1024;
    words->table = (char **)malloc(tableSize * sizeof(char *));
    words->size = 0;
    const size_t BUF_SIZE = 48;
    char buf[BUF_SIZE+1];
    while (fgets(buf, BUF_SIZE, file)) {
        // Knock newline off of word
        int end = strlen(buf) - 1;
        if (end >= 0) {
            buf[end] = '\0';
        }
        
        // Lookup table management
        int key = buf[0] - 'a';
        if (key >= 0 && key < N_LETTERS) {
            if (words->lookup[key] == LOOKUP_EMPTY) {
                words->lookup[key] = words->size;
            }
        }
        
        // Check if we need to expand the table.
        if (words->size >= tableSize) {
            // Reallocate the table
            char **newTable = (char **)malloc(2*tableSize * sizeof(char *));
            if (!newTable) {
                eprintf("Failed to expand the table.\n");
                wordtable_Destroy(words);
                return false;
            }
            
            // Copy the old table
            for (int i = 0; i < words->size; i++) {
                newTable[i] = words->table[i];
            }
            
            // Replace the table
            free(words->table);
            words->table = newTable;
            tableSize *= 2;
        }

        // Generate the current table entry
        words->table[words->size] = (char *)malloc((strlen(buf)+1) * sizeof(char));
        if (!words->table[words->size]) {
            eprintf("Out of memory.\n");
            wordtable_Destroy(words);
            return false;
        }
        strcpy(words->table[words->size], buf);
        words->size++;
    }
    
    // Compress the table size
    char **compressed = (char **)malloc(words->size * sizeof(char *));
    if (!words->table) {
        eprintf("Failed to compress the table.\n");
        wordtable_Destroy(words);
        return false;
    }
    
    // Copy the old table
    for (int i = 0; i < words->size; i++) {
        compressed[i] = words->table[i];
    }
    free(words->table);
    words->table = compressed;
    return true;
}

/*============================================================*
 * Destroying the table
 *============================================================*/
void wordtable_Destroy(WORD_TABLE *table) {
    // Guard against double free.
    if (table->table) {
        for (int i = 0; i < table->size; i++) {
            free(table->table[i]);
        }
        free(table->table);
    }
    
    // Reset table attributes.
    table->table = NULL;
    table->size = 0;
}

/*============================================================*
 * Contianment checking
 *============================================================*/
static bool ContainsHelper(const WORD_TABLE *words, const char *what, int start, int end) {
    // Base case
    if (start == end+1 || start >= end) {
        return strcmp(words->table[start], what) == 0;
    }
    
    // Split
    int midpoint = (start + end) / 2;
    int compare = strcmp(words->table[midpoint], what);
    if (compare == 0) {
        return true;
    } else if (compare > 0) {
        return ContainsHelper(words, what, start, midpoint);
    } else {
        return ContainsHelper(words, what, midpoint+1, end);
    }
}
    

bool wordtable_Contains(const WORD_TABLE *words, const char *what) {
    return ContainsHelper(words, what, 0, words->size);
}

/*============================================================*/
