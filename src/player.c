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

/**********************************************************//**
 * @struct PLAYER_SAVE
 * @brief Structure of a PLAYER save file.
 **************************************************************/
typedef struct {
    char header[8]; ///< Header data for the player.
    PLAYER payload; ///< Payload.
    char footer[8]; ///< Footer data for the player.
} PLAYER_SAVE;

/// Header and footer for the PLAYER_SAVE structure.
static const char MAGIC_NUMBER[8] = "SAVEWSM1";

/*============================================================*
 * Creating a player
 *============================================================*/
bool player_Create(PLAYER *player, const char *username) {
    
    // Zero out the struct
    memset(&player, 0, sizeof(PLAYER));
    
    // Set up player username
    int length = strlen(username);
    if (length < 1 || length > MAX_WORD_LENGTH) {
        eprintf("Player cannot have the username \"%s\"\n", username);
        return false;
    }
    strncpy(player->username, username, length);
    
    // Initialize player stats to empty
    player->nWords = 0;
    player->nTeam = 0;
    player->nBox = 0;
    player->rank = MIN_RANK;
    player->letters = 0;
    player->gold = 0;
    return true;
}

/*============================================================*
 * Player loading
 *============================================================*/
bool player_Load(PLAYER *player, const char *filename) {
    
    // Open the file
    FILE *saveFile = fopen(filename, "rb");
    if (!saveFile) {
        eprintf("Failed to open the file \"%s\"\n", filename);
        return false;
    }
    
    // Load the data from the file
    PLAYER_SAVE data;
    size_t nRead = fread(&data, sizeof(PLAYER_SAVE), 1, saveFile);
    fclose(saveFile);
    
    // Check payload size and header / footer tags.
    if (nRead != sizeof(PLAYER_SAVE) || strcmp(data.header, MAGIC_NUMBER) || strcmp(data.footer, MAGIC_NUMBER)) {
        eprintf("Failed to read valid data from the file.\n");
        return false;
    }
    
    // Successfully read player data.
    memcpy(player, &data.payload, sizeof(PLAYER));
    return true;
}

/*============================================================*
 * Player saving
 *============================================================*/
bool player_Save(const PLAYER *player, const char *filename) {
    
    // Open the file
    FILE *saveFile = fopen(filename, "wb");
    if (!saveFile) {
        eprintf("Failed to open the file \"%s\"\n", filename);
        return false;
    }
    
    // Create the payload and maintain the headers.
    PLAYER_SAVE data;
    memcpy(&data.header, MAGIC_NUMBER, sizeof(MAGIC_NUMBER));
    memcpy(&data.footer, MAGIC_NUMBER, sizeof(MAGIC_NUMBER));
    memcpy(&data.payload, player, sizeof(PLAYER));
    
    // Write the data
    size_t nWrite = fwrite(&data, sizeof(PLAYER_SAVE), 1, saveFile);
    fclose(saveFile);
    
    // Check if it succeeded.
    if (nWrite != sizeof(PLAYER_SAVE)) {
        eprintf("Failed to write all the data.\n");
        return false;
    }
    return true;
}

/*============================================================*
 * Adding words
 *============================================================*/
bool player_AddWord(PLAYER *player, const WORD *word) {
    
    // Can't add any more?
    assert(player->nWords == player->nTeam+player->nBox);
    if (player->nWords >= MAX_WORDS) {
        eprintf("The player already has too many words.\n");
        return false;
    }
    
    // Copy word into player's list.
    // This assumes the words, team, and box lists are not
    // porous - maintain this invariant through all functions!
    int index = player->nWords;
    player->words[player->nWords++] = *word;
    
    // Check if the words should be automatically added to
    // the team or placed in the box.
    if (player->nTeam < TEAM_SIZE) {
        player->team[player->nTeam++] = index;
    } else {
        assert(player->nBox < MAX_WORDS);
        player->box[player->nBox++] = index;
    }
    return true;
}

/*============================================================*
 * Removing words
 *============================================================*/
