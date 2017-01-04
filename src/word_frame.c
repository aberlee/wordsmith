/**********************************************************//**
 * @file word_frame.c
 * @brief Wordsmith word data rendering.
 **************************************************************/

// Standard library
#include <stdio.h>      // sprintf

// Allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

// This project
#include "frame.h"      // frame_DrawText
#include "bar.h"        // BAR
#include "word.h"       // WORD

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
        g = ratio;
        
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
void word_DrawHud(const WORD *word, int x, int y, bool inDepth) {
    
    // Basic word HUD
    static ALLEGRO_BITMAP *hud = NULL;
    if (hud == NULL) {
        assert(hud = al_load_bitmap("data/image/hud.png"));
    }
    
    // Extended word HUD
    static ALLEGRO_BITMAP *hudExp = NULL;
    if (hudExp == NULL) {
        assert(hudExp = al_load_bitmap("data/image/hud_exp.png"));
    }
    
    // Choose whether to draw experience or not
    if (inDepth) {
        al_draw_bitmap(hudExp, x, y, 0);
    } else {
        al_draw_bitmap(hud, x, y, 0);
    }
    
    // Draw the word's name
    frame_DrawText(x+8, y+7, word->text);
    
    // Draw the word's level
    char levelString[4];
    sprintf(levelString, "%d", word->level);
    frame_DrawText(x+144, y+7, levelString);
    
    // Draw the word's health bar
    BAR health;
    health.x = x+6;
    health.y = y+21;
    health.width = 127;
    health.height = 5;
    health.ratio = (float)word->hp / word->stat[MAXHP];
    health.foreground = HealthColor(health.ratio);
    health.flags = BAR_NO_BACKGROUND;
    bar_Draw(&health);
    
    // Draw the word's health fraction
    char healthString[8];
    sprintf(healthString, "%d", word->hp);
    frame_DrawText(x+137, y+20, healthString);
    
    // Draw the word's experience bar
    if (inDepth) {
        BAR exp;
        exp.x = x+17;
        exp.y = y+33;
        exp.width = 151;
        exp.height = 2;
        exp.ratio = 1.0 - (float)word->exp / word->expNeed;
        exp.foreground = al_map_rgb(124, 118, 184);
        exp.flags = BAR_NO_BACKGROUND;
        bar_Draw(&exp);
    }
}

/*============================================================*/
