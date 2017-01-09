/**********************************************************//**
 * @file main.c
 * @brief Setup and game loop functions for the Wordsmith RPG.
 * @author Wes64
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
#include "frame.h"      // frame_SetTheme
#include "word_sprite.h"// word_SetFont
#include "word_table.h" // wordtable_Load

// Debugging
#include "test_state.h"

/*============================================================*
 * Display window
 *============================================================*/

/// The width of the display window in pixels.
#define WINDOW_WIDTH 640

/// The height of the display window in pixels.
#define WINDOW_HEIGHT 320

/// The frame rate of the game.
#define FRAME_RATE 60

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
    
    // Set up real word table
    if (!wordtable_Load("data/words/english.txt")) {
        eprintf("Failed to load the real word table.\n");
        return false;
    }
    
    // Initial state
    state_Initialize(InitialState());
    return true;
}

/**********************************************************//**
 * @brief Program cleanup function.
 * @return This is called when the program ends.
 **************************************************************/
static inline void cleanup(void) {
    // Destroy resources
    wordtable_Destroy();
}

/**********************************************************//**
 * @brief Handles the game loop.
 * @param argc: Number of command-line arguments.
 * @param argv: String values of the command-line arguments.
 * @return Exit code.
 **************************************************************/
int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    // Setup function
    if (!setup()) {
        return EXIT_FAILURE;
    }
    
    // Timer setup
    ALLEGRO_TIMER *timer;
    if (!(timer = al_create_timer(1.0 / FRAME_RATE))) {
        eprintf("Failed to create timer.\n");
        return EXIT_FAILURE;
    }
    
    // Display setup
    ALLEGRO_DISPLAY *display;
    if (!(display = al_create_display(WINDOW_WIDTH, WINDOW_HEIGHT))) {
        eprintf("Failed to create display.\n");
        return EXIT_FAILURE;
    }
    
    // Event queue setup
    ALLEGRO_EVENT_QUEUE *queue;
    if (!(queue = al_create_event_queue())) {
        eprintf("Failed to create an event queue.\n");
        return EXIT_FAILURE;
    }
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));
    
    // Set up screen
    ALLEGRO_COLOR background = al_map_rgb(200, 200, 200);
    al_clear_to_color(background);
    al_flip_display();
    
    // Start game
    al_start_timer(timer);
    
    // Game loop
    ALLEGRO_EVENT event;
    bool running = true;
    bool redraw = false;
    while (running) {
        // Get the next event
        al_wait_for_event(queue, &event);
        switch (event.type) {
        case ALLEGRO_EVENT_TIMER:
            // Calculate the real time of the frame.
            // Also only redraw on frame updates
            // for efficiency.
            // TODO does this cause errors?
            running = state_Update(al_get_time());
            redraw = true;
            break;
            
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            running = false;
            break;
        
        default:
            running = state_Run(&event);
        }
        
        // Redraw the screen
        if (running && redraw && al_is_event_queue_empty(queue)) {
            al_clear_to_color(background);
            if (!state_Draw()) {
                eprintf("Failed to draw the state.\n");
                return EXIT_FAILURE;
            }
            al_flip_display();
            redraw = false;
        }
    }
    
    // Allegro cleanup
    al_destroy_display(display);
    al_destroy_event_queue(queue);
    
    // Exit function
    cleanup();
    return EXIT_SUCCESS;
}

/*============================================================*/
