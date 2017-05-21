/**********************************************************//**
 * @file word_sprite.h
 * @brief Wordsmith word rendering implementations.
 **************************************************************/

// Standard library
#include <stddef.h>     // size_t
#include <stdbool.h>    // bool
#include <string.h>     // strlen
#include <math.h>       // fabs, sin

// Allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

// This project
#include "debug.h"      // assert, eprintf
#include "random.h"     // uniform
#include "word.h"       // WORD
#include "word_sprite.h"// WORD_SPRITE

//**************************************************************
#define SPACING 12      ///< Spacing between word letters.
#define TRANSITION_TIME 0.1 ///< Length of an animation transition.

//**************************************************************
/// Font to use when drawing words.
static ALLEGRO_FONT *GlobalFont;   

/*============================================================*
 * Library initialization
 *============================================================*/
void wordSprite_Initialize(void) {
    GlobalFont = al_load_ttf_font("data/font/wordsmith.ttf", 32, ALLEGRO_TTF_MONOCHROME);
}

/*============================================================*
 * Word rendering
 *============================================================*/
void wordSprite_Draw(const WORD_SPRITE *sprite) {
    // Draw each letter, centering it at the declared origin
    int fontHeight = al_get_font_line_height(GlobalFont);
    int xDraw = sprite->x - (sprite->nLetters*SPACING) / 2;
    int yDraw = sprite->y - fontHeight;
    
    // Old transformation matrix
    ALLEGRO_TRANSFORM old;
    al_copy_transform(&old, al_get_current_transform());
    
    // Draw each latter
    ALLEGRO_TRANSFORM transform;
    char string[2] = {'\0', '\0'};  // Need null terminator!
    for (int i = 0; i < sprite->nLetters; i++) {
        // Get the current letter
        const LETTER_SPRITE *current = &sprite->letters[i];
        float x = xDraw + current->x + (i*SPACING);
        float y = yDraw - current->y;
        
        // Get the text to draw
        string[0] = current->letter;
        int fontWidth = al_get_text_width(GlobalFont, string);
        
        // Center the letter
        al_copy_transform(&transform, &old);
        float tx = -fontWidth / 2.0;
        float ty = -fontHeight / 2.0;
        al_translate_transform(&transform, tx, ty);
        
        // Rotate and scale the letter about its center
        al_rotate_transform(&transform, current->rotation);
        al_scale_transform(&transform, current->scaling, current->scaling);
        
        // Translate the letter to the proper position
        al_translate_transform(&transform, x, y);
        
        // Use this transformation matrix to draw the letter
        al_use_transform(&transform);
        ALLEGRO_COLOR color = al_map_rgba(255, 255, 255, 255*current->opacity);
        al_draw_text(GlobalFont, color, 0, 0, ALLEGRO_ALIGN_LEFT, string);
    }
    
    // Done drawing - restore transform
    al_use_transform(&old);
}

/*============================================================*
 * Sprite animation
 *============================================================*/
bool wordSprite_Update(WORD_SPRITE *sprite, float dt) {
    // Exit transition check
    if (sprite->next != NULL && sprite->transition+dt >= TRANSITION_TIME) {
        sprite->timer = 0.0;
        sprite->transition = 0.0;
        sprite->animate = sprite->next;
        sprite->next = NULL;
    }
    
    // Not a transition phase, animate as usual.
    if (sprite->next == NULL) {
        bool ret = sprite->animate(sprite, dt);
        sprite->timer += dt;
        return ret;
    }
    
    // Set up the copy timer to play from the beginning
    WORD_SPRITE copy = *sprite;
    copy.timer = 0.0;
    
    // Get animation transitions to interpolate
    sprite->next(&copy, dt);
    sprite->animate(sprite, dt);
    sprite->transition += dt;
    
    // Interpolate the phases
    float secondWeight = sprite->transition / TRANSITION_TIME;
    float firstWeight = 1.0 - secondWeight;
    for (int i = 0; i < sprite->nLetters; i++) {
        // Get letters to interpolate
        const LETTER_SPRITE *second = &copy.letters[i];
        LETTER_SPRITE *first = &sprite->letters[i];
        
        // Interpolate animation stats
        first->opacity = (first->opacity*firstWeight) + (second->opacity*secondWeight);
        first->scaling = (first->scaling*firstWeight) + (second->scaling*secondWeight);
        first->rotation = (first->rotation*firstWeight) + (second->rotation*secondWeight);
        first->x = (first->x*firstWeight) + (second->x*secondWeight);
        first->y = (first->y*firstWeight) + (second->y*secondWeight);
    }
    return true;
}

