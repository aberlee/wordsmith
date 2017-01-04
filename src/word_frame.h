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

/**********************************************************//**
 * @brief Draw the word's heads-up display.
 * @param word: The word to display.
 * @param x: The x position of the frame.
 * @param y: The y position of the frame.
 * @param inDepth: Render the experience bar?
 **************************************************************/
void word_DrawHud(const WORD *word, int x, int y, bool inDepth);

/*============================================================*/
#endif // _WORD_FRAME_H_
