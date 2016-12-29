/**********************************************************//**
 * @file word_sprite.h
 * @brief Header file for rendering and animating words.
 **************************************************************/
 
#ifndef _WORD_SPRITE_H_
#define _WORD_SPRITE_H_

// Standard library
#include <stdbool.h>    // bool

// Allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

// This project
#include "word.h"       // WORD

/**********************************************************//**
 * @struct LETTER_SPRITE
 * @brief Defines how the letter is displayed on the screen.
 **************************************************************/
typedef struct {
    // Display properties
    char letter;        ///< The letter to display.
    ALLEGRO_COLOR color;///< The color of the letter.
    
    // Transformation and position
    float scaling;      ///< Scaling applied to the letter.
    float rotation;     ///< Rotation in radians of the letter.
    int xOffset;        ///< Letter x position.
    int yOffset;        ///< Letter y position.
} LETTER_SPRITE;

/**********************************************************//**
 * @struct WORD_SPRITE
 * @brief Defines how the word is displayed on the screen.
 **************************************************************/
typedef struct {
    LETTER_SPRITE letters[MAX_WORD_LENGTH]; ///< Letter sprites.
    int nLetters;       ///< Number of letters to draw.
    int xOrigin;        ///< Original x position.
    int yOrigin;        ///< Original y position.
} WORD_SPRITE;

/**********************************************************//**
 * @brief Set the font used for drawing words.
 * @param font: The font to use.
 * @param spacing: The distance between eltters.
 **************************************************************/
extern void word_SetFont(ALLEGRO_FONT *font, int spacing);

/**********************************************************//**
 * @brief Draw the word on the screen.
 * @param sprite: The word's sprite configuration.
 **************************************************************/
extern void word_DrawSprite(const WORD_SPRITE *sprite);

/**********************************************************//**
 * @brief Loads a sprite for the given word. This places the
 * sprite at the origin.
 * @param sprite: The sprite to load.
 * @param word: The word to turn into a sprite.
 **************************************************************/
extern void word_LoadSprite(WORD_SPRITE *sprite, const WORD *word);

/**********************************************************//**
 * @brief Sets the letter offsets for the given frame of the
 * looping idle animation.
 * @param sprite: The sprote to animate.
 * @param frame: The frame of animation to use (strictly
 * increasing and starts at 0).
 **************************************************************/
extern void word_AnimateIdle(WORD_SPRITE *sprite, int frame);

/*============================================================*/
#endif // _WORD_SPRITE_H_
