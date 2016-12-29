/**********************************************************//**
 * @file state.c
 * @author Wes64
 * @brief Implements game state interface.
 **************************************************************/

// Standard library
#include <stdbool.h>    // bool

// This project
#include "state.h"      // STATE

/*============================================================*
 * State stack
 *============================================================*/
#define STATE_STACK_SIZE 64             // Size of state stack
static STATE stack[STATE_STACK_SIZE];   // The state stack.
static int current = -1;                // Top of the state stack.

/*============================================================*
 * Changing to a new state
 *============================================================*/
bool state_Push(const STATE *state) {
    if (current+1 < STATE_STACK_SIZE) {
        current++;
        stack[current] = *state;
        return true;
    }
    return false;
}

/*============================================================*
 * Return to a previous state
 *============================================================*/
bool state_Pop(void) {
    if (current >= 0) {
        current--;
        return true;
    }
    return false;
}

/*============================================================*
 * Draw the current state
 *============================================================*/
bool state_Draw(void) {
    if (current >= 0) {
        stack[current].draw();
        return true;
    }
    return false;
}

/*============================================================*
 * Run the current state
 *============================================================*/
bool state_Run(const ALLEGRO_EVENT *event) {
    if (current >= 0) {
        return stack[current].run(event);
    }
    return false;
}

/*============================================================*/
