/**********************************************************//**
 * @file bar.c
 * @brief Implementation of stat bars.
 **************************************************************/

// Allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

// This project
#include "bar.h"        // BAR

/*============================================================*
 * Word loading
 *============================================================*/
void bar_Draw(const BAR *bar) {
    // Draw the bar
    int xo = bar->x;
    int xf = bar->x + bar->width;
    int yo = bar->y;
    int yf = bar->y + bar->height;
    
    // Ratio variables
    int xr = bar->x + bar->ratio*bar->width;
    
    // The background of the entire bar
    if (!(bar->flags & BAR_NO_BACKGROUND)) {
        al_draw_filled_rectangle(xo, yo, xf, yf, bar->background);
    }
    
    // Filled section
    if (xr > xo) {
        al_draw_filled_rectangle(xo, yo, xr, yf, bar->foreground);
    }
}

/*============================================================*/
