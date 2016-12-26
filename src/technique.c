/**********************************************************//**
 * @file technique.c
 * @brief Technique data implementation file.
 **************************************************************/

// This project
#include "technique.h"

/*============================================================*
 * Technique data stats
 *============================================================*/
static const TECHNIQUE_DATA TECHNIQUES[N_TECHNIQUES] = {
    // Sentinel techniques
    [NONE] = {
        .id=NONE,
        .name="None",
        .power=0,
        .cost=0,
        .description="This attack does nothing.",
        .priority=PRI_NORMAL
    },
    
    // Default techniques
    [T_ATTACK] = {
        .id=T_ATTACK,
        .name="Attack",
        .power=30,
        .cost=1,
        .description="A basic attack that damages the enemy.",
        .priority=PRI_NORMAL,
    },
    [T_DEFEND] = {
        .id=T_DEFEND,
        .name="Defend",
        .power=0,
        .cost=0,
        .description="The user takes half damage this turn.",
        .priority=PRI_FASTEST,
    },
    [SWITCH] = {
        .id=SWITCH,
        .name="Switch",
        .power=0,
        .cost=1,
        .description="The user switches out.",
        .priority=PRI_FASTEST,
    },
    
    // Healing attacks
    [HEAL] = {
        .id=HEAL,
        .name="Heal",
        .power=0,
        .cost=6,
        .description="The user recovers half its HP.",
        .priority=PRI_NORMAL,
    },
    [RECOVER] = {
        .name="Recover",
        .power=0,
        .cost=8,
        .description="The user recovers half its HP and is cured of status.",
        .priority=PRI_NORMAL,
    },
    [DRAIN] = {
        .id=DRAIN,
        .name="Drain",
        .power=30,
        .cost=2,
        .description="The user steals HP from the target, healing itself.",
        .priority=PRI_NORMAL,
    },
    [ANTI_HEAL] = {
        .id=ANTI_HEAL,
        .name="Anti Heal",
        .power=0,
        .cost=2,
        .description="The target is prevented from healing for 5 turns.",
        .priority=PRI_NORMAL,
    },
    [CURE] = {
        .id=CURE,
        .name="Cure",
        .power=0,
        .cost=2,
        .description="The user is cured of status.",
        .priority=PRI_NORMAL,
    },
    [AURA] = {
        .id=AURA,
        .name="Aura",
        .power=0,
        .cost=4,
        .description="The user heals some HP at the end of each turn for 5 turns.",
        .priority=PRI_NORMAL,
    },
    [EMERGENCY] = {
        .id=EMERGENCY,
        .name="Emergency",
        .power=0,
        .cost=4,
        .description="The user heals some HP. This attack usually goes first.",
        .priority=PRI_FAST,
    },
    [TEAM_HEAL] = {
        .id=TEAM_HEAL,
        .name="Team Heal",
        .power=0,
        .cost=10,
        .description="The user and its allies recover some HP.",
        .priority=PRI_NORMAL,
    },
    
    // Offensive attacks
    [SMASH] = {
        .id=SMASH,
        .name="Smash",
        .power=120,
        .cost=6,
        .description="A powerful attack that damages the user as well.",
        .priority=PRI_NORMAL,
    },
    [EXPLOSION] = {
        .id=EXPLOSION,
        .name="Explosion",
        .power=255,
        .cost=10,
        .description="The user explodes and passes out, dealing terrible damage.",
        .priority=PRI_NORMAL,
    },
    [CHARGE] = {
        .id=CHARGE,
        .name="Charge",
        .power=0,
        .cost=4,
        .description="The user charges power and increases its Attack.",
        .priority=PRI_NORMAL,
    },
    [BREAK] = {
        .id=BREAK,
        .name="Break",
        .power=60,
        .cost=6,
        .description="The user attacks viciously. The target's Defend is lowered.",
        .priority=PRI_NORMAL,
    },
    [BLUNT] = {
        .id=BLUNT,
        .name="Blunt",
        .power=60,
        .cost=6,
        .description="The user disarms the target. The target's Attack is lowered.",
        .priority=PRI_NORMAL,
    },
    [STUN] = {
        .id=STUN,
        .name="Stun",
        .power=60,
        .cost=8,
        .description="The target is stunned, leaving it unable to attack this turn.",
        .priority=PRI_NORMAL,
    },
    [WRAP] = {
        .id=WRAP,
        .name="Wrap",
        .power=30,
        .cost=8,
        .description="The target is damaged at the end of each turn for 5 turns. It cannot switch out.",
        .priority=PRI_NORMAL,
    },
    [SLOW] = {
        .id=SLOW,
        .name="Slow",
        .power=60,
        .cost=6,
        .description="The target is slowed down, reducing its Speed.",
        .priority=PRI_NORMAL,
    },
    
    // Defensive attacks
    [PROTECT] = {
        .id=PROTECT,
        .name="Protect",
        .power=0,
        .cost=10,
        .description="The user is protected from any attack this turn.",
        .priority=PRI_FASTEST,
    },
    [SLOW_SWITCH] = {
        .id=SLOW_SWITCH,
        .name="Slow Switch",
        .power=0,
        .cost=4,
        .description="The user stalls and switches out. This always goes last.",
        .priority=PRI_SLOWEST, // This must occur last.
    },
    [RETALIATE] = {
        .id=RETALIATE,
        .name="Retaliate",
        .power=0,
        .cost=4,
        .description="The enemy is damaged if they attack the user for 5 turns.",
        .priority=PRI_NORMAL,
    },
    [TANK] = {
        .id=TANK,
        .name="Tank",
        .power=30,
        .cost=3,
        .description="The user attacks while defending itself.",
        .priority=PRI_NORMAL,
    },
    [BOLSTER] = {
        .id=BOLSTER,
        .name="Bolster",
        .power=0,
        .cost=4,
        .description="The user sturdies itself and increases its Defend.",
        .priority=PRI_NORMAL,
    },
    [SCREEN] = {
        .id=SCREEN,
        .name="Screen",
        .power=0,
        .cost=8,
        .description="Damage is halved for the user's team for 5 turns.",
        .priority=PRI_NORMAL,
    },
    [SLOW_ATTACK] = {
        .id=SLOW_ATTACK,
        .name="Slow Attack",
        .power=60,
        .cost=4,
        .description="An attack that always goes last.",
        .priority=PRI_SLOW, // SLOW_SWITCH is slower
    },
    [REFLECT] = {
        .id=REFLECT,
        .name="Reflect",
        .power=0,
        .cost=8,
        .description="The enemy takes all damage the user would take this turn.",
        .priority=PRI_FASTEST,
    },
    
    // Speed attacks
    [CONCENTRATE] = {
        .id=CONCENTRATE,
        .name="Concentrate",
        .power=0,
        .cost=0,
        .description="The user's TP recharge speed is doubled for 5 turns.",
        .priority=PRI_NORMAL,
    },
    [STEAL] = {
        .id=STEAL,
        .name="Steal",
        .power=0,
        .cost=2,
        .description="The user steals up to 4 TP from the enemy.",
        .priority=PRI_NORMAL,
    },
    [SWITCH_ATTACK] = {
        .id=SWITCH_ATTACK,
        .name="Switch Attack",
        .power=30,
        .cost=3,
        .description="The user switches out after attacking.",
        .priority=PRI_NORMAL,
    },
    [QUICK_ATTACK] = {
        .id=QUICK_ATTACK,
        .name="Quick Attack",
        .power=30,
        .cost=3,
        .description="The user attacks quickly, always going first.",
        .priority=PRI_FAST,
    },
    [HAZARD] = {
        .id=HAZARD,
        .name="Hazard",
        .power=0,
        .cost=6,
        .description="The enemy field is trapped. Every enemy switching in is damaged for 5 turns.",
        .priority=PRI_NORMAL,
    },
    [EJECT] = {
        .id=EJECT,
        .name="Eject",
        .power=30,
        .cost=6,
        .description="The enemy is attacked and forced to switch out.",
        .priority=PRI_NORMAL,
    },
    [QUICKEN] = {
        .id=QUICKEN,
        .name="Quicken",
        .power=0,
        .cost=4,
        .description="The user moves quickly, increasing its Speed.",
        .priority=PRI_NORMAL,
    },
    [SWIFT] = {
        .id=SWIFT,
        .name="Swift",
        .power=0,
        .cost=8,
        .description="The user team's Speed is doubled for 5 turns.",
        .priority=PRI_NORMAL,
    },
    
    // Special attacks
    [SPECIAL] = {
        .name="Special",
        .power=90,
        .cost=6,
        .description="A special event attack.",
        .priority=PRI_NORMAL,
    },
};

/*============================================================*
 * Technique data accessor
 *============================================================*/
const TECHNIQUE_DATA *technique_GetData(TECHNIQUE tech) {
    return &TECHNIQUES[tech];
}

/*============================================================*/