static inline float Period(WORD_SPRITE *sprite, float period) {
    return fmod(sprite->timer, period) / period;
}

/*============================================================*
 * Idle animation
 *============================================================*/
static bool AnimateExplode(WORD_SPRITE *sprite, float dt);
 
static bool AnimateIdle(WORD_SPRITE *sprite, float dt) {
    (void)dt;
    
    // Reset all the letters to begin
    if (sprite->timer == 0.0) {
        // Set initial X and Y positions
        for (int i = 0; i < sprite->nLetters; i++) {
            LETTER_SPRITE *current = &sprite->letters[i];
            current->x = 0.0;
            current->opacity = 1.0;
            current->scaling = 1.0;
            current->rotation = 0.0;
            current->xv = 0.0;
            current->yv = 0.0;
            current->rv = 0.0;
            current->sv = 0.0;
        }
    }
    
    // Animate a sine wave for the motion.
    float period = Period(sprite, 1.0);
    for (int i = 0; i < sprite->nLetters; i++) {
        sprite->letters[i].y = 4*sin((period + i/16.0)*2*M_PI);
    }
    
    if (sprite->timer > 2.0) {
        sprite->next = &AnimateExplode;
    }
    return true;
}

/*============================================================*
 * Explosion animation
 *============================================================*/
static bool AnimateExplode(WORD_SPRITE *sprite, float dt) {
    // Check if this is the first time the animation
    // has played for this sprite, or whether we are
    // continuing.
    const float MAX_TIME = 2.0;
    float phase = sprite->timer;
    if (phase >= MAX_TIME) {
        // Hide the sprite
        for (int i = 0; i < sprite->nLetters; i++) {
            sprite->letters[i].opacity = 0.0;
        }
        return false;
    }
    
    // Animate the word exploding.
    for (int i = 0; i < sprite->nLetters; i++) {
        // Get the letter
        LETTER_SPRITE *current = &sprite->letters[i];
        
        // Get the tilt of the letter within the shape.
        // First letter is -1, middle is 0, last is +1.
        float tilt = 2*(i / (float)(sprite->nLetters - 1)) - 1;
        
        // Initialize random explosion if first time.
        if (phase == 0.0) {
            current->xv = (tilt + uniform(-0.5, 0.5))*16;
            current->yv = (2-fabs(tilt))*48;
            current->sv = uniform(-0.5, 0.5);
            current->rv = 2*(tilt+uniform(-0.75, 0.75));
        }
        
        // Update letter with Eulerian timestep
        current->rotation += current->rv*dt;
        current->scaling += current->sv*dt;
        current->opacity = 1.0 - (phase/MAX_TIME);
        current->x += 2*current->xv*dt;
        current->y += 2*current->yv*dt;
        current->yv -= 128*dt;
        current->xv -= 0.1*dt*current->xv;
        
        // Floor collision
        if (current->y < 0.0) {
            current->y = 0.0;
            current->yv *= -0.8;
        }
    }
    return true;
}

/*============================================================*
 * Word loading
 *============================================================*/
void wordSprite_Load(WORD_SPRITE *sprite, float x, float y, const WORD *word) {
    // Load each letter
    int length = strlen(word->text);
    for (int i = 0; i < length; i++) {
        // Set letter data
        sprite->letters[i].letter = word->text[i];
        sprite->letters[i].opacity = 1.0;
        sprite->letters[i].scaling = 1.0;
        sprite->letters[i].rotation = 0.0;
        sprite->letters[i].x = 0;
        sprite->letters[i].y = 0;
    }
    
    // Set constant data
    sprite->nLetters = length;
    sprite->x = x;
    sprite->y = y;
    sprite->animate = &AnimateIdle;
    sprite->next = NULL;
    sprite->timer = 0.0;
    sprite->transition = 0.0;
}

/*============================================================*/
