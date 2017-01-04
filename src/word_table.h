/**********************************************************//**
 * @file word_table.h
 * @brief Header file for word dictionary tables.
 **************************************************************/

#ifndef _WORD_TABLE_H_
#define _WORD_TABLE_H_

// Standard library
#include <stdbool.h>    // bool

/// The number of unique lowercase letters.
#define N_LETTERS 26

/**********************************************************//**
 * @struct WORD_TABLE
 * @brief Stores lookup table of words.
 **************************************************************/
typedef struct {
    char **table;     ///< The table of all words.
    int size;               ///< Entries in the table.
    int lookup[N_LETTERS];  ///< Letter-indexed lookup table.
} WORD_TABLE;

/// Value for when the lookup table has no value.
#define LOOKUP_EMPTY -1

/// Initializer for the word table.
#define WORD_TABLE_INITIALIZER {0, 0, {LOOKUP_EMPTY}}

/**********************************************************//**
 * @brief Loads the given file as the "real words" table.
 * This is a text file of lowercase words separated by '\n'.
 * The text file must be in alphabetical order.
 * @param words: The table to load.
 * @param filename: The file to load.
 * @return Whether the loading succeeded. If it succeeds you
 * must destroy the table with wordtable_Destroy later.
 **************************************************************/
extern bool wordtable_Load(WORD_TABLE *words, const char *filename);

/**********************************************************//**
 * @brief Destroys the initialized word table.
 * @param words: The table to get rid of.
 **************************************************************/
extern void wordtable_Destroy(WORD_TABLE *table);

/**********************************************************//**
 * @brief Checks if a word is in the table.
 * @param words: The table to query.
 * @param what: The string to check if it is the table.
 * @return Whether the word is in the table.
 **************************************************************/
extern bool wordtable_Contains(const WORD_TABLE *words, const char *what);

/*============================================================*/
#endif // _WORD_TABLE_H_
