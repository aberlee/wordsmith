/**********************************************************//**
 * @file word.h
 * @brief Wordsmith word implementations.
 **************************************************************/

// Standard library
#include <stddef.h>     // size_t
#include <stdbool.h>    // bool
#include <string.h>     // strlen, strcpy
#include <ctype.h>      // toupper

// This project
#include "debug.h"      // assert, eprintf
#include "word.h"       // WORD
#include "technique.h"  // TECHNIQUE

/// Maps each letter (0-25) to the stat it's associated with.
static const char LETTER_STATS[N_LETTERS] = {
    MAXHP,  // A
    DEFEND, // B
    SPEED,  // C
    DEFEND, // D
    MAXHP,  // E
    ATTACK, // F
    DEFEND, // G
    SPEED,  // H
    MAXHP,  // I
    ATTACK, // J
    ATTACK, // K
    SPEED,  // L
    DEFEND, // M
    SPEED,  // N
    MAXHP,  // O
    SPEED,  // P
    SPEED,  // Q
    ATTACK, // R
    ATTACK, // S
    DEFEND, // T
    MAXHP,  // U
    ATTACK, // V
    DEFEND, // W
    ATTACK, // X
    MAXHP,  // Y
    SPEED,  // Z
};

/**********************************************************//**
 * @brief Maps each letter to a corresponding stat using the
 * LETTER_STATS lookup table.
 * @param letter: An uppercase or lowercase letter.
 * @return The stat associated to the letter or -1 on failure.
 **************************************************************/
static int LetterStat(char letter) {
    int index;

    // Try lower case
    index = letter - 'a';
    if (index >= 0 && index < N_LETTERS) {
        return LETTER_STATS[index];
    }
    
    // Try upper case
    index = letter - 'A';
    if (index >= 0 && index < N_LETTERS) {
        return LETTER_STATS[index];
    }
    
    // Failed to get the stat
    eprintf("Invalid word letter: '%c'\n", letter);
    return -1;
}

/**********************************************************//**
 * @brief Maps two stats to a unique stat codon.
 * @param first: The primary stat.
 * @param second: The secondary stat.
 * @return A unique stat codon.
 **************************************************************/
static inline int StatCodon(STAT first, STAT second) {
    // Generates a codon from the two stat keys
    // Stat keys are 0, 1, 2, 3 (2 bits)
    return (first << 2) | second;
}

// The total number of unique technique codons.
#define N_CODONS (N_STATS*N_STATS)

// Codon stacks
#define PRIMARY 0   // This is the primary technique for the codon.
#define ADVANCED 1  // This is the secondary technique for the codon.

// Total number of codon stacks.
#define N_STACKS 2

// Base stat boost when stacks are too large.
#define STAT_BOOST 2

/// Mapping techniques from codons.
static const TECHNIQUE TECHNIQUE_TABLE[N_CODONS][N_STACKS] = {
    // Health-dominant techniques
    {HEAL, RECOVER},        // HH
    {DRAIN, ANTI_HEAL},     // HA
    {CURE, AURA},           // HD
    {EMERGENCY, TEAM_HEAL}, // HS
    
    // Attack-dominant techniques
    {SMASH, EXPLOSION},     // AH
    {CHARGE, BREAK},        // AA
    {STUN, BLUNT},          // AD
    {WRAP, SLOW},           // AS
    
    // Defend-dominant techniques
    {PROTECT, SLOW_SWITCH}, // DH
    {TANK, RETALIATE},      // DA
    {BOLSTER, SCREEN},      // DD
    {SLOW_ATTACK, REFLECT}, // DS
    
    // Speed-dominant attacks
    {CONCENTRATE, STEAL},           // SH
    {SWITCH_ATTACK, QUICK_ATTACK},  // SA
    {HAZARD, EJECT},                // SD
    {QUICKEN, SWIFT},               // SS
};

/**********************************************************//**
 * @brief Gets the technique the codon is associated with.
 * @param codon: The stat codon.
 * @param stacks: Number of times the codon has occurred
 * previously in the word.
 * @return The technique or -1 on failure.
 **************************************************************/
static int CodonTechnique(int codon, int stacks) {
    
    // Error checking
    if (codon < 0 || codon >= N_CODONS) {
        eprintf("Invalid codon: %d\n", codon);
        return -1;
    }
    
    // Get the technique from the table
    switch (stacks) {
    case PRIMARY:
        return TECHNIQUE_TABLE[codon][PRIMARY];
        
    case ADVANCED:
        return TECHNIQUE_TABLE[codon][ADVANCED];
    
    default:
        // Too many stacks applied, no further techniques!
        return NONE;
    }
}

