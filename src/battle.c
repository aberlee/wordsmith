/**********************************************************//**
 * @file battle.c
 * @brief Implementation of Wordsmith battle system.
 **************************************************************/

// Standard library
#include <stddef.h>     // size_t
#include <stdbool.h>    // bool
#include <math.h>       // pow
#include <stdio.h>      // fprintf

// This project
#include "debug.h"      // assert, eprintf
#include "word.h"       // WORD
#include "technique.h"  // TECHNIQUE
#include "battle.h"     // TEAM, BATTLE

/**********************************************************//**
 * @brief Remove all the word effects from the active word.
 * @param team: The team to remove effects from.
 **************************************************************/
static inline void team_ClearWordEffects(TEAM *team) {
    // Remove the word effects
    for (int i = 0; i < N_WORD_EFFECTS; i++) {
        team->wordEffects[i] = 0;
    }
    // Remove the stat boosts
    for (int i = 0; i < N_STATS; i++) {
        team->statBoosts[i] = 0;
    }
}

/**********************************************************//**
 * @brief Remove all the field effects from the team.
 * @param team: The team to remove effects from.
 **************************************************************/
static inline void team_ClearFieldEffects(TEAM *team) {
    // Remove the field effects
    for (int i = 0; i < N_FIELD_EFFECTS; i++) {
        team->fieldEffects[i] = 0;
    }
}

/*============================================================*
 * Team initialization
 *============================================================*/
bool team_Create(TEAM *team, WORD **words, int size) {
    
    // Initialize a team given an array of 3 or less words
    if (size > TEAM_SIZE || size <= 0) {
        eprintf("Invalid number of words for a team: %d\n", size);
        return false;
    }
    
    // Place words into team
    for (int i = 0; i < size; i++) {
        team->words[i] = words[i];
    }
    team->nWords = size;
    
    // Initialize empty stats
    team_ClearWordEffects(team);
    team_ClearFieldEffects(team);
    team->techPoints = START_TP;
    team->tech = NONE;
    return true;
}

/**********************************************************//**
 * @brief Switch the active word in the team with another.
 * This automatically causes all the word effects to be cleared
 * because the active word changes.
 * @param team: The team to mutate.
 * @param index: The index of the word to switch in.
 * @return Whether the switch succeeded.
 **************************************************************/
static bool team_SwitchActiveWord(TEAM *team, int index) {
    
    // Get the next word
    if (index <= 0 || index >= team->nWords) {
        eprintf("Invalid switch-in index: %d\n", index);
        return false;
    }
    WORD *next = team->words[index];
    
    // Check next word is alive
    if (next->hp <= 0) {
        eprintf("The switch-in word is dead.\n");
        return false;
    }
    
    // Switch out word
    team->words[index] = team->words[ACTIVE_WORD];
    team->words[ACTIVE_WORD] = next;
    
    // Reset word effects because we switched in.
    team_ClearWordEffects(team);
    return true;
}

/**********************************************************//**
 * @brief Check if the team is defeated.
 * @param team: The team to check.
 * @return Whether the team was defeated.
 **************************************************************/
static inline bool team_IsDefeated(const TEAM *team) {
    for (int i = 0; i < team->nWords; i++) {
        if (team->words[i]->hp > 0) {
            return false;
        }
    }
    return true;
}

/**********************************************************//**
 * @brief Modify the stat boosts of the active word.
 * @param team: The team to mutate.
 * @param stat: The stat to boost or lower.
 * @param delta: The amount to raise or lower.
 * @return Whether the boost changed anything.
 **************************************************************/
static int team_ChangeBoost(TEAM *team, STAT stat, int delta) {
    int old = team->statBoosts[stat];
    int next = team->statBoosts[stat] + delta;
    if (next > MAX_BOOST) {
        next = MAX_BOOST;
    } else if (next < MIN_BOOST) {
        next = MIN_BOOST;
    }
    team->statBoosts[stat] = next;
    return next == old;
}

/**********************************************************//**
 * @brief Get the boosted value of the active word's stats.
 * @param team: The team to check.
 * @param stat: The stat to get.
 * @return The value of the stat.
 **************************************************************/
static int team_GetBoostedStat(const TEAM *team, STAT stat) {
    
    // Apply boost
    int unboosted = team->words[ACTIVE_WORD]->stat[stat];
    int boost = team->statBoosts[stat];
    int boosted = (int)(unboosted * pow(2.0, (double)boost));
    
    // Determine if there is a global field effect.
    int field = -1;
    switch (stat) {
    case ATTACK:
        field = FIELD_ATTACK;
        break;
    case DEFEND:
        field = FIELD_DEFEND;
        break;
    case SPEED:
        field = FIELD_SPEED;
        break;
    default:
        break;
    }
    
    // The field effect exists and is currently applied.
    if (field != -1 && team->fieldEffects[field] != 0) {
        boosted *= 2;
    }
    
    // Bounding the stat
    if (boosted < MIN_STAT) {
        boosted = MIN_STAT;
    } else if (boosted > MAX_STAT) {
        boosted = MAX_STAT;
    }
    return boosted;
}

/**********************************************************//**
 * @brief Check if any effects wear off this turn and
 * remove them from the active word.
 * @param team: The team to check.
 **************************************************************/
static void team_AdvanceEffects(TEAM *team) {
    
    // Advance all the word effects
    for (int i = 0; i < N_WORD_EFFECTS; i++) {
        if (team->wordEffects[i] == EFFECT_EPHEMERAL) {
            team->wordEffects[i] = 0;
        } else if (team->wordEffects[i] > 0) {
            team->wordEffects[i]--;
        }
    }
    
    // Advance all the field effects
    for (int i = 0; i < N_FIELD_EFFECTS; i++) {
        if (team->fieldEffects[i] == EFFECT_EPHEMERAL) {
            team->fieldEffects[i] = 0;
        } else if (team->fieldEffects[i] > 0) {
            team->fieldEffects[i]--;
        }
    }
}

/**********************************************************//**
 * @brief Get tech points for this team.
 * @param team: The team to charge.
 **************************************************************/
void team_ChargeTechPoints(TEAM *team) {
    
    // Team gets TP at the beginning of the turn
    int delta = CHARGE_TP;
    if (team->wordEffects[WORD_DOUBLE_TP] != 0) {
        delta *= 2;
    }
    
    // Increment TP
    team->techPoints += delta;
    if (team->techPoints > MAX_TP) {
        team->techPoints = MAX_TP;
    }
}

/*============================================================*/
