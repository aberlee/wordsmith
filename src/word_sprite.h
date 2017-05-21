/**********************************************************//**
 * @file word_sprite.h
 * @brief Header file for rendering and animating words.
 **************************************************************/
 
#ifndef _WORD_SPRITE_H_
#define _WORD_SPRITE_H_

// Standard library
#include <stdbool.h>        // bool

// Allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

// This project
#include "word.h"           // WORD

/**********************************************************//**
 * @struct LETTER_SPRITE
 * @brief Defines how the letter is displayed on the screen.
 **************************************************************/
typedef struct {
    char letter;            ///< The letter to display.
    float opacity;          ///< Opacity of the letter.
    float scaling;          ///< Scaling applied to the letter.
    float rotation;         ///< Rotation in radians of the letter.
    float x;                ///< Letter X offset position.
    float y;                ///< Letter Y offset position.
    float xv;               ///< X velocity.
    float yv;               ///< Y velicity.
    float rv;               ///< Rotational velocity.
    float sv;               ///< Size velocity.
} LETTER_SPRITE;

/**********************************************************//**
 * @struct WORD_SPRITE
 * @brief Defines how the word is displayed on the screen.
 **************************************************************/
typedef struct WORD_SPRITE {
    // Letters
    LETTER_SPRITE letters[MAX_WORD_LENGTH]; ///< Letter sprites.
    int nLetters;           ///< Number of letters to draw.
    
    // Overall position
    float x;                ///< Word origin X position.
    float y;                ///< Word origin Y position.
    float timer;            ///< Animation timer.
    int counter;            ///< Animation letter counter.
    
    /// Current animation behavior.
    bool (*animate)(struct WORD_SPRITE *sprite, float dt);
} WORD_SPRITE;

/**********************************************************//**
 * @typedef WORD_ANIMATION
 * @brief Animation function type for animating a WORD_SPRITE
 * as a particle system.
 * @param sprite: The sprite to animate.
 * @return Whether the sprite continues existing.
 **************************************************************/
typedef bool (*WORD_ANIMATION)(WORD_SPRITE *sprite, float dt);

/**********************************************************//**
 * @brief Initializes the word_sprite module.
 **************************************************************/
extern void wordSprite_Initialize(void);

/**********************************************************//**
 * @brief Draw the word on the screen.
 * @param sprite: The word's sprite configuration.
 **************************************************************/
extern void wordSprite_Draw(const WORD_SPRITE *sprite);

/**********************************************************//**
 * @brief Update the word's animation.
 * @param sprite: The word's sprite configuration.
 * @param dt: The time update.
 * @return Whether the sprite should be deleted.
 **************************************************************/
extern bool wordSprite_Update(WORD_SPRITE *sprite, float dt);

/**********************************************************//**
 * @brief Loads a sprite for the given word. This places the
 * sprite at the origin.
 * @param sprite: The sprite to load.
 * @param x: Anchor x position.
 * @param y: Anchor y position.
 * @param word: The word to turn into a sprite.
 **************************************************************/
extern void wordSprite_Load(WORD_SPRITE *sprite, float x, float y, const WORD *word);

/*============================================================*/
#endif // _WORD_SPRITE_H_
