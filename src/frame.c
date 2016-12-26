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

// Debugging libraries
#include "debug.h"      // eprintf, assert

// This project
#include "frame.h"      // FRAME

/*============================================================*
 * Constants
 *============================================================*/
#define OUTLINE_SIZE 1  // Width of an outline on the frame.
#define PADDING_SIZE 2  // Space between the frame border and the content.
#define HEADER_SIZE 4   // Height of the frame header.
#define LINE_SPACING 2  // Space between two lines of text.

/*============================================================*
 * Shared theme data
 *============================================================*/
static THEME theme;

void frame_SetTheme(const THEME *newTheme) {
    memcpy(&theme, newTheme, sizeof(theme));
}

const THEME *frame_GetTheme(void) {
    return &theme;
}

/*============================================================*
 * Drawing frames
 *============================================================*/
void frame_Draw(const FRAME *frame) {
    
    // Determine the origin point of the frame (upper left)
    int xo = frame->x;
    int yo = frame->y;
    
    // Determine the upper right
    int xf = xo + frame->width + PADDING_SIZE;
    if (frame->flags & FRAME_OUTLINE) {
        xf += OUTLINE_SIZE;
    }
    
    // Determine the lower left
    int yf = yo + frame->height + PADDING_SIZE;
    if (frame->flags & FRAME_OUTLINE) {
        yf += OUTLINE_SIZE;
    }
    if (frame->flags & FRAME_HEADER) {
        yf += HEADER_SIZE;
    }
    
    // Draw the frame background
    al_draw_filled_rectangle(xo, yo, xf, yf, theme.background);
    
    // Draw the frame outline
    if (frame->flags & FRAME_OUTLINE) {
        al_draw_rectangle(xo, yo, xf, yf, theme.foreground, OUTLINE_SIZE);
    }
    
    // Draw the frame header
    int hxf;
    if (frame->flags & FRAME_HEADER) {
        if (frame->flags & FRAME_OUTLINE) {
            hxf = xf - OUTLINE_SIZE;
        } else {
            hxf = xf;
        }
        al_draw_filled_rectangle(xo, yo, hxf, yo + HEADER_SIZE, theme.highlight);
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
    
    // Get the base frame width
    if (frame->maxWidth == TEXT_FRAME_DYNAMIC_WIDTH) {
        // Get the maximum width of all the strings
        int fontWidth=0, temp;
        for (int line = 0; line < frame->lines; line++) {
            if ((temp = al_get_text_width(theme.font, frame->data[line].text)) > fontWidth) {
                fontWidth = temp;
            }
        }
        base.width = fontWidth + PADDING_SIZE;
    } else {
        base.width = frame->maxWidth + PADDING_SIZE;
    }
    
    // Get the base frame height
    int fontHeight = al_get_font_line_height(theme.font);
    base.height = (fontHeight + LINE_SPACING)*frame->lines - LINE_SPACING + PADDING_SIZE;
    
    // Draw the base frame
    base.flags = frame->flags;
    frame_Draw(&base);
    
    // Determine the x range for the selection box
    int xo = base.x + PADDING_SIZE;
    int xf = base.x + base.width;
    
    // Determine the height of each item
    int y = base.y + PADDING_SIZE;
    if (frame->flags & FRAME_HEADER) {
        y += HEADER_SIZE;
    }
    int dy = fontHeight + LINE_SPACING;
    
    // Draw all the text
    int allegroFlags = ALLEGRO_ALIGN_LEFT | ALLEGRO_ALIGN_INTEGER;
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