bool player_RemoveWord(PLAYER *player, const WORD *word) {
    
    // Get the index of the word. Catss ptrdiff_t to int
    int index = (int)(word - &player->words[0]);
    if (index < 0 || index >= player->nWords) {
        eprintf("Word doesn't belong to the player.\n");
        return false;
    }
    
    // Swap the last word with the excised index to maintain
    // continuity. This means the last word's index changes
    // to the current index. This might also remove a word from
    // the team, in which case the last word should be placed in
    // the box instead of in the team.
    int lastIndex = player->nWords - 1;
    if (lastIndex > 0 && lastIndex != index) {
        // We have to shift down because there are still words
        // left and we didn't remove the last word.
        player->words[index] = player->words[lastIndex];
    }
    player->nWords--;
    
    // Excise the word from the team if it is there.
    bool wordInTeam = false;
    for (int i = 0; i < player->nTeam; i++) {
        if (player->team[i] == index) {
            // Need to shift team down by one.
            while (i < player->nTeam-1) {
                player->team[i] = player->team[i+1];
                i++;
            }
            wordInTeam = true;
            player->nTeam--;
            break;
        }
    }

    // Based on where the word was found, repair the data structures.
    if (wordInTeam) {
        // Repair the team because index points elsewhere now.
        for (int i = 0; i < player->nTeam; i++) {
            if (player->team[i] == lastIndex) {
                player->team[i] = index;
            }
        }
        
    } else {
        // Excise the word from the box.
        bool wordInBox = false;
        for (int i = 0; i < player->nBox; i++) {
            if (player->box[i] == index) {
                // Need to shift the box down by one
                while (i < player->nBox-1) {
                    player->box[i] = player->box[i+1];
                    i++;
                }
                wordInBox = true;
                player->nBox--;
                break;
            }
        }
        
        // Problems
        if (!wordInBox) {
            eprintf("Failed to find the word in the team or in the box.\n");
            return false;
        }
        
        // Repair the box because index points elsewhere now.
        for (int i = 0; i < player->nTeam; i++) {
            if (player->box[i] == lastIndex) {
                player->box[i] = index;
            }
        }
    }
    
    // Done!
    assert(player->nWords == player->nTeam+player->nBox);
    return true;
}

/*============================================================*
 * Swapping words
 *============================================================*/
bool player_SwapWord(PLAYER *player, const WORD *word) {
    
    // Get the index of the word to swap.
    int index = (int)(word - &player->words[0]);
    if (index < 0 || index >= player->nWords) {
        eprintf("Word doesn't belong to the player.\n");
        return false;
    }
    
    // Check where the word resides. Default behavior is to swap the
    // word between the team and the box.
    int teamIndex = -1;
    for (int i = 0; i < player->nTeam; i++) {
        if (player->team[i] == index) {
            teamIndex = i;
            break;
        }
    }
    
    // Check where the word resides in the box
    int boxIndex = -1;
    if (teamIndex == -1) {
        for (int i = 0; i < player->nBox; i++) {
            if (player->box[i] == index) {
                boxIndex = i;
                break;
            }
        }
    }
    
    // Perform the swap
    if (teamIndex == -1 && boxIndex == -1) {
        eprintf("Word not in team or box.\n");
        return false;
        
    } else if (teamIndex == -1 && player->nTeam < TEAM_SIZE) {
        // Word in box: Move into team and repair box.
        player->team[player->nTeam++] = index;
        for (int i = boxIndex; i < player->nBox-1; i++) {
            player->box[i] = player->box[i+1];
        }
        player->nBox--;
        
    } else if (boxIndex == -1 && player->nBox < MAX_WORDS) {
        // Word in team: Move into box and repair team
        player->box[player->nBox++] = index;
        for (int i = teamIndex; i < player->nTeam; i++) {
            player->team[i] = player->team[i+1];
        }
        player->nTeam--;
    }
    return true;
}

/*============================================================*
 * Get the player's team
 *============================================================*/
bool player_GetTeam(PLAYER *player, TEAM *team) {
    
    // Error check the team size.
    if (player->nTeam <= 0) {
        eprintf("Player has no active team.\n");
        return false;
    }
    
    // Copy the words into the team.
    WORD *words[TEAM_SIZE];
    for (int i = 0; i < player->nTeam; i++) {
        words[i] = &player->words[player->team[i]];
    }
    
    // Generate the team
    return team_Create(team, words, player->nTeam);
}

/*============================================================*/
