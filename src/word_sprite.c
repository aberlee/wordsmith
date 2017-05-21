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
#include "window.h"     // window size
#include "word.h"       // WORD
#include "word_sprite.h"// WORD_SPRITE

//**************************************************************
#define SPACING 12      ///< Spacing between word letters.
#define TRANSITION_TIME 0.3 ///< Length of an animation transition.

//**************************************************************
/// Font to use when drawing words.
static ALLEGRO_FONT *GlobalFont;   

/*============================================================*
 * Library initialization
 *============================================================*/
void wordSprite_Initialize(void) {
    GlobalFont = al_load_ttf_font("data/font/wordsmith.ttf", 32, ALLEGRO_TTF_MONOCHROME);
}

static float Offset(const WORD_SPRITE *sprite, int index) {
    return (index - sprite->nLetters/2.0)*SPACING;
}

/*============================================================*
 * Word rendering
 *============================================================*/
void wordSprite_Draw(const WORD_SPRITE *sprite) {
    // Draw each letter, centering it at the declared origin
    int fontHeight = al_get_font_line_height(GlobalFont);
    int xDraw = sprite->x;
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
        float x = xDraw + current->x + Offset(sprite, i);
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

static inline void ChangeAnimation(WORD_SPRITE *sprite, WORD_ANIMATION animate) {
    printf("Change\n");
    sprite->animate = animate;
    sprite->timer = 0.0;
    sprite->counter = 0;
}

static inline void ResetSprite(WORD_SPRITE *sprite) {
    printf("Reset\n");
    // Set initial X and Y positions
    for (int i = 0; i < sprite->nLetters; i++) {
        LETTER_SPRITE *current = &sprite->letters[i];
        current->x = 0.0;
        current->y = 0.0;
        current->opacity = 1.0;
        current->scaling = 1.0;
        current->rotation = 0.0;
        current->xv = 0.0;
        current->yv = 0.0;
        current->rv = 0.0;
        current->sv = 0.0;
    }
}

/*============================================================*
 * Sprite animation
 *============================================================*/
bool wordSprite_Update(WORD_SPRITE *sprite, float dt) {
    sprite->timer += dt;
    return sprite->animate(sprite, dt);
}

static inline float Period(const WORD_SPRITE *sprite, float period) {
    return fmod(sprite->timer, period) / period;
}

static inline float Tilt(const WORD_SPRITE *sprite, int index) {
    return 2*(index / (float)(sprite->nLetters - 1)) - 1;
}

static inline float IdleHeight(const WORD_SPRITE *sprite, int index) {
    float period = Period(sprite, 1.0);
    return 4*(1 + sin((period + index/16.0)*2*M_PI));
}

static inline bool SpriteAlignLeft(const WORD_SPRITE *sprite) {
    return sprite->x <= WINDOW_WIDTH/2.0;
}

/*============================================================*
 * Idle animation
 *============================================================*/
static bool AnimateIdle(WORD_SPRITE *sprite, float dt) {
    (void)dt;
    
    // Animate a sine wave for the motion.
    for (int i = 0; i < sprite->nLetters; i++) {
        LETTER_SPRITE *current = &sprite->letters[i];
        current->y = IdleHeight(sprite, i);
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
        float tilt = Tilt(sprite, i);
        
        // Initialize random explosion if first time.
        if (sprite->counter == 0) {
            current->x = 0.0;
            current->y = IdleHeight(sprite, i);
            current->scaling = 1.0;
            current->rotation = 0.0;
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
    sprite->counter = 1;
    return true;
}

/*============================================================*
 * Jumping animation
 *============================================================*/
static bool AnimateJump(WORD_SPRITE *sprite, float dt) {
    // Stop jumping check.
    int nDown = 0;
    for (int i = 0; i < sprite->nLetters; i++) {
        // Get the letter
        LETTER_SPRITE *current = &sprite->letters[i];
        
        // Make letters jump one-by-one.
        // This depends on the time step precision.
        if (i == sprite->counter && sprite->timer > i*0.05) {
            current->yv = 32;
            sprite->counter++;
        }
        
        // Update letter with Eulerian timestep
        current->y += 4*current->yv*dt;
        current->yv -= 128*dt;
        
        // Floor collision (coincide with idle animation)
        float idle = IdleHeight(sprite, i);
        if (current->y <= idle) {
            current->y = idle;
            nDown++;
        }
    }
    
    // Stop animating check
    if (sprite->counter >= sprite->nLetters && nDown == sprite->nLetters) {
        ChangeAnimation(sprite, &AnimateIdle);
        ResetSprite(sprite);
    }
    return true;
}

static bool AnimateEscape(WORD_SPRITE *sprite, float dt) {
    // Animation time
    const float MAX_TIME = 2.0;
    
    // Escape location
    float escapeX;
    float escapeY = WINDOW_HEIGHT/2.0;
    if (SpriteAlignLeft(sprite)) {
        escapeX = 0.0;
    } else {
        escapeX = WINDOW_WIDTH;
    }
    
    // Escape animation
    for (int i = 0; i < sprite->nLetters; i++) {
        // Get the letter
        LETTER_SPRITE *current = &sprite->letters[i];
        
        // Make letters jump one-by-one.
        // This depends on the time step precision.
        if (i == sprite->counter && sprite->timer > i*0.1) {
            current->xv = (escapeX - (sprite->x + current->x + Offset(sprite, i))) / MAX_TIME;
            current->yv = (escapeY - (sprite->y + IdleHeight(sprite, i))) / MAX_TIME;
            current->rv = current->xv / 16.0;
            current->sv = -0.1;
            sprite->counter++;
        }
        
        // Update letter with Eulerian timestep
        current->rotation += current->rv*dt;
        current->scaling += current->sv*dt;
        current->opacity = 1.0 - (sprite->timer/MAX_TIME);
        current->x += 2*current->xv*dt;
        current->y += 2*current->yv*dt;
    }
    return sprite->timer < MAX_TIME;
}

/*============================================================*
 * Word loading
 *============================================================*/
void wordSprite_Load(WORD_SPRITE *sprite, float x, float y, const WORD *word) {
    // Load each letter
    int length = strlen(word->text);
    for (int i = 0; i < length; i++) {
        sprite->letters[i].letter = word->text[i];
    }
    
    // Set constant data
    sprite->nLetters = length;
    sprite->x = x;
    sprite->y = y;
    sprite->animate = &AnimateEscape;
    sprite->timer = 0.0;
    sprite->counter = 0;
    
    // Reset sprite
    ResetSprite(sprite);
}

/*============================================================*/
