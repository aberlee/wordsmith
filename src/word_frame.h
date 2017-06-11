/**********************************************************//**
 * @file word_frame.h
 * @brief Wordsmith word data rendering header file.
 **************************************************************/

#ifndef _WORD_FRAME_H_
#define _WORD_FRAME_H_

// Standard library
#include <stdbool.h>    // bool

// This project
#include "word.h"       // WORD

#define WORD_HUD_WIDTH 172
#define WORD_HUD_HEIGHT_BASIC 32
#define WORD_HUD_HEIGHT_EXTENDED 39
#define WORD_HUD_HEIGHT_FULL 98

/**********************************************************//**
 * @enum HUD_MODE
 * @brief Rendering mode for the HUD.
 **************************************************************/
typedef enum {
    HUD_BASIC,      ///< Draw only the basic name, level, and HP.
    HUD_EXTENDED,   ///< Also draw extended stats such as experience.
    HUD_FULL,       ///< Draw all the word's stats.
} HUD_MODE;

/**********************************************************//**
 * @brief Initializes the word_frame module.
 **************************************************************/
extern void wordFrame_Initialize(void);

/**********************************************************//**
 * @brief Draw the word's heads-up display.
 * @param word: The word to display.
 * @param x: The x position of the frame.
 * @param y: The y position of the frame.
 * @param mode: The kind of HUD to draw.
 **************************************************************/
extern void wordFrame_DrawHUD(const WORD *word, int x, int y, HUD_MODE mode, bool selected);

/*============================================================*/
#endif // _WORD_FRAME_H_
