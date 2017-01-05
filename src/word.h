/**********************************************************//**
 * @file word.h
 * @brief Wordsmith word data header file.
 **************************************************************/
 
#ifndef _WORD_H_
#define _WORD_H_

// Standard library
#include <stdbool.h>    // bool

// This project
#include "technique.h"  // TECHNIQUE

/*============================================================*
 * Constants
 *============================================================*/

/// Number of letters supported (a-z)
#define N_LETTERS 26

/// The maximum number of special techniques any word can have.
#define MAX_TECHNIQUES 4

// Word length bounds
#define MIN_WORD_LENGTH 2   ///< Length of the smallest word
#define MAX_WORD_LENGTH 16  ///< Length of the longest word.

// Word level range
#define MIN_LEVEL 1         ///< The minimum word level.
#define MAX_LEVEL 100       ///< The maximum word level.

// Range for word stats
#define MIN_BASE_STAT 1     ///< The minimum base stat value.
#define MAX_BASE_STAT 255   ///< The maximum base stat value.

// Range for actual word stats
#define MIN_STAT 1          ///< The minimum value for any computed stat.
#define MAX_STAT 9999       ///< The maximum value for any computed stat.

/**********************************************************//**
 * @enum STAT
 * @brief Defines all word statistics.
 **************************************************************/
typedef enum {
    MAXHP,  ///< Maximum health.
    ATTACK, ///< Attack power.
    DEFEND, ///< Defensive power.
    SPEED,  ///< Speed and agility.
} STAT;

/// The total number of unique stats.
#define N_STATS 4

/**********************************************************//**
 * @enum RANK
 * @brief Defines the rank of the word, based on the base stat
 * total (BST).
 **************************************************************/
typedef enum {
    RANK_F=0,   ///< Worst rank. BST < 300
    RANK_D=1,   ///< Lower rank. 300 <= BST < 350
    RANK_C=2,   ///< Average rank. 350 <= BST < 400
    RANK_B=3,   ///< Above average rank. 400 <= BST < 450
    RANK_A=4,   ///< Excellent rank. 450 <= BST < 500
    RANK_S=5,   ///< Best rank. 500 <= BST
} RANK;

/**********************************************************//**
 * @struct WORD
 * @brief Defines all the data in one word.
 **************************************************************/
typedef struct {
    // Constant properties
    char text[MAX_WORD_LENGTH+1];   ///< Actual text of the word
    TECHNIQUE techs[MAX_TECHNIQUES];///< Techniques known.
    int base[N_STATS];  ///< Constant base stats.
    int nTechs;         ///< Number of techniques.
    RANK rank;          ///< The rank of the word.
    bool isReal;        ///< Whether this word is a real word.
    
    // Current stats (recalculate on level up)
    int level;          ///< Level of the word
    int hp;             ///< Current HP
    int exp;            ///< Current EXP
    int expNeed;        ///< Required experience to level up.
    int stat[N_STATS];  ///< Current stats
    
} WORD;

/**********************************************************//**
 * @brief Create a word
 * @param word: Output parameter for the word which is being
 * constructed.
 * @param text: The English text of the word.
 * @param level: Initial level of the word
 * @return Whether the word was successfully constructed.
 **************************************************************/
extern bool word_Create(WORD *word, const char *text, int level);

/**********************************************************//**
 * @brief Heal or damage the word
 * @param word: The word to read
 * @param delta: heal if > 0 otherwise damage
 **************************************************************/
extern void word_ChangeCurrentHP(WORD *word, int delta);

/**********************************************************//**
 * @brief Increase the EXP of the word and level up if needed
 * @param word: The word in question
 * @param delta: the amount of EXP earned
 **************************************************************/
extern void word_ChangeExperience(WORD *word, int delta);

/*============================================================*/
#endif // _WORD_H_
