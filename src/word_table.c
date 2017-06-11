/**********************************************************//**
 * @file word_table.c
 * @brief Implementation of word tables.
 **************************************************************/

// Standard library
#include <stddef.h>         // size_t
#include <stdbool.h>        // bool
#include <stdlib.h>         // malloc, free
#include <stdio.h>          // FILE
#include <string.h>         // strcmp

// This project
#include "debug.h"          // assert, eprintf
#include "word_table.h"     // WORD_TABLE

/// The number of unique lowercase letters.
#define N_LETTERS 26

/**********************************************************//**
 * @struct WORD_TABLE
 * @brief Stores lookup table of words.
 **************************************************************/
typedef struct {
    char **table;           ///< The table of all words.
    int size;               ///< Entries in the table.
    int lookup[N_LETTERS];  ///< Letter-indexed lookup table.
} WORD_TABLE;

/// Value for when the lookup table has no value.
#define LOOKUP_EMPTY -1

/// The current word table
static WORD_TABLE GlobalWords = {0, 0, {LOOKUP_EMPTY}};

/*============================================================*
 * Table is valid
 *============================================================*/
bool wordTable_IsValid(void) {
    return GlobalWords.size != 0;
}

/*============================================================*
 * Loading the table
 *============================================================*/
bool wordTable_Load(const char *filename) {
    // Load the file
    FILE *file = fopen(filename, "r");
    if (!file) {
        eprintf("Failed to open file.\n");
        return false;
    }
    
    // Set up the lookup table
    for (int i = 0; i < N_LETTERS; i++) {
        GlobalWords.lookup[i] = LOOKUP_EMPTY;
    }
    
    // Read the entire file
    int tableSize = 1024;
    GlobalWords.table = (char **)malloc(tableSize * sizeof(char *));
    GlobalWords.size = 0;
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
            if (GlobalWords.lookup[key] == LOOKUP_EMPTY) {
                GlobalWords.lookup[key] = GlobalWords.size;
            }
        }
        
        // Check if we need to expand the table.
        if (GlobalWords.size >= tableSize) {
            // Reallocate the table
            char **newTable = (char **)malloc(2*tableSize * sizeof(char *));
            if (!newTable) {
                eprintf("Failed to expand the table.\n");
                wordTable_Destroy();
                fclose(file);
                return false;
            }
            
            // Copy the old table
            for (int i = 0; i < GlobalWords.size; i++) {
                newTable[i] = GlobalWords.table[i];
            }
            
            // Replace the table
            free(GlobalWords.table);
            GlobalWords.table = newTable;
            tableSize *= 2;
        }

        // Generate the current table entry
        GlobalWords.table[GlobalWords.size] = (char *)malloc((strlen(buf)+1) * sizeof(char));
        if (!GlobalWords.table[GlobalWords.size]) {
            eprintf("Out of memory.\n");
            wordTable_Destroy();
            fclose(file);
            return false;
        }
        strcpy(GlobalWords.table[GlobalWords.size], buf);
        GlobalWords.size++;
    }
    
    // Done reading
    fclose(file);
    
    // Compress the table size
    char **compressed = (char **)malloc(GlobalWords.size * sizeof(char *));
    if (!GlobalWords.table) {
        eprintf("Failed to compress the table.\n");
        wordTable_Destroy();
        return false;
    }
    
    // Copy the old table
    for (int i = 0; i < GlobalWords.size; i++) {
        compressed[i] = GlobalWords.table[i];
    }
    free(GlobalWords.table);
    GlobalWords.table = compressed;
    return true;
}

/*============================================================*
 * Destroying the table
 *============================================================*/
void wordTable_Destroy(void) {
    // Guard against double free.
    if (GlobalWords.table) {
        for (int i = 0; i < GlobalWords.size; i++) {
            free(GlobalWords.table[i]);
        }
        free(GlobalWords.table);
    }
    
    // Reset table attributes.
    GlobalWords.table = NULL;
    GlobalWords.size = 0;
}

/*============================================================*
 * Contianment checking
 *============================================================*/
static bool ContainsHelper(const char *what, int start, int end) {
    // Base case
    if (start == end+1 || start >= end) {
        return strcmp(GlobalWords.table[start], what) == 0;
    }
    
    // Split
    int midpoint = (start + end) / 2;
    int compare = strcmp(GlobalWords.table[midpoint], what);
    if (compare == 0) {
        return true;
    } else if (compare > 0) {
        return ContainsHelper(what, start, midpoint);
    } else {
        return ContainsHelper(what, midpoint+1, end);
    }
}

bool wordTable_Contains(const char *what) {
    if (GlobalWords.size == 0) {
        return false;
    }
    return ContainsHelper(what, 0, GlobalWords.size);
}

/*============================================================*/
