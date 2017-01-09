/**********************************************************//**
 * @file state.h
 * @author Wes64
 * @brief Defines a standard interface to game states.
 **************************************************************/

#ifndef _STATE_H_
#define _STATE_H_

// Standard library
#include <stddef.h>     // NULL
#include <stdbool.h>    // bool

// Allegro
#include <allegro5/allegro.h>

/**********************************************************//**
 * @struct STATE
 * @brief Provides a set of functions that control the game
 * state at any time.
 **************************************************************/
typedef struct {
    /// @brief Initializes the current state. This function is
    /// called whenever the state is transitioned to.
    void (*setup)(void);
    
    /// @brief Renders the current state.
    void (*draw)(void);
    
    /// @brief Runs the current state. This is used to handle
    /// all events other than frame refresh events.
    /// @param event: The user input event to handle.
    /// @return Whether the execution succeeded.
    bool (*run)(const ALLEGRO_EVENT *event);
    
    /// @brief Update the state. This function handles frame
    /// refresh events.
    /// @param time: The real time since the last update.
    /// @return Whether the execution succeeded.
    bool (*update)(double time);  

    /// @brief Cleans up the current state. This function is
    /// called whenever the state is transitioned away from.
    void (*cleanup)(void);
    
} STATE;

/// The null state.
#define STATE_INITIALIZER {NULL}

/**********************************************************//**
 * @brief Draws the current state.
 * @return Whether the drawing succeeded.
 **************************************************************/
extern bool state_Draw(void);

/**********************************************************//**
 * @brief Handles a user input event with the current state.
 * @param event: The event to handle.
 * @return Whether the run succeeded.
 **************************************************************/
extern bool state_Run(const ALLEGRO_EVENT *event);

/**********************************************************//**
 * @brief Updates the state on every frame.
 * @param time: The time since the last frame.
 **************************************************************/
extern bool state_Update(double time);

/**********************************************************//**
 * @brief Changes to a new state.
 * @param state: The next state. This can be deleted after.
 **************************************************************/
extern void state_Transition(const STATE *state);

/**********************************************************//**
 * @brief Initializes the first state.
 * @param state: The initial state's data. This data can be
 * destroyed later.
 **************************************************************/
#define state_Initialize(state) state_Transition(state)

/*============================================================*/
#endif // _STATE_H_
