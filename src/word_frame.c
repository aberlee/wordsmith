/**********************************************************//**
 * @file word_frame.c
 * @brief Wordsmith word data rendering.
 **************************************************************/

// Standard library
#include <stdbool.h>        // bool
#include <stdio.h>          // sprintf

// Allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

// This project
#include "debug.h"          // eprintf, assert
#include "frame.h"          // FRAME
#include "bar.h"            // BAR
#include "word.h"           // WORD
#include "word_frame.h"     // HUD_MODE


//**************************************************************
/// Basic WORD HUD display.
static ALLEGRO_BITMAP *GlobalHUD;

/// WORD HUD display with experience bar attached.
static ALLEGRO_BITMAP *GlobalHUDExp;

/// WORD HUD display with all stats attached.
static ALLEGRO_BITMAP *GlobalHUDFull;

/// Icon denoting that a word is in the dictionary on its HUD.
static ALLEGRO_BITMAP *GlobalRealWordIcon;

/// Icon denoting a word is RANK S.
static ALLEGRO_BITMAP *GlobalRankS;

/// Icon denoting a word is RANK A.
static ALLEGRO_BITMAP *GlobalRankA;

/// Icon denoting a word is RANK B.
static ALLEGRO_BITMAP *GlobalRankB;

/// Icon denoting a word is RANK C.
static ALLEGRO_BITMAP *GlobalRankC;

/// Icon denoting a word is RANK D.
static ALLEGRO_BITMAP *GlobalRankD;

/// Icon denoting a word is RANK F.
static ALLEGRO_BITMAP *GlobalRankF;

/*============================================================*
 * Images
 *============================================================*/
void wordFrame_Initialize(void) {
    // Load HUD images
    GlobalHUD = al_load_bitmap("data/image/hud.png");
    GlobalHUDExp = al_load_bitmap("data/image/hud_exp.png");
    GlobalHUDFull = al_load_bitmap("data/image/hud_stats.png");
    
    // Load HUD icons
    GlobalRealWordIcon = al_load_bitmap("data/image/real.png");
    
    // Load rank icons
    GlobalRankS = al_load_bitmap("data/image/rank_s.png");
    GlobalRankA = al_load_bitmap("data/image/rank_a.png");
    GlobalRankB = al_load_bitmap("data/image/rank_b.png");
    GlobalRankC = al_load_bitmap("data/image/rank_c.png");
    GlobalRankD = al_load_bitmap("data/image/rank_d.png");
    GlobalRankF = al_load_bitmap("data/image/rank_f.png");
}

/**********************************************************//**
 * @brief Get the color of the health bar at the given ratio.
 * @param ratio: The ratio of health from 0.0 (dead) to 1.0.
 * @return The color of the health bar.
 **************************************************************/
static ALLEGRO_COLOR HealthColor(float ratio) {
    unsigned char r, g;
    int balance;
    if (ratio < 0.5) {
        // Balance red and yellow
        balance = 255 * 2 * ratio;
        if (balance < 0) {
            balance = 0;
        } else if (balance > 255) {
            balance = 255;
        }
        r = 255;
        g = balance;
        
    } else {
        // Balance yellow and green
        balance = 255 * 2 * (ratio - 0.5);
        if (balance < 0) {
            balance = 0;
        } else if (balance > 255) {
            balance = 255;
        }
        r = 255 - balance;
        g = 255;
    }
    return al_map_rgb(r, g, 60);
}

/*============================================================*
 * Draw the word HUD
 *============================================================*/
void wordFrame_DrawHUD(const WORD *word, int x, int y, HUD_MODE mode) {
    // Choose whether to draw experience or not
    switch (mode) {
    case HUD_EXTENDED:
        al_draw_bitmap(GlobalHUDExp, x, y, 0);
        break;
    
    case HUD_FULL:
        al_draw_bitmap(GlobalHUDFull, x, y, 0);
        break;
    
    case HUD_BASIC:
    default:
        al_draw_bitmap(GlobalHUD, x, y, 0);
        break;
    }
    
    // Draw the word's name
    frame_DrawText(x+8, y+7, word->text);
    
    // Buffer for all strings
    char string[32];
    
    // Draw the word's level
    sprintf(string, "%d", word->level);
    frame_DrawText(x+144, y+7, string);
    
    // Draw the word's health bar
    BAR health;
    health.x = x+6;
    health.y = y+21;
    health.width = 127;
    health.height = 5;
    health.ratio = (float)word->hp / word->stat[STAT_MAXHP];
    health.foreground = HealthColor(health.ratio);
    health.flags = BAR_NO_BACKGROUND;
    bar_Draw(&health);
    
    // Draw the word's health fraction
    sprintf(string, "%d/%d", word->hp, word->stat[STAT_MAXHP]);
    frame_DrawOutlinedText(x+7, y+19, string);
    
    // Draw the real word symbol
    if (word->isReal) {
        al_draw_bitmap(GlobalRealWordIcon, x+137, y+20, 0);
    }

    // Draw the word's experience bar
    if (mode == HUD_EXTENDED || mode == HUD_FULL) {
        BAR exp;
        exp.x = x+17;
        exp.y = (mode == HUD_FULL)? y+92: y+33;
        exp.width = 151;
        exp.height = 2;
        exp.ratio = 1.0 - (float)word->exp / word->expNeed;
        exp.foreground = al_map_rgb(124, 118, 184);
        exp.flags = BAR_NO_BACKGROUND;
        bar_Draw(&exp);
    }
    
    // Draw the full stats
    if (mode == HUD_FULL) {
        // Draw all the stats
        sprintf(string, "%d", word->stat[STAT_ATTACK]);
        frame_DrawText(x+146, y+41, string);
        
        sprintf(string, "%d", word->stat[STAT_DEFEND]);
        frame_DrawText(x+146, y+53, string);
        
        sprintf(string, "%d", word->stat[STAT_SPEED]);
        frame_DrawText(x+146, y+65, string);
        
        // Draw the rank
        ALLEGRO_BITMAP *rank;
        switch (word->rank) {
        case RANK_S:
            rank = GlobalRankS;
            break;
        case RANK_A:
            rank = GlobalRankA;
            break;
        case RANK_B:
            rank = GlobalRankB;
            break;
        case RANK_C:
            rank = GlobalRankC;
            break;
        case RANK_D:
            rank = GlobalRankD;
            break;
        case RANK_F:
        default:
            rank = GlobalRankF;
            break;
        }
        al_draw_bitmap(rank, x+147, y+77, 0);
        
        // Draw the techniques
        const TECHNIQUE_DATA *data;
        int techY = y+41;
        for (int i = 0; i < word->nTechs; i++) {
            data = technique_GetData(word->techs[i]);
            frame_DrawText(x+7, techY, data->name);
            techY += 12;
        }
    }
}

/*============================================================*/
