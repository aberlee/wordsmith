/**********************************************************//**
 * @file frame.h
 * @author Wes64
 * @brief Defines structs and drawing functions for user
 * interface graphics on the screen.
 **************************************************************/

#ifndef _FRAME_H_
#define _FRAME_H_

// Allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

/**********************************************************//**
 * @struct THEME
 * @brief Stores global window color and font information.
 **************************************************************/
typedef struct {
    const ALLEGRO_FONT *font;   ///< The font used to render text.
    ALLEGRO_COLOR foreground;   ///< The color of text and foreground elements.
    ALLEGRO_COLOR background;   ///< The background color used for frames.
    ALLEGRO_COLOR highlight;    ///< The highlight color for important information.
    ALLEGRO_COLOR disabled;     ///< Color for disabled text items.
} THEME;

/**********************************************************//**
 * @brief Sets the global frame theme. Resources associated
 * to the old theme must be freed by the programmer.
 * @param theme: The new theme to copy.
 **************************************************************/
extern void frame_SetTheme(const THEME *theme);

/**********************************************************//**
 * @brief Gets a pointer to the current global frame theme.
 * @return A pointer to the current theme.
 **************************************************************/
extern const THEME *frame_GetTheme(void);

/**********************************************************//**
 * @struct FRAME
 * @brief Stores the configuration for one graphics frame on
 * the screen. The graphics are guaranteed not to extend out
 * of the frame's area.
 **************************************************************/
typedef struct {
    int x;      ///< The x-coordinate of the origin.
    int y;      ///< The y-coordinate of the origin.
    int width;  ///< The width of the area.
    int height; ///< The height of the area.
    int flags;  ///< Rendering configuration for the frame.
} FRAME;

/// Flag to specify whether the frame should be outlined with
/// the foreground color.
#define FRAME_OUTLINE 1

/// Flag to specify whether the frame should be drawn with a
/// highlighted header box.
#define FRAME_HEADER 2

/**********************************************************//**
 * @brief Draws the frame on the screen using the current theme.
 * @param frame: Pointer to a frame to draw.
 **************************************************************/
extern void frame_Draw(const FRAME *frame);

/**********************************************************//**
 * @struct TEXT_ENTRY
 * @brief Stores setting for one element of text in a
 * TEXT_FRAME.
 **************************************************************/
typedef struct {
    const char *text;
    int flags;
} TEXT_ENTRY;

/// The text entry is disabled, and can't be selected.
#define ENTRY_DISABLED 1

/// The text entry is special and should be highlighted.
#define ENTRY_HIGHLIGHT 2

/// The text entry is selected and should be rendered in inverse,
/// with a rectangle highlighting it from behind.
#define ENTRY_SELECTED 4

/**********************************************************//**
 * @struct TEXT_FRAME
 * @brief Stores the configuration for one graphics frame on
 * the screen. The graphics are guaranteed not to extend out
 * of the frame's area.
 **************************************************************/
typedef struct {
    int x;                  ///< The x position of the frame.
    int y;                  ///< The y position of the frame.
    int maxWidth;           ///< The maximum width of any line of text.
    int lines;              ///< The number of lines of text.
    const TEXT_ENTRY *data; ///< The actual text to render.
    int flags;              ///< Rendering flags for the frame.
} TEXT_FRAME;

/// Using this flag instead of a maxWidth for a TEXT_FRAME will
/// allow for dynamic computation of the frame width based on
/// the content of the textData array.
#define TEXT_FRAME_DYNAMIC_WIDTH -1

/**********************************************************//**
 * @brief Draws the text frame and the text it contains based
 * on the current theme.
 * @param frame: Pointer to a text frame to draw.
 **************************************************************/
extern void textframe_Draw(const TEXT_FRAME *frame);

/**********************************************************//**
 * @struct MENU
 * @brief A frame for giving the user a selection of options.
 **************************************************************/
typedef struct {
    int x;
    int y;
    int maxWidth;
    int maxLines;
    const TEXT_ENTRY *data;
    int scroll;
    int cursor;
    int selected;
    int flags;
} MENU;

extern void menu_Draw(const MENU *menu);
extern bool menu_Run(MENU *menu, const ALLEGRO_EVENT *event);

/*============================================================*/
#endif // _FRAME_H_
