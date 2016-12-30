/**********************************************************//**
 * @file technique.h
 * @brief Wordsmith technique data header file.
 **************************************************************/

#ifndef _TECHNIQUE_H_
#define _TECHNIQUE_H_

// Standard library
#include <stdbool.h>    // bool

/**********************************************************//**
 * @enum PRIORITY
 * @brief Defines all technique priority orders.
 **************************************************************/
typedef enum {
    PRI_FASTEST=5,  ///< The fastest priority of all.
    PRI_FAST=4,     ///< Above-average priority.
    PRI_NORMAL=3,   ///< Ordinary priority.
    PRI_SLOW=2,     ///< Below-average priority.
    PRI_SLOWEST=1,  ///< The slowest priority of all.
} PRIORITY;

/**********************************************************//**
 * @enum TECHNIQUE
 * @brief Defines all different techniques.
 **************************************************************/
typedef enum {
    // Sentinels
    NONE = 0,       ///< No technique is defined (sentinel).
    
    // Basic attacks
    T_ATTACK,       ///< Basic attack that damages the enemy.
    T_DEFEND,       ///< Take reduced damage and recharge TP.
    SWITCH,         ///< Switch your word out.
    
    // Health-dominant techniques
    HEAL,           ///< Heal 50% of your HP
    RECOVER,        ///< Heal 50% of your HP and solve status effects
    DRAIN,          ///< Damage the enemy and heal yourself
    ANTI_HEAL,      ///< Prevent the target from healing
    CURE,           ///< Solve status effects
    AURA,           ///< You heal at the end of each turn
    EMERGENCY,      ///< Heal yourself 25% (always go first)
    TEAM_HEAL,      ///< Entire team is healed 12.5%
    
    // Attack-dominant techniques
    SMASH,          ///< Damages the enemy but take recoil
    EXPLOSION,      ///< Lots of damage but you die
    CHARGE,         ///< Increase your attack
    BREAK,          ///< Damages the enemy and may reduce defend
    BLUNT,          ///< Damages the enemy and may reduce attack
    STUN,           ///< Damages the enemy and may make them immobile
    WRAP,           ///< Damage the enemy and possibly make it inactive
    SLOW,           ///< Damages the enemy and may reduce speed
    
    // Defend-dominant techniques
    PROTECT,        ///< Do not take damage this turn
    SLOW_SWITCH,    ///< Switch out (always go last)
    RETALIATE,      ///< The enemy is hurt if they attack you
    TANK,           ///< Attack while defending yourself
    BOLSTER,        ///< Increase your defend
    SCREEN,         ///< Increase team defend for 5 turns
    SLOW_ATTACK,    ///< Damages the enemy (always go last)
    REFLECT,        ///< Enemy takes all damage you would take
    
    // Speed-dominant attacks
    CONCENTRATE,    ///< Increase TP recharge speed
    STEAL,          ///< Take TP from the enemy
    SWITCH_ATTACK,  ///< Damage the enemy and switch out after
    QUICK_ATTACK,   ///< Damage the enemy (always go first)
    HAZARD,         ///< Enemy team damaged by switching in
    EJECT,          ///< Damage the enemy and force it to switch out
    QUICKEN,        ///< Increase your speed
    SWIFT,          ///< Increase team speed for 5 turns
    
    // Event-only attacks
    SPECIAL,        ///< Do a lot of damage

} TECHNIQUE;

/// Number of distinct techniques
#define N_TECHNIQUES 37

/**********************************************************//**
 * @struct TECHNIQUE_DATA
 * @brief Defines the data for each technique.
 **************************************************************/
typedef struct {
    TECHNIQUE id;           ///< The identity of the technique.
    const char *name;       ///< Name of the technique
    int power;              ///< Power of the technique
    int cost;               ///< Cost to use the technique
    const char *description;///< English description of the technique
    PRIORITY priority;      ///< Speed priority of the technique
} TECHNIQUE_DATA;

/**********************************************************//**
 * @brief Get technique data for the given technique key.
 * @param tech: The technique key.
 * @return Pointer to a TECHNIQUE_DATA struct with the data.
 **************************************************************/
extern const TECHNIQUE_DATA *technique_GetData(TECHNIQUE tech);

/*============================================================*/
#endif // _TECHNIQUE_H_
