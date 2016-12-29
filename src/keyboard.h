/**********************************************************//**
 * @file keyboard.h
 * @author Wes64
 * @brief Defines the standard keys we care about.
 **************************************************************/

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

// Allegro
#include <allegro5/allegro.h>

/**********************************************************//**
 * @enum KEY
 * @brief All the keys used for user input.
 **************************************************************/
typedef enum {
    KEY_UP=ALLEGRO_KEY_UP,          ///< Moves up.
    KEY_DOWN=ALLEGRO_KEY_DOWN,      ///< Moves down.
    KEY_LEFT=ALLEGRO_KEY_LEFT,      ///< Moves left.
    KEY_RIGHT=ALLEGRO_KEY_RIGHT,    ///< Moves right.
    KEY_START=ALLEGRO_KEY_W,        ///< Start key (begin, menu).
    KEY_SELECT=ALLEGRO_KEY_A,       ///< Select key (special).
    KEY_A=ALLEGRO_KEY_D,            ///< Confirm a choice.
    KEY_B=ALLEGRO_KEY_S,            ///< Deny a choice.
} KEY;

/*============================================================*/
#endif // _KEYBOARD_H_
