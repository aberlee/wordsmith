/**********************************************************//**
 * @file main.c
 * @brief Setup and game loop functions for the Wordsmith RPG.
 * @author Wes64
 **************************************************************/

// Standard library
#include <stdlib.h>         // malloc, srand
#include <stdbool.h>        // bool
#include <stdio.h>          // printf, fopen, fclose ...
#include <time.h>           // time

// Allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

// This project
#include "debug.h"          // eprintf, assert
#include "window.h"         // window size
#include "frame_rate.h"     // FrameRate
#include "frame.h"          // FRAME
#include "word_sprite.h"    // WORD_SPRITE
#include "word_frame.h"     // WORD_FRAME
#include "word_table.h"     // WORD_TABLE

//*************************************************************
/// The frame rate of the game.
#define FRAME_RATE 60.0

//*************************************************************
/// Debugging font.
static ALLEGRO_FONT *GlobalDebugFont;

static WORD Word;
static WORD_SPRITE Sprite;

/**********************************************************//**
 * @brief Program setup function.
 * @return Whether the setup succeeded. The program must be
 * aborted if this function fails. It also must be called only
 * once, before any other functions care called.
 **************************************************************/
static bool setup(void) {
    // Random number generator setup
    srand(time(NULL));

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
    RegisterTimer(&al_get_time);
    
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
    
    // Set up real word table
    if (!wordTable_Load("data/words/english.txt")) {
        eprintf("Failed to load the real word table.\n");
        return false;
    }
    
    // System setup
    GlobalDebugFont = al_load_ttf_font("data/font/wordsmith.ttf", 16, ALLEGRO_TTF_MONOCHROME);
    if (!GlobalDebugFont) {
        eprintf("Failed to load system debug font.\n");
        return false;
    }
    
    // Not error checking these because the effect will be
    // obvious if a resource is missing.
    wordSprite_Initialize();
    wordFrame_Initialize();
    
    // Game initialization
    if (!word_Create(&Word, "explosion", 69)) {
        eprintf("Unable to fuck.\n");
        return false;
    }
    wordSprite_Load(&Sprite, 100, 100, &Word);
    
    return true;
}

/**********************************************************//**
 * @brief Program cleanup function.
 * @return This is called when the program ends.
 **************************************************************/
static void cleanup(void) {
    // Destroy resources
    wordTable_Destroy();
}

/**********************************************************//**
 * @brief Screen rendering function.
 **************************************************************/
static void render(void) {
    // Draw the frame rate
    char buf[64];
    sprintf(buf, "%0.1lf FPS", FrameRate());
    al_draw_text(GlobalDebugFont, al_map_rgb(255, 255, 255), 1, 1, ALLEGRO_ALIGN_LEFT, buf);
    
    // Render stats
    //wordFrame_DrawHUD(&Word, 10, 10, HUD_BASIC);
    wordSprite_Draw(&Sprite);
}

/**********************************************************//**
 * @brief Update loop function.
 **************************************************************/
static bool update(float dt) {
    // Record the frame for frame rate
    RegisterFrame();
    
    // Update the sprites
    wordSprite_Update(&Sprite, dt);
    return true;
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
    al_set_new_display_flags(ALLEGRO_RESIZABLE);
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
    ALLEGRO_COLOR background = al_map_rgb(0, 0, 0);
    al_clear_to_color(background);
    al_flip_display();
    
    // Start game
    al_start_timer(timer);
    
    // Game loop
    ALLEGRO_EVENT event;
    bool running = true;
    bool redraw = false;
    float previous = al_get_time();
    float current;
    while (running) {
        // Get the next event
        al_wait_for_event(queue, &event);
        switch (event.type) {
        case ALLEGRO_EVENT_TIMER:
            // Compute the time step and update
            current = al_get_time();
            running = update(current-previous);
            previous = current;
            redraw = true;
            break;
            
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            running = false;
            break;
        
        default:
            break;
        }
        
        // Redraw the screen
        if (running && redraw && al_is_event_queue_empty(queue)) {
            al_clear_to_color(background);
            render();
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
