/**********************************************************//**
 * @file battle.h
 * @brief Wordsmith battle system header file.
 **************************************************************/

#ifndef _BATTLE_H_
#define _BATTLE_H_

// Standard library
#include <stddef.h>     // size_t

// This project
#include "word.h"       // WORD
#include "player.h"

/*============================================================*
 * Constants
 *============================================================*/

// Technique points
#define MIN_TP 0        ///< The minimum amount of technique points.
#define MAX_TP 20       ///< The maximum number of technique points.
#define START_TP 10     ///< The initial number of technique points for a team.
#define CHARGE_TP 2     ///< Amount of technique points recovered each turn.

// Boosts
#define MAX_BOOST 4     ///< The maximum stat boost level.
#define MIN_BOOST -4    ///< The minimum stat boost level.

// Effect keys
#define EFFECT_PERMANENT -2 ///< The effect is applied permanently.
#define EFFECT_EPHEMERAL -1 ///< The effect is applied for one turn only.
#define EFFECT_MAX_TIME 5   ///< The standard number of turns an effect lasts.

/// The index of the active word on a team.
#define ACTIVE_WORD 0

/**********************************************************//**
 * @enum WORD_EFFECT
 * @brief Defines all different effects that can be applied
 * to words (that affect only the active word).
 **************************************************************/
typedef enum {
    WORD_BLOCK_HEAL,  ///< The word cannot heal.
    WORD_AURA_HEAL,   ///< Heal at the end of the turn
    WORD_AURA_HURT,   ///< Damage at the end of the turn
    WORD_STUN,        ///< Cannot attack this turn
    WORD_PROTECT,     ///< Shielded from everything
    WORD_DEFEND,      ///< Take half damage
    WORD_RETALIATE,   ///< Enemy damaged on attack
    WORD_REFLECT,     ///< Enemy attack bounced back
    WORD_DOUBLE_TP,   ///< Recover TP twice as fast
    WORD_NO_ESCAPE,   ///< The word cannot switch out.
} WORD_EFFECT;

/// The number of different word effects.
#define N_WORD_EFFECTS 10

/**********************************************************//**
 * @enum FIELD_EFFECT
 * @brief Defines all different effects that can be applied
 * to the field (and affect the team as a whole).
 **************************************************************/
typedef enum {
    FIELD_ATTACK,     ///< Raise whole team Attack
    FIELD_DEFEND,     ///< Raise whole team Defend
    FIELD_SPEED,      ///< Raise whole team Speed
    FIELD_HAZARD,     ///< Field hazard on switching in
} FIELD_EFFECT;

/// The number of different field effects.
#define N_FIELD_EFFECTS 4

/**********************************************************//**
 * @struct TEAM
 * @brief Defines all the words and effects on one team in
 * a battle.
 **************************************************************/
typedef struct {
    // Permanent data
    WORD *words[TEAM_SIZE]; ///< All words in the active team.
    int nWords;             ///< Number of words on team.
    
    // Battle data
    int techPoints;                     ///< Current number of technique points.
    int fieldEffects[N_FIELD_EFFECTS];  ///< The field effects currently applied.
    int wordEffects[N_WORD_EFFECTS];    ///< The word effects currently applied.
    int statBoosts[N_STATS];            ///< Active word's stat boosts.
    
    // Per-turn data
    TECHNIQUE tech;     ///< The technique being used this turn.
} TEAM;

/**********************************************************//**
 * @struct BATTLE
 * @brief Holds all battle data.
 **************************************************************/
typedef struct {
    TEAM users;     ///< The user-controlled team.
    TEAM enemies;   ///< The enemy-controlled team.
    int turnCount;  ///< The current turn of battle.
    bool isOver;    ///< Whether the battle has ended.
    bool usersWon;  ///< Whether the user team won.
} BATTLE;

/**********************************************************//**
 * @brief Generate a pointer array for the player team
 * @param player: The player data to mutate.
 * @param team: The team to load.
 * @return Whether the team was successfully created.
 **************************************************************/
extern bool player_GetTeam(PLAYER *player, TEAM *team);

/**********************************************************//**
 * @brief Initialize a team structure with the given words.
 * @param team: The team to initialize.
 * @param words: Pointer to a word array that forms the team.
 * @param size: Size of the array (up to TEAM_SIZE).
 * @return Whether the team creation succeeded.
 **************************************************************/
extern bool team_Create(TEAM *team, WORD **words, int size);

/**********************************************************//**
 * @brief Start a battle between the two teams.
 * @param battle: The battle to conduct.
 * @return Whether the battle succeeded.
 **************************************************************/
extern bool battle_Start(BATTLE *battle);

/*============================================================*/
#endif // _BATTLE_H_
