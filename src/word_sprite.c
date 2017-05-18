/**********************************************************//**
 * @file word_sprite.h
 * @brief Wordsmith word rendering implementations.
 **************************************************************/

// Standard library
#include <stddef.h>     // size_t
#include <stdbool.h>    // bool
#include <string.h>     // strlen

// Allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

// This project
#include "debug.h"      // assert, eprintf
#include "word.h"       // WORD
#include "word_sprite.h"// WORD_SPRITE

//**************************************************************
#define SPACING 32      ///< Spacing between word letters.

//**************************************************************
/// Font to use when drawing words.
static ALLEGRO_FONT *GlobalFont;   

void wordSprite_Initialize(void) {
    GlobalFont = al_load_ttf_font("data/font/wordsmith.ttf", 32, ALLEGRO_TTF_MONOCHROME);
}

/*============================================================*
 * Word rendering
 *============================================================*/
void word_DrawSprite(const WORD_SPRITE *sprite) {
    // Font dimensions
    int fontHeight = al_get_font_line_height(GlobalFont);
    int fontWidth;
    
    // Draw each letter, centering it at the declared origin
    int xDraw = sprite->xOrigin - (sprite->nLetters*SPACING) / 2;
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
        fontWidth = al_get_text_width(GlobalFont, string);
        
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
        al_draw_text(GlobalFont, current->color, 0, 0, ALLEGRO_ALIGN_LEFT, string);
        
        // Next leter
        xDraw += SPACING;
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

/*============================================================*/
