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
    // Font elements
    const ALLEGRO_FONT *font;   ///< The font used to render text.
    
    // Colors
    ALLEGRO_COLOR foreground;   ///< The color of text and foreground elements.
    ALLEGRO_COLOR background;   ///< The background color used for frames.
    ALLEGRO_COLOR highlight;    ///< The highlight color for important information.
    ALLEGRO_COLOR disabled;     ///< Color for disabled text items.
    
    // Dimensions
    int outline;    ///< Size of the outline.
    int padding;    ///< Distance between the outline and the content.
    int header;     ///< Size of the header bar.
    int spacing;    ///< Distance between two lines of text.
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
    int x;          ///< The x-coordinate of the origin.
    int y;          ///< The y-coordinate of the origin.
    int width;      ///< The width of the area.
    int height;     ///< The height of the area.
    int flags;      ///< Rendering configuration for the frame.
} FRAME;

/// Flag to specify whether the frame should be outlined with
/// the foreground color.
#define FRAME_OUTLINE 1

/// Flag to specify whether the frame should be drawn with a
/// highlighted header box.
#define FRAME_HEADER 2

/**********************************************************//**
 * @brief Sets the frame's width and height with respect
 * to the outline and padding, so that the interior area of the
 * frame is large enough for the given dimensions.
 * @param frame: Pointer to a frame to modify.
 * @param width: Width of the internal area.
 * @param height: Height of the internal area.
 **************************************************************/
extern void frame_SetSize(FRAME *frame, int width, int height);

/**********************************************************//**
 * @brief Gets the coordinates where internal data should be
 * drawn in the frame. This is the upper left corner.
 * @param frame: The frame to inspect.
 * @param xo: Output parameter for the x coordinate.
 * @param yo: Output parameter for the y coordinate.
 **************************************************************/
extern void frame_GetStart(const FRAME *frame, int *xo, int *yo);

/**********************************************************//**
 * @brief Gets the coordinates where internal data should be
 * drawn in the frame. This is the lower right corner.
 * @param frame: The frame to inspect.
 * @param xf: Output parameter for the x coordinate.
 * @param yf: Output parameter for the y coordinate.
 **************************************************************/
extern void frame_GetEnd(const FRAME *frame, int *xf, int *yf);

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
    int x;              ///< The x position of the frame.
    int y;              ///< The y position of the frame.
    int maxWidth;       ///< The maximum width of any line of text.
    int lines;          ///< The number of lines of text.
    TEXT_ENTRY *data;   ///< The actual text to render.
    int flags;          ///< Rendering flags for the frame.
} TEXT_FRAME;

/// Using this flag will allow for dynamic computation of the
/// frame width based on the content of the textData array.
#define FRAME_DYNAMIC_WIDTH 4

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
    int x;              ///< The x position of the frame.
    int y;              ///< The y position of the frame.
    int maxWidth;       ///< The maximum width of any line of text.
    int lines;          ///< The lines to display.
    int maxLines;       ///< The actual number of lines.
    TEXT_ENTRY *data;   ///< All the data in the menu.
    int scroll;         ///< The index of the top entry displayed.
    int cursor;         ///< The placement of the cursor relative to scrolling.
    int flags;          ///< Rendering flags for the frame.
} MENU;

/// Defines whether the elements of the menu can loop around.
#define FRAME_LOOP 8

/// Defines whether the user can cancel the menu.
#define FRAME_CANCEL 16

// Menu running results. Results 0...n are valid indexes that
// should be considered.
#define MENU_CANCEL -2      ///< Cancel the menu (stop)
#define MENU_CONTINUE -1    ///< Continue running the menu.

/**********************************************************//**
 * @brief Draws the menu on the screen using the current theme.
 * @param menu: Pointer to the menu to draw.
 **************************************************************/
extern void menu_Draw(const MENU *menu);

/**********************************************************//**
 * @brief Handles keyboard input for the given menu.
 * @param menu: The menu being interacted with.
 * @param event: The keyboard event to handle.
 * @return The index of the selected entry on confirmation,
 * or MENU_CANCEL if the menu was cancelled, otherwise
 * MENU_CONTINUE if the menu should keep being interacted with.
 **************************************************************/
extern int menu_Run(MENU *menu, const ALLEGRO_EVENT *event);

/**********************************************************//**
 * @brief Initializes the cursor to the first menu entry.
 * @param menu: Pointer to the menu to reset.
 **************************************************************/
static inline void menu_Reset(MENU *menu) {
    menu->cursor = 0;
    menu->scroll = 0;
}

/*============================================================*/
#endif // _FRAME_H_
