/**********************************************************//**
 * @file word_sprite.h
 * @brief Wordsmith word rendering implementations.
 **************************************************************/

// Standard library
#include <stddef.h>     // size_t
#include <stdbool.h>    // bool
#include <string.h>     // strlen
#include <math.h>       // sin, cos

// Allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

// This project
#include "debug.h"      // assert, eprintf
#include "word.h"       // WORD
#include "word_sprite.h"// WORD_SPRITE

/*============================================================*
 * Word font setup
 *============================================================*/
static ALLEGRO_FONT *font = NULL;   ///< Font to use when drawing words.
static int spacing = 0;             ///< Spacing to use when drawing words.

// Font configuration
void word_SetFont(ALLEGRO_FONT *newFont, int newSpacing) {
    font = newFont;
    spacing = newSpacing;
}

/*============================================================*
 * Word rendering
 *============================================================*/
void word_DrawSprite(const WORD_SPRITE *sprite) {
    
    // Must load the font first!
    assert(font);
    
    // Font dimensions
    int fontHeight = al_get_font_line_height(font);
    int fontWidth;
    
    // Draw each letter, centering it at the declared origin
    int xDraw = sprite->xOrigin - (sprite->nLetters*spacing) / 2;
    int yDraw = sprite->yOrigin - fontHeight;
    
    // Draw each letter
    const LETTER_SPRITE *current;
    ALLEGRO_TRANSFORM transform;
    char string[2] = {'\0', '\0'};  // Need null terminator!
    int x, y, tx, ty;
    for (int i = 0; i < sprite->nLetters; i++) {
        // Get the current letter
        current = &sprite->letters[i];
        
        // Get the draw position
        x = xDraw + current->xOffset;
        y = yDraw + current->yOffset;
        
        // Get the text to draw
        string[0] = current->letter;
        fontWidth = al_get_text_width(font, string);
        
        // Get a transformation matrix for the scaling and rotation
        al_identity_transform(&transform);
        
        // Center the letter
        tx = fontWidth / -2;
        ty = fontHeight / -2;
        al_translate_transform(&transform, tx, ty);
        
        // Rotate and scale the letter about its center
        al_rotate_transform(&transform, current->rotation);
        al_scale_transform(&transform, current->scaling, current->scaling);
        
        // Translate the letter to the proper position
        al_translate_transform(&transform, x-tx, y-ty);
        
        // Use this transformation matrix to draw the letter
        al_use_transform(&transform);
        al_draw_text(font, current->color, 0, 0, ALLEGRO_ALIGN_LEFT, string);
        
        // Next leter
        xDraw += spacing;
    }
    
    // Done drawing - restore transform
    al_identity_transform(&transform);
    al_use_transform(&transform);
}

/*============================================================*
 * Word loading
 *============================================================*/
void word_LoadSprite(WORD_SPRITE *sprite, const WORD *word) {
    
    // Load each letter
    int length = strlen(word->text);
    for (int i = 0; i < length; i++) {
        // Set letter data
        sprite->letters[i].letter = word->text[i];
        sprite->letters[i].color = al_map_rgb(255, 255, 255);
        sprite->letters[i].scaling = 1.0;
        sprite->letters[i].rotation = 0.0;
        sprite->letters[i].xOffset = 0;
        sprite->letters[i].yOffset = 0;
    }
    
    // Set constant data
    sprite->nLetters = length;
    sprite->xOrigin = 0;
    sprite->yOrigin = 0;
}

/*============================================================*
 * Word animation
 *============================================================*/
#define IDLE_AMPLITUDE 8.0  ///< Height of the letter animation.
#define IDLE_PERIOD 20.0    ///< Curviness of the word as a whole.

void word_AnimateIdle(WORD_SPRITE *sprite, int frame) {
    
    // Set the idle frame animation
    double theta;
    for (int i = 0; i < sprite->nLetters; i++) {
        // 4*atan(1.0) is PI
        theta = (double)(frame + i*IDLE_PERIOD) / 180 * 4*atan(1.0);
        sprite->letters[i].yOffset = IDLE_AMPLITUDE*sin(theta);
    }
}

/*============================================================*/
