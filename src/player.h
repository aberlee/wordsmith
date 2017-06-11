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

//**************************************************************
/// The maximum number of words a player can own.
#define MAX_WORDS 100

#define MAX_USERNAME_LENGTH 25
#define MIN_USERNAME_LENGTH 3

/// The number of words on one team.
#define TEAM_SIZE 3

/**********************************************************//**
 * @struct PLAYER
 * @brief Contains all the player's game data.
 **************************************************************/
typedef struct {
    // Words owned by the player
    WORD words[MAX_WORDS];  ///< Actual word data.
    int nWords;             ///< The number of words owned.
    
    /// Words selected to be in the team
    int team[TEAM_SIZE];
    int nTeam;
    
    // User constants.
    char username[MAX_USERNAME_LENGTH]; ///< Player's username
} PLAYER;

/**********************************************************//**
 * @brief Create an empty player data structure.
 * @param player: The player data to initialize.
 * @param username: The player's name.
 * @return Whether the creation succeeded.
 **************************************************************/
extern bool player_Create(PLAYER *player, const char *username);

static inline const char *player_Username(const PLAYER *player) {
    return player->username;
}

static inline int player_WordCount(const PLAYER *player) {
    return player->nWords;
}

static inline int player_TeamIndex(const PLAYER *player, int index) {
    for (int i = 0; i < TEAM_SIZE; i++) {
        if (player->team[i] == index) {
            return i;
        }
    }
    return -1;
}

static inline bool player_TeamContainsWord(const PLAYER *player, int index) {
    return player_TeamIndex(player, index) >= 0;
}

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
 * @param index: Index of the word to remove.
 * @return Whether the word could be removed.
 **************************************************************/
extern bool player_RemoveWord(PLAYER *player, int index);

/**********************************************************//**
 * @brief Swap a word to or from the active team.
 * @param player: The player data to mutate.
 * @param word: Pointer to a WORD to swap.
 * @return Whether the swap succeeded.
 **************************************************************/
extern bool player_SwapWord(PLAYER *player, int index);

/*============================================================*/
#endif // _PLAYER_H_
