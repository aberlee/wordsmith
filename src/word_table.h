/**********************************************************//**
 * @file word_table.h
 * @brief Header file for word dictionary tables.
 **************************************************************/

#ifndef _WORD_TABLE_H_
#define _WORD_TABLE_H_

// Standard library
#include <stdbool.h>    // bool

/**********************************************************//**
 * @brief Loads the given file as the "real words" table.
 * This is a text file of lowercase words separated by '\n'.
 * The text file must be in alphabetical order.
 * @param filename: The file to load.
 * @return Whether the loading succeeded. If it succeeds you
 * must destroy the table with wordtable_Destroy later.
 **************************************************************/
extern bool wordTable_Load(const char *filename);

/**********************************************************//**
 * @brief Destroys the initialized word table.
 **************************************************************/
extern void wordTable_Destroy(void);

/**********************************************************//**
 * @brief Check if the word table is loaded.
 * @return Whether the word table has been initialized by a
 * call to wordtable_Load.
 **************************************************************/
extern bool wordTable_IsValid(void);

/**********************************************************//**
 * @brief Checks if a word is in the table.
 * @param what: The string to check if it is the table.
 * @return Whether the word is in the table.
 **************************************************************/
extern bool wordTable_Contains(const char *what);

/*============================================================*/
#endif // _WORD_TABLE_H_
