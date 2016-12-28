/**********************************************************//**
 * @file frame.c
 * @author Wes64
 * @brief Implements the rendering and management of frames.
 **************************************************************/

// Standard library
#include <stdlib.h>     // malloc, srand
#include <stdbool.h>    // bool

// Allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

// This project
#include "debug.h"      // eprintf, assert
#include "frame.h"      // FRAME
#include "keyboard.h"   // KEY_UP, KEY_DOWN ...

/*============================================================*
 * Shared theme data
 *============================================================*/
static THEME theme;

// Public theme setter
void frame_SetTheme(const THEME *newTheme) {
    memcpy(&theme, newTheme, sizeof(theme));
}

// Public theme getter
const THEME *frame_GetTheme(void) {
    return &theme;
}

/*============================================================*
 * Sizing frames
 *============================================================*/
void frame_SetSize(FRAME *frame, int width, int height) {
    
    // Default
    int delta = theme.outline + 2*theme.padding;
    frame->width = width + delta;
    frame->height = height + delta;

    // Header
    if (frame->flags & FRAME_HEADER) {
        frame->height += theme.header;
    }
}

/*============================================================*
 * Getting the draw origin in a frame
 *============================================================*/
void frame_GetStart(const FRAME *frame, int *xo, int *yo) {
    
    // Frame's claimed origin
    *xo = frame->x + theme.padding;
    *yo = frame->y + theme.padding;
    
    // Outlining
    if (frame->flags & FRAME_OUTLINE) {
        *xo += theme.outline / 2;
        *yo += theme.outline / 2;
    }
    
    // Offset for the header
    if (frame->flags & FRAME_HEADER) {
        *yo += theme.header;
    }
}

/*============================================================*
 * Getting the draw end in a frame
 *============================================================*/
void frame_GetEnd(const FRAME *frame, int *xf, int *yf) {
    
    // Frame's claimed origin
    *xf = frame->x + frame->width - theme.padding;
    *yf = frame->y + frame->height - theme.padding;
    
    // Outlining
    if (frame->flags & FRAME_OUTLINE) {
        // Need to do this weird thing to ensure the integer division
        // balances out with the start padding and outline.
        *xf -= theme.outline - (theme.outline / 2);
        *yf -= theme.outline - (theme.outline / 2);
    }
}

/*============================================================*
 * Drawing frames
 *============================================================*/
void frame_Draw(const FRAME *frame) {
    
    // Determine the origin point of the frame (upper left)
    int xo = frame->x;
    int yo = frame->y;
    
    // Determine the upper right
    int xf = xo + frame->width;
    
    // Determine the lower left
    int yf = yo + frame->height;
    
    // Draw the frame background
    al_draw_filled_rectangle(xo, yo, xf, yf, theme.background);
    
    // Draw the frame outline
    if (frame->flags & FRAME_OUTLINE) {
        al_draw_rectangle(xo, yo, xf, yf, theme.foreground, theme.outline);
    }
    
    // Draw the frame header
    int hxo, hxf, hyo, hyf;
    if (frame->flags & FRAME_HEADER) {
        if (frame->flags & FRAME_OUTLINE) {
            hxo = xo + theme.outline / 2;
            hxf = xf - theme.outline / 2;
            hyo = yo + theme.outline / 2;
            hyf = hyo + theme.header;
        } else {
            hxo = xo;
            hxf = xf;
            hyo = yo;
            hyf = hyo + theme.header;
        }
        al_draw_filled_rectangle(hxo, hyo, hxf, hyf, theme.highlight);
    }
}

/*============================================================*
 * Drawing text frames
 *============================================================*/
