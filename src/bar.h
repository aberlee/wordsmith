/**********************************************************//**
 * @file bar.h
 * @brief Header file for rendering stat bars.
 **************************************************************/

#ifndef _BAR_H_
#define _BAR_H_

// Allegro
#include <allegro5/allegro.h>

/**********************************************************//**
 * @enum BAR_FLAGS
 * @brief Stores all the rendering flags for bars.
 **************************************************************/
typedef enum {
    BAR_NO_BACKGROUND=1,
} BAR_FLAGS;

/**********************************************************//**
 * @struct BAR
 * @brief Stores all information needed to render a stat bar.
 **************************************************************/
typedef struct {
    int x;                      ///< The x position of the upper left.
    int y;                      ///< The y position of the upper left.
    int width;                  ///< Width of the full bar.
    int height;                 ///< Height of the full bar.
    ALLEGRO_COLOR foreground;   ///< The foreground color.
    ALLEGRO_COLOR background;   ///< The background color.
    float ratio;                ///< How full the bar is (0.0 to 1.0).
    BAR_FLAGS flags;            ///< Rendering flags.
} BAR;

/**********************************************************//**
 * @brief Draws the bar on the screen.
 * @param bar: The bar to draw.
 **************************************************************/
void bar_Draw(const BAR *bar);

/*============================================================*/
#endif
