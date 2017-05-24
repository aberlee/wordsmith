/**********************************************************//**
 * @file player.c
 * @brief Wordsmith player game data implementation.
 **************************************************************/

// Standard library
#include <stdbool.h>    // bool
#include <stdio.h>      // fread, fwrite
#include <string.h>     // strlen, strncpy, memset, memcpy

// This project
#include "debug.h"      // assert, eprintf
#include "player.h"     // PLAYER
#include "word.h"       // WORD

/*============================================================*
 * Creating a player
 *============================================================*/
bool player_Create(PLAYER *player, const char *username) {
    // Set up player username
    int length = strlen(username);
    if (length < MIN_USERNAME_LENGTH || length > MAX_USERNAME_LENGTH) {
        eprintf("Player cannot have the username \"%s\"\n", username);
        return false;
    }
    strncpy(player->username, username, length);
    
    // Initialize player stats to empty
    player->nWords = 0;
    player->nTeam = 0;
    return true;
}

/*============================================================*
 * Adding words
 *============================================================*/
bool player_AddWord(PLAYER *player, const WORD *word) {
    // Can't add any more?
    if (player->nWords >= MAX_WORDS) {
        eprintf("The player already has too many words.\n");
        return false;
    }
    
    // Copy word into player's list.
    int index = player->nWords;
    player->words[index] = *word;
    player->nWords++;
    
    // Add word to the team if necessary.
    if (player->nTeam < TEAM_SIZE) {
        player->team[player->nTeam++] = index;
    }
    return true;
}

static inline void RemoveFromTeam(PLAYER *player, int index) {
    for (int i = 0; i < player->nTeam; i++) {
        if (player->team[i] == index) {
            while (i < player->nTeam-1) {
                player->team[i] = player->team[i+1];
            }
            player->nTeam--;
            break;
        }
    }
}

/*============================================================*
 * Removing words
 *============================================================*/
bool player_RemoveWord(PLAYER *player, int index) {
    // Delete the current word
    if (index < 0 || index >= player->nWords) {
        eprintf("Word not found in player's team.\n");
        return false;
    }
    
    // Shift words in team up one
    for (int i = index; i < player->nWords-1; i++) {
        player->words[i] = player->words[i+1];
    }
    player->nWords--;
    
    // Reset pointers in the team
    RemoveFromTeam(player, index);
    for (int i = 0; i < player->nTeam; i++) {
        if (player->team[i] > index) {
            player->team[i]--;
        }
    }
    return true;
}

/*============================================================*
 * Swapping words
 *============================================================*/
bool player_SwapWord(PLAYER *player, int index) {
    // Swap from team to box?
    if (player_TeamContainsWord(player, index)) {
        RemoveFromTeam(player, index);
        return true;
    }
    
    // Check team capacity.
    if (player->nTeam >= TEAM_SIZE) {
        eprintf("Team is full.\n");
        return false;
    }
    
    // Swap from box to team.
    player->team[player->nTeam++] = index;
    return true;
}

/*============================================================*/
