/**********************************************************//**
 * @file player.h
 * @brief Wordsmith player game data header file.
 **************************************************************/

#ifndef _PLAYER_H_
#define _PLAYER_H_

// Standard library
#include <stdbool.h>    // bool

// This project
#include "word.h"       // WORD, MAX_WORD_LENGTH
#include "battle.h"     // TEAM_SIZE

/*============================================================*
 * Constants
 *============================================================*/

/// The maximum number of words a player can own.
#define MAX_WORDS 100

// Ranks
#define MIN_RANK 1  ///< The first rank.
#define MAX_RANK 5  ///< The maximum rank.

/**********************************************************//**
 * @struct PLAYER
 * @brief Contains all the player's game data.
 **************************************************************/
typedef struct {
    
    // Words owned by the player
    WORD words[MAX_WORDS];  ///< Actual word data.
    int team[TEAM_SIZE];    ///< Words in the player's team.
    int box[MAX_WORDS];     ///< Order of words in the box.
    int nWords;             ///< The number of words owned.
    int nTeam;              ///< The number of words in the team.
    int nBox;               ///< The number of words in the box.
    
    // User constants.
    char username[MAX_WORD_LENGTH]; ///< Player's username
    int rank;                       ///< Player's rank.
    
    // User resources.
    int letters;    ///< Letters the user can spend to make words.
    int gold;       ///< Money the user can spend.
    
} PLAYER;

/**********************************************************//**
 * @brief Create an empty player data structure.
 * @param player: The player data to initialize.
 * @param username: The player's name.
 * @return Whether the creation succeeded.
 **************************************************************/
extern bool player_Create(PLAYER *player, const char *username);

/**********************************************************//**
 * @brief Load the player's data from the file.
 * @param player: The player data to initialize.
 * @param filename: The file to load from.
 * @return Whether the load succeeded.
 **************************************************************/
extern bool player_Load(PLAYER *player, const char *filename);

/**********************************************************//**
 * @brief Save the player's information to the file.
 * @param player: The player data to save.
 * @param filename: The file to save to.
 * @return Whether the save succeeded.
 **************************************************************/
extern bool player_Save(const PLAYER *player, const char *filename);

/**********************************************************//**
 * @brief Add a word to the player's words and possibly
 * insert it automatically into the team.
 * @param player: The player data to mutate.
 * @param word: Pointer to a WORD structure that is copied.
 * @return Whether the word could be added.
 **************************************************************/
extern bool player_AddWord(PLAYER *player, const WORD *word);

/**********************************************************//**
 * @brief Remove a word from the player's words.
 * @param player: The player data to mutate.
 * @param word: Pointer to a WORD to remove.
 * @return Whether the word could be removed.
 **************************************************************/
extern bool player_RemoveWord(PLAYER *player, const WORD *word);

/**********************************************************//**
 * @brief Swap a word to or from the active team.
 * @param player: The player data to mutate.
 * @param word: Pointer to a WORD to swap.
 * @return Whether the swap succeeded.
 **************************************************************/
extern bool player_SwapWord(PLAYER *player, const WORD *word);

/**********************************************************//**
 * @brief Generate a pointer array for the player team
 * @param player: The player data to mutate.
 * @param team: The team to load.
 * @return Whether the team was successfully created.
 **************************************************************/
extern bool player_GetTeam(PLAYER *player, TEAM *team);

/*============================================================*/
#endif // _PLAYER_H_
