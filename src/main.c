/**********************************************************//**
 * @file main.c
 * @brief Main module for C Program.
 * Some "Hello World" code from gist.github.com/dradtke/2494024
 * @author Wes64
 * @mainpage
 * @htmlinclude main.html
 **************************************************************/

// Standard library
#include <stdlib.h>     // malloc, srand
#include <stdbool.h>    // bool
#include <stdio.h>      // printf, fopen, fclose ...
#include <time.h>       // time

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
 * Display window
 *============================================================*/

/// The width of the display window in pixels.
#define WINDOW_WIDTH 640

/// The height of the display window in pixels.
#define WINDOW_HEIGHT 320

/// The display window used by allegro.
static ALLEGRO_DISPLAY *display = NULL;

/// The game background color
static ALLEGRO_COLOR background;

/*============================================================*
 * Frame rate
 *============================================================*/

/// The frame rate of the game.
#define FRAME_RATE 60

/// The event queue used by allegro.
static ALLEGRO_EVENT_QUEUE *event_queue = NULL;

/// The timer used by allegro.
static ALLEGRO_TIMER *timer = NULL;

/**********************************************************//**
 * @brief Draws the screen.
 **************************************************************/
static void draw(void) {
    static int frame = 0;
    frame++;
    
    FRAME first;
    first.x = 10;
    first.y = 10;
    first.width = 100;
    first.height = 20;
    first.flags = FRAME_OUTLINE | FRAME_HEADER;
    frame_Draw(&first);
    
    TEXT_ENTRY entries[4];
    entries[0].text = "Hello";
    entries[0].flags = ENTRY_SELECTED;
    entries[1].text = "World";
    entries[1].flags = ENTRY_DISABLED;
    entries[2].text = "Hello";
    entries[2].flags = ENTRY_HIGHLIGHT;
    entries[3].text = "Hello World";
    entries[3].flags = ENTRY_SELECTED | ENTRY_HIGHLIGHT;
    
    TEXT_FRAME second;
    second.x = 120;
    second.y = 10;
    second.maxWidth = -1;
    second.lines = 4;
    second.data = entries;
    second.flags = FRAME_OUTLINE | FRAME_DYNAMIC_WIDTH;
    textframe_Draw(&second);
    
    WORD word;
    word_Create(&word, "Test", 100);
    word.hp /= 2;
    word.exp /= 2;
    
    WORD_SPRITE sprite;
    word_LoadSprite(&sprite, &word);
    sprite.xOrigin = 300;
    sprite.yOrigin = 150;
    
    word_AnimateIdle(&sprite, frame*8);
    word_DrawSprite(&sprite);
    
    word_DrawHud(&word, 300, 10, true);
}

/**********************************************************//**
 * @brief Program game loop function.
 * @return Whether the program should run again.
 **************************************************************/
static bool run(const ALLEGRO_EVENT *event) {
    
    // Event runner
    switch (event->type) {
    
    // The display was destroyed, can't continue to run.
    case ALLEGRO_EVENT_DISPLAY_CLOSE:
        return false;
    
    // Timer events
    case ALLEGRO_EVENT_TIMER:
        return true;
    
    default:
        eprintf("Unsupported event received: %d\n", event->type);
        break;
    }
    
    // Keep running
    return true;
}

/**********************************************************//**
 * @brief The test state.
 **************************************************************/
static const STATE INITIAL_STATE = { &draw, &run };

/**********************************************************//**
 * @brief Program setup function.
 * @return Whether the setup succeeded. The program must be
 * aborted if this function fails. It also must be called only
 * once, before any other functions care called.
 **************************************************************/
static inline bool setup(void) {
    
    // Random number generator setup
#ifdef DEBUG
    srand(42);
#else
    srand(time(NULL));
#endif

    // Allegro setup
    if (!al_init()) {
        eprintf("Failed to initialize allegro.\n");
        return false;
    }
    if (!al_install_audio()) {
        eprintf("Failed to initialize allegro audio addon.\n");
        return false;
    }
    al_init_font_addon();
    if (!al_init_ttf_addon()) {
        eprintf("Failed to initialize allegro ttf addon.\n");
        return false;
    }
    if (!al_init_image_addon()) {
        eprintf("Failed to initialize allegro image addon.\n");
        return false;
    }
    if (!al_init_primitives_addon()) {
        eprintf("Failed to initialize allegro primitives addon.\n");
        return false;
    }
    
    // Keyboard setup
    if (!al_install_keyboard()) {
        eprintf("Failed to install keyboard.\n");
        return false;
    }
    
    // Blender setup
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
    
    // Timer setup
    if (!(timer = al_create_timer(1.0 / FRAME_RATE))) {
        eprintf("Failed to create timer.\n");
        return false;
    }
    
    // Display setup
    if (!(display = al_create_display(WINDOW_WIDTH, WINDOW_HEIGHT))) {
        eprintf("Failed to create display.\n");
        return false;
    }
    
    // Event queue setup
    if (!(event_queue = al_create_event_queue())) {
        eprintf("Failed to create an event queue.\n");
        return false;
    }
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    
    // Set up screen
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();
    
    // Set up theme
    THEME theme;
    theme.font = al_load_ttf_font("data/font/wordsmith.ttf", 16, ALLEGRO_TTF_MONOCHROME);
    if (!theme.font) {
        eprintf("Failed to load the font.\n");
        return false;
    }
    theme.foreground = al_map_rgb(42, 42, 42);
    theme.background = al_map_rgb(255, 255, 255);
    theme.highlight = al_map_rgb(207, 82, 82);
    theme.disabled = al_map_rgb(128, 128, 128);
    theme.outline = 1;
    theme.padding = 2;
    theme.header = 4;
    theme.spacing = 2;
    frame_SetTheme(&theme);
    
    // Set up word font
    ALLEGRO_FONT *wordFont = al_load_ttf_font("data/font/wordsmith.ttf", 32, ALLEGRO_TTF_MONOCHROME);
    if (!wordFont) {
        eprintf("Failed to load the word font.\n");
        return false;
    }
    word_SetFont(wordFont, 16);
    
    // Set up background color
    background = al_map_rgb(200, 200, 200);
    
    // Initial state
    if (!state_Push(&INITIAL_STATE)) {
        eprintf("Failed to start initial state.\n");
        return false;
    }
    
    // Start game
    al_start_timer(timer);
    return true;
}

/**********************************************************//**
 * @brief Program cleanup function.
 * @return This is called when the program ends.
 **************************************************************/
static inline void cleanup(void) {
    // Destroy resources
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
}

/**********************************************************//**
 * @brief Driver function.
 **************************************************************/
int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    // Setup function
    if (!setup()) {
        return EXIT_FAILURE;
    }
    
    // Game loop
    ALLEGRO_EVENT event;
    bool running = true;
    while (running) {
        // Get the next event
        al_wait_for_event(event_queue, &event);
        
        // Check for redrawing
        if (event.type == ALLEGRO_EVENT_TIMER) {
            if (al_is_event_queue_empty(event_queue)) {
                // Redraw the entire state
                al_clear_to_color(background);
                if (!state_Draw()) {
                    eprintf("Failed to draw the state.\n");
                    return EXIT_FAILURE;
                }
                al_flip_display();
            }
        }
        running = state_Run(&event);
    }
    
    // Exit function
    cleanup();
    return EXIT_SUCCESS;
}

/*============================================================*/