void textframe_Draw(const TEXT_FRAME *frame) {
    
    // Get the frame's area
    FRAME base;
    base.x = frame->x;
    base.y = frame->y;
    base.flags = frame->flags;
    
    // Get the base frame width
    int width;
    if (frame->flags & FRAME_DYNAMIC_WIDTH) {
        // Get the maximum width of all the strings
        int fontWidth=0, temp;
        for (int line = 0; line < frame->lines; line++) {
            if ((temp = al_get_text_width(theme.font, frame->data[line].text)) > fontWidth) {
                fontWidth = temp;
            }
        }
        width = fontWidth;
    } else {
        width = frame->maxWidth;
    }
    
    // Get the base frame height
    int fontHeight = al_get_font_line_height(theme.font);
    int height = (fontHeight + theme.spacing)*frame->lines - theme.spacing;
    
    // Draw the base frame
    frame_SetSize(&base, width, height);
    frame_Draw(&base);
    
    // Determine the draw origin
    int xo, xf;
    int y, yf;
    frame_GetStart(&base, &xo, &y);
    frame_GetEnd(&base, &xf, &yf);
    
    // Determine other drawing parameters.
    int dy = fontHeight + theme.spacing;
    int allegroFlags = ALLEGRO_ALIGN_LEFT | ALLEGRO_ALIGN_INTEGER;
    
    // Draw all the text
    int textFlags = 0;
    ALLEGRO_COLOR color;
    for (int line = 0; line < frame->lines; line++) {
        // Apply the entry flags
        textFlags = frame->data[line].flags;
        
        // Get the text color
        if (textFlags & ENTRY_DISABLED) {
            color = theme.disabled;
        } else if (textFlags & ENTRY_HIGHLIGHT) {
            color = theme.highlight;
        } else {
            color = theme.foreground;
        }
        
        // Draw the entry and selection bar
        // To center the text in the selection perfectly we need to add 1 to xo
        if (textFlags & ENTRY_SELECTED) {
            al_draw_filled_rectangle(xo, y, xf, y+fontHeight, color);
            al_draw_text(theme.font, theme.background, xo+1, y, allegroFlags, frame->data[line].text);
        } else {
            al_draw_text(theme.font, color, xo+1, y, allegroFlags, frame->data[line].text);
        }
        y += dy;
    }
}

/*============================================================*
 * Menu reset
 *============================================================*/
void menu_Reset(MENU *menu) {
    menu->cursor = 0;
    menu->scroll = 0;
    
    // Zero out all the selections
    for (int i = 0; i < menu->maxLines; i++) {
        menu->data[i].flags &= ~ENTRY_SELECTED;
    }
    
    // Select the current item only
    menu->data[0].flags |= ENTRY_SELECTED;
}

/*============================================================*
 * Drawing menus
 *============================================================*/
void menu_Draw(const MENU *menu) {
    
    // Determine the text frame to draw
    TEXT_FRAME base;
    base.x = menu->x;
    base.y = menu->y;
    base.maxWidth = menu->maxWidth;
    base.lines = menu->lines;
    base.flags = menu->flags;
    
    // Determine the subset of data to display.
    base.data = &menu->data[menu->scroll];

    // Draw the frame
    textframe_Draw(&base);
}

/*============================================================*
 * Menu execution
 *============================================================*/
int menu_Run(MENU *menu, const ALLEGRO_EVENT *event) {
    
    // The currently selected item
    int current = menu->scroll + menu->cursor;
    int maxScroll = menu->maxLines - menu->lines + 1;
    int maxCursor = menu->lines - 1;
    
    // Key press handler
    switch (event->type) {
    case ALLEGRO_EVENT_KEY_DOWN:
    case ALLEGRO_EVENT_KEY_CHAR:
        // Received a keyboard event
        switch (event->keyboard.keycode) {
        case KEY_UP:
            if (menu->cursor > 0) {
                // Move the cursor up
                menu->cursor--;
                menu->data[current].flags &= ~ENTRY_SELECTED;
                menu->data[current-1].flags |= ENTRY_SELECTED;
                
            } else if (menu->scroll > 0) {
                // Cursor already at the top, scroll up
                menu->scroll--;
            
            } else if (menu->flags & FRAME_LOOP) {
                // Loop around to the end of the menu
                menu->cursor = maxCursor;
                menu->scroll = maxScroll;
            }
            return MENU_CONTINUE;
        
        case KEY_DOWN:
            if (menu->cursor < maxCursor) {
                // Move the cursor down
                menu->cursor++;
                menu->data[current].flags &= ~ENTRY_SELECTED;
                menu->data[current+1].flags |= ENTRY_SELECTED;
                
            } else if (menu->scroll < maxScroll) {
                // Cursor already at bottom, scroll down
                menu->scroll++;
            
            } else if (menu->flags & FRAME_LOOP) {
                // Loop around to the top
                menu->cursor = 0;
                menu->scroll = 0;
            }
            return MENU_CONTINUE;
            
        case KEY_A:
            // Index chosen!
            if (menu->data[current].flags & ENTRY_DISABLED) {
                // The item can't be chosen!
                return MENU_CONTINUE;
            }
            return current;
            
        case KEY_B:
            // Menu cancelled.
            if (menu->flags & FRAME_CANCEL) {
                return MENU_CANCEL;
            }
            return MENU_CONTINUE;
        
        default:
            // Unregistered key handled.
            return MENU_CONTINUE;
        }
    
    default:
        // Unregistered event handled.
        return MENU_CONTINUE;
    }
}

/*============================================================*/
