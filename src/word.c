/**********************************************************//**
 * @file word.h
 * @brief Wordsmith word implementations.
 **************************************************************/

// Standard library
#include <stddef.h>     // size_t
#include <stdbool.h>    // bool
#include <string.h>     // strlen, strcpy
#include <ctype.h>      // tolower

// This project
#include "debug.h"      // assert, eprintf
#include "word.h"       // WORD
#include "technique.h"  // TECHNIQUE
#include "word_table.h" // WORD_TABLE

//**************************************************************
/// Initial base stat value.
#define INITIAL_STAT 30

/// Real-word base stat boost.
#define REAL_BOOST 30

/// Base stat boost when stacks are too large.
#define OVERFLOW_BOOST 5

//**************************************************************
/// Maps each letter (0-25) to the stat it's associated with.
static const char LETTER_STATS[N_LETTERS] = {
    STAT_MAXHP,  // A
    STAT_DEFEND, // B
    STAT_SPEED,  // C
    STAT_DEFEND, // D
    STAT_MAXHP,  // E
    STAT_ATTACK, // F
    STAT_DEFEND, // G
    STAT_SPEED,  // H
    STAT_MAXHP,  // I
    STAT_ATTACK, // J
    STAT_ATTACK, // K
    STAT_SPEED,  // L
    STAT_DEFEND, // M
    STAT_SPEED,  // N
    STAT_MAXHP,  // O
    STAT_SPEED,  // P
    STAT_SPEED,  // Q
    STAT_ATTACK, // R
    STAT_ATTACK, // S
    STAT_DEFEND, // T
    STAT_MAXHP,  // U
    STAT_ATTACK, // V
    STAT_DEFEND, // W
    STAT_ATTACK, // X
    STAT_MAXHP,  // Y
    STAT_SPEED,  // Z
};

/**********************************************************//**
 * @brief Maps each letter to a corresponding stat using the
 * LETTER_STATS lookup table.
 * @param letter: An uppercase or lowercase letter.
 * @return The stat associated to the letter or -1 on failure.
 **************************************************************/
static int LetterStat(char letter) {
    // Assume input is in English
    int index = tolower(letter) - 'a';
    if (index >= 0 && index < N_LETTERS) {
        return LETTER_STATS[index];
    }
    
    // Default to MAX HP if not a latin letter.
    return STAT_MAXHP;
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
    return ((first & 3) << 2) | (second & 3);
}

/// The total number of unique technique codons.
#define N_CODONS (N_STATS*N_STATS)

/// Total number of codon stacks.
#define N_STACKS 2

/// Mapping techniques from codons.
static const TECHNIQUE TECHNIQUE_TABLE[N_CODONS][N_STACKS] = {
    // Health-dominant techniques
    {HEAL, RECOVER},                // HH
    {DRAIN, ANTI_HEAL},             // HA
    {CURE, AURA},                   // HD
    {EMERGENCY, TEAM_HEAL},         // HS
    
    // Attack-dominant techniques
    {SMASH, EXPLOSION},             // AH
    {CHARGE, BREAK},                // AA
    {STUN, BLUNT},                  // AD
    {WRAP, SLOW},                   // AS
    
    // Defend-dominant techniques
    {PROTECT, SLOW_SWITCH},         // DH
    {TANK, RETALIATE},              // DA
    {BOLSTER, SCREEN},              // DD
    {SLOW_ATTACK, REFLECT},         // DS
    
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
    if (stacks >= N_STACKS || stacks < 0) {
        return NONE;
    }
    return TECHNIQUE_TABLE[codon][stacks];
}

/**********************************************************//**
 * @brief Recompute the word's stats.
 * @param word: The word to update.
 **************************************************************/
static inline void word_UpdateStats(WORD *word) {
    for (int i = 0; i < N_STATS; i++) {
        word->stat[i] = (word->base[i]*(word->level + 5)) * 3 / 100;
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
    
    // Convert entire word to lowercase
    char lowercase[MAX_WORD_LENGTH+1];
    for (int i = 0; i < length; i++) {
        word->text[i] = toupper(text[i]);
        lowercase[i] = tolower(text[i]);
    }
    word->text[length] = '\0';
    lowercase[length] = '\0';
    
    // Check if this is a real word (need to check lowercase)
    if (!wordTable_IsValid()) {
        eprintf("Word table has not been initialized.\n");
        return false;
    }
    word->isReal = wordTable_Contains(lowercase);
    
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
    int acc[N_STATS] = {1};
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
    for (int i = 0; i < N_STATS; i++) {
        total += acc[i];
    }
    int statAverage = 1 + (total + 3) / 4;
    
    // Get the initial base stat modifier
    int initial = INITIAL_STAT;
    if (word->isReal) {
        initial += REAL_BOOST;
    }
    
    // Set initial base stats
    for (int i = 0; i < N_STATS; i++) {
        // Multiply first to avoid truncation errors
        word->base[i] = initial + (acc[i] * 60) / statAverage;
    }
    
    // Set up codon reading
    int codon;
    int codonStacks[N_CODONS];
    for (int i = 0; i < N_CODONS; i++) {
        codonStacks[i] = 0;
    }
    
    // Read all codons
    int first, second = LetterStat(text[0]);
    int index;
    int tech;
    word->nTechs = 0;
    for (int i = 1; i < length; i++) {
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
        if (tech != NONE && word->nTechs < MAX_TECHNIQUES) {
            word->techs[word->nTechs++] = tech;
        } else if (word->isReal) {
            // Only get these boosts for real words so we can prevent
            // spamming stuff like "aaaaaaaaaaaaaaaa"
            word->base[first] += OVERFLOW_BOOST;
            word->base[second] += OVERFLOW_BOOST;
        }
    }
    
    // Base stat restriction
    for (int i = 0; i < N_STATS; i++) {
        if (word->base[i] > MAX_BASE_STAT) {
            word->base[i] = MAX_BASE_STAT;
        }
    }
    
    // Find the word rank.
    int bst = word->base[STAT_MAXHP] + word->base[STAT_ATTACK] + word->base[STAT_DEFEND] + word->base[STAT_SPEED];
    if (bst < 300) {
        word->rank = RANK_F;
    } else if (bst < 350) {
        word->rank = RANK_D;
    } else if (bst < 400) {
        word->rank = RANK_C;
    } else if (bst < 450) {
        word->rank = RANK_B;
    } else if (bst < 500) {
        word->rank = RANK_A;
    } else {
        word->rank = RANK_S;
    }
    
    // Initialize stats
    word->expNeed = word->exp = ExperienceNeeded(level);
    word_UpdateStats(word);
    word->hp = word->stat[STAT_MAXHP];
    return true;
}

/*============================================================*
 * Changing HP
 *============================================================*/
void word_ChangeCurrentHP(WORD *word, int delta) {
    int temp = word->hp + delta;
    int max = word->stat[STAT_MAXHP];
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
