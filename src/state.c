/**********************************************************//**
 * @file state.c
 * @author Wes64
 * @brief Implements game state interface.
 **************************************************************/

// Standard library
#include <stdbool.h>    // bool

// This project
#include "state.h"      // STATE

/// The current state.
static STATE current = STATE_INITIALIZER;

/*============================================================*
 * Changing to a new state
 *============================================================*/
void state_Transition(const STATE *state) {
    if (current.cleanup) {
        current.cleanup();
    }
    current = *state;
    if (current.setup) {
        current.setup();
    }
}

/*============================================================*
 * Interact with the current state
 *============================================================*/
bool state_Draw(void) {
    if (current.draw) {
        current.draw();
        return true;
    }
    // Not implemented
    return true;
}

bool state_Run(const ALLEGRO_EVENT *event) {
    if (current.run) {
        return current.run(event);
    }
    // Not implemented
    return true;
}

bool state_Update(double time) {
    if (current.update) {
        return current.update(time);
    }
    // Not implemented
    return true;
}

/*============================================================*/
