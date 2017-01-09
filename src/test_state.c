/**********************************************************//**
 * @file test_state.c
 * @brief Implements the testing state for Wordsmith.
 * @author Wes64
 **************************************************************/

// Standard library
#include <stdbool.h>    // bool

// Allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

// This project
#include "debug.h"      // eprintf, assert
#include "state.h"      // STATE
#include "frame.h"      // frame_Draw
#include "word.h"       // WORD
#include "word_sprite.h"// WORD_SPRITE
#include "word_frame.h" // word_DrawHud


/*============================================================*
 * State variables
 *============================================================*/
static FRAME first;
static TEXT_ENTRY entries[4];
static TEXT_FRAME second;
static WORD word;
static WORD_SPRITE sprite;

/**********************************************************//**
 * @brief Initializes the current state.
 **************************************************************/
static void setup(void) {
    first.x = 10;
    first.y = 10;
    first.width = 100;
    first.height = 20;
    first.flags = FRAME_OUTLINE | FRAME_HEADER;

    entries[0].text = "Hello";
    entries[0].flags = ENTRY_SELECTED;
    entries[1].text = "World";
    entries[1].flags = ENTRY_DISABLED;
    entries[2].text = "Hello";
    entries[2].flags = ENTRY_HIGHLIGHT;
    entries[3].text = "Hello World";
    entries[3].flags = ENTRY_SELECTED | ENTRY_HIGHLIGHT;

    second.x = 120;
    second.y = 10;
    second.maxWidth = -1;
    second.lines = 4;
    second.data = entries;
    second.flags = FRAME_OUTLINE | FRAME_DYNAMIC_WIDTH;
    
    word_Create(&word, "shit", 1);
    word.hp *= 0.45;
    word.exp /= 2;

    word_LoadSprite(&sprite, &word);
    sprite.xOrigin = 300;
    sprite.yOrigin = 150;
}

/**********************************************************//**
 * @brief Draws the screen.
 **************************************************************/
static void draw(void) {
    frame_Draw(&first);
    textframe_Draw(&second);
    word_DrawSprite(&sprite);
    word_DrawHud(&word, 300, 10, HUD_FULL);
}

/**********************************************************//**
 * @brief Program frame update function.
 * @param time: The time since the last frame.
 **************************************************************/
static bool update(double time) {
    word_AnimateIdle(&sprite, 500*time);
    return true;
}

/**********************************************************//**
 * @brief The test state.
 **************************************************************/
static const STATE INITIAL_STATE = {&setup, &draw, NULL, &update, NULL};

/*============================================================*
 * Initial state
 *============================================================*/
const STATE *InitialState(void) {
    return &INITIAL_STATE;
}

/*============================================================*/
