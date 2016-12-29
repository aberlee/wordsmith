/**********************************************************//**
 * @file state.h
 * @author Wes64
 * @brief Defines a standard interface to game states.
 **************************************************************/

#ifndef _STATE_H_
#define _STATE_H_

// Standard library
#include <stdbool.h>    // bool

// Allegro
#include <allegro5/allegro.h>

/**********************************************************//**
 * @struct STATE
 * @brief Provides a set of functions that control the game
 * state at any time.
 **************************************************************/
typedef struct {
    void (*draw)(void);     ///< Function to draw the state.
    bool (*run)(const ALLEGRO_EVENT *); ///< Runs the state.
} STATE;

/**********************************************************//**
 * @brief Draws the current state.
 * @return Whether the drawing succeeded.
 **************************************************************/
extern bool state_Draw(void);

/**********************************************************//**
 * @brief Runs one frame of the current state.
 * @return Whether the run succeeded.
 **************************************************************/
extern bool state_Run(const ALLEGRO_EVENT *event);

/**********************************************************//**
 * @brief Changes to a new state.
 * @param state: The next state. This can be deleted after.
 * @return Whether the state change succeeded.
 **************************************************************/
extern bool state_Push(const STATE *state);

/**********************************************************//**
 * @brief Returns to the previous state.
 * @return Whether the state change succeeded.
 **************************************************************/
extern bool state_Pop(void);

/*============================================================*/
#endif // _STATE_H_
