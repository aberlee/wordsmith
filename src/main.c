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

// Debugging libraries
#include "debug.h"

/*============================================================*
 * Variables
 *============================================================*/

/// The frame rate of the game.
#define FRAME_RATE 60.0

/// The width of the display window in pixels.
#define WINDOW_WIDTH 640

/// The height of the display window in pixels.
#define WINDOW_HEIGHT 320

/// The display window used by allegro.
static ALLEGRO_DISPLAY *display = NULL;

/// The event queue used by allegro.
static ALLEGRO_EVENT_QUEUE *event_queue = NULL;

/// The timer used by allegro.
static ALLEGRO_TIMER *timer = NULL;

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
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    
    // Set up screen
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();
    
    // Start timer
    al_start_timer(timer);
    
    // Succeeded at setup
    return true;
}

/**********************************************************//**
 * @brief Draws the screen.
 **************************************************************/
static inline void draw(void) {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();
}

/**********************************************************//**
 * @brief Program game loop function.
 * @return Whether the program should run again.
 **************************************************************/
static inline bool run(const ALLEGRO_EVENT *event) {
    
    // Event runner
    switch (event->type) {
    case ALLEGRO_EVENT_TIMER:
        // Redraw the screen
        if (al_is_event_queue_empty(event_queue)) {
            draw();
        }
        break;
    
    case ALLEGRO_EVENT_DISPLAY_CLOSE:
        return false;
    
    default:
        eprintf("Unsupposted event received: %d\n", event->type);
        return false;
    }
    
    // Keep running
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
        running = run(&event);
    }
    
    // Exit function
    cleanup();
    return EXIT_SUCCESS;
}

/*============================================================*/
