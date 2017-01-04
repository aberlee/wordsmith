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
    int ym = (yo + yf) / 2;
    
    // The background of the entire bar
    al_draw_filled_rectangle(xo, yo, xf, yf, bar->background);
    
    // Upper highlight of the bar
    al_draw_filled_rectangle(xo, yo, xr, ym, bar->foreground);
    
    // Lower underlay of the bar (use middle color)
    ALLEGRO_COLOR middleground;
    unsigned char ar, ag, ab, br, bg, bb;
    al_unmap_rgb(bar->foreground, &ar, &ag, &ab);
    al_unmap_rgb(bar->background, &br, &bg, &bb);
    middleground = al_map_rgb((ar+br) / 2, (ag+bg) / 2, (ab+bb) / 2);
    al_draw_filled_rectangle(xo, ym, xr, yf, middleground);
}

/*============================================================*/