/**********************************************************//**
 * @brief Recompute the word's stats.
 * @param word: The word to update.
 **************************************************************/
static inline void word_UpdateStats(WORD *word) {
    for (int i = 0; i < N_STATS; i++) {
        word->stat[i] = (word->base[i]*(word->level + 5)) / 100;
    }
}

/**********************************************************//**
 * @brief Experience needed to move from level n to level n+1.
 * @param n: The current level.
 * @return The amount of experience needed.
 **************************************************************/
static inline int ExperienceNeeded(int n) {
    return n*n;
}

/*============================================================*
 * Creating a word
 *============================================================*/
bool word_Create(WORD *word, const char *text, int level) {
    
    // Set word text
    int length = strlen(text);
    if (length < MIN_WORD_LENGTH || length > MAX_WORD_LENGTH) {
        eprintf("The word \"%s\" is of invalid length.\n", text);
        return false;
    }
    
    // Convert entire word to uppercase
    for (int i = 0; i < length; i++) {
        word->text[i] =toupper(text[i]);
    }
    word->text[length] = '\0';
    
    // Set level
    if (level < MIN_LEVEL || level > MAX_LEVEL) {
        eprintf("Invalid initial word level: %d\n", level);
        return false;
    }
    word->level = level;
    
    // Accumulate total base stats for the letter
    // The word gets points for each letter, with letters located
    // earlier in the word having more weight. After this is
    // completed, the word certainly contains only valid stats.
    int acc[N_STATS] = {1, 1, 1, 1};
    int stat;
    for (int i = 0; i < length; i++) {
        // Weight each letter according to its position in the word
        if ((stat = LetterStat(text[i])) < 0) {
            eprintf("Invalid stat discovered: %d\n", stat);
            return false;
        }
        acc[stat] += 1;
    }
    
    // Scale base stat totals (balancing)
    int total = 0;
    for (int i = 0; i < N_STATS; total += acc[i++]);
    int statAverage = (total + 3) / 4;
    
    // Set base stats
    int baseStat;
    for (int i = 0; i < N_STATS; i++) {
        // Multiply first to avoid truncation errors
        baseStat = (acc[i] * 255) / statAverage;
        if (baseStat > MAX_STAT) {
            baseStat = MAX_STAT;
        } else if (baseStat < MIN_STAT) {
            baseStat = MIN_STAT;
        }
        word->base[i] = baseStat;
    }
    
    // Set up codon reading
    int codon;
    int codonStacks[N_CODONS];
    for (int i = 0; i < N_CODONS; i++) {
        codonStacks[i] = PRIMARY;
    }
    
    // Read all codons
    int first, second = LetterStat(text[0]);
    int index;
    int tech;
    word->nTechs = 0;
    for (int i = 1; i < length && word->nTechs < MAX_TECHNIQUES; i++) {
        // Read the next codon (loop if invalid)
        first = second;
        index = i % length;
        second = LetterStat(text[index]);
        codon = StatCodon(first, second);
        if ((tech = CodonTechnique(codon, codonStacks[codon]++)) < 0) {
            eprintf("Invalid technique: %d\n", tech);
            return false;
        }
        
        // Boost base stat if a repeat codon was discovered.
        // It is OK if the stats boosted are the same.
        // It is also impossible for tech to already exist in
        // the word's moveset so long as the mapping array is
        // configured properly.
        if (tech == NONE) {
            word->base[first] += STAT_BOOST;
            word->base[second] += STAT_BOOST;
        } else {
            word->techs[word->nTechs++] = tech;
        }
    }
    
    // Initialize stats
    word->expNeed = word->exp = ExperienceNeeded(level);
    word_UpdateStats(word);
    word->hp = word->stat[MAXHP];
    return true;
}

/*============================================================*
 * Changing HP
 *============================================================*/
void word_ChangeCurrentHP(WORD *word, int delta) {
    int temp = word->hp + delta;
    int max = word->stat[MAXHP];
    int min = 0;
    if (temp > max) {
        word->hp = max;
    } else if (temp < min) {
        word->hp = min;
    } else {
        word->hp = temp;
    }
    return;
}

/*============================================================*
 * Gaining experience
 *============================================================*/
void word_ChangeExperience(WORD *word, int delta) {
    word->exp -= delta;
    while (word->exp < 0) {
        word->level++;
        word->exp += word->expNeed = ExperienceNeeded(word->level);
    }
    word_UpdateStats(word);
}

/*============================================================*/
