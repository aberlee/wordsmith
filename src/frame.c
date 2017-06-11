/**********************************************************//**
 * @file frame.c
 * @author Wes64
 * @brief Implements the rendering and management of frames.
 **************************************************************/

// Standard library
#include <stdbool.h>    // bool
#include <stdlib.h>     // malloc, free

// Allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

// This project
#include "debug.h"      // eprintf, assert
#include "frame.h"      // FRAME

//**************************************************************
/// Rendering flags of the text.
#define TEXT_FLAGS (ALLEGRO_ALIGN_LEFT | ALLEGRO_ALIGN_INTEGER)

//**************************************************************
/// Shared theme data for all frames.
static THEME GlobalTheme;

/*============================================================*
 * Global theme manipulation
 *============================================================*/
void frame_SetTheme(const THEME *newTheme) {
    GlobalTheme = *newTheme;
}

const THEME *frame_GetTheme(void) {
    return &GlobalTheme;
}

/*============================================================*
 * Spacing data
 *============================================================*/
int frame_GetLineHeight(int lines) {
    int fontHeight = al_get_font_line_height(GlobalTheme.font);
    return (fontHeight + GlobalTheme.spacing)*lines - GlobalTheme.spacing;
}

int frame_GetLineSpacing(void) {
    return al_get_font_line_height(GlobalTheme.font) + GlobalTheme.spacing;
}

/*============================================================*
 * Drawing text on the screen
 *============================================================*/
void frame_DrawText(int x, int y, const char *text) {
    al_draw_text(GlobalTheme.font, GlobalTheme.foreground, x, y, TEXT_FLAGS, text);
}

void frame_DrawOutlinedText(int x, int y, const char *text) {
    // Construct the outline
    al_draw_text(GlobalTheme.font, GlobalTheme.foreground, x-1, y, TEXT_FLAGS, text);
    al_draw_text(GlobalTheme.font, GlobalTheme.foreground, x+1, y, TEXT_FLAGS, text);
    al_draw_text(GlobalTheme.font, GlobalTheme.foreground, x, y-1, TEXT_FLAGS, text);
    al_draw_text(GlobalTheme.font, GlobalTheme.foreground, x, y+1, TEXT_FLAGS, text);
    
    // Draw the actual text
    al_draw_text(GlobalTheme.font, GlobalTheme.background, x, y, TEXT_FLAGS, text);
}

/*============================================================*
 * Sizing frames
 *============================================================*/
void frame_SetSize(FRAME *frame, int width, int height) {
    // Default padding
    int delta = GlobalTheme.outline + 2*GlobalTheme.padding;
    frame->width = width + delta;
    frame->height = height + delta;

    // Header padding
    if (frame->flags & FRAME_HEADER) {
        frame->height += GlobalTheme.header;
    }
}

/*============================================================*
 * Getting the draw origin in a frame
 *============================================================*/
void frame_GetStart(const FRAME *frame, int *xo, int *yo) {
    // Frame's claimed origin
    *xo = frame->x + GlobalTheme.padding;
    *yo = frame->y + GlobalTheme.padding;
    
    // Outlining
    if (frame->flags & FRAME_OUTLINE) {
        *xo += GlobalTheme.outline / 2;
        *yo += GlobalTheme.outline / 2;
    }
    
    // Offset for the header
    if (frame->flags & FRAME_HEADER) {
        *yo += GlobalTheme.header;
    }
}

/*============================================================*
 * Getting the draw end in a frame
 *============================================================*/
void frame_GetEnd(const FRAME *frame, int *xf, int *yf) {
    // Frame's claimed origin
    *xf = frame->x + frame->width - GlobalTheme.padding;
    *yf = frame->y + frame->height - GlobalTheme.padding;
    
    // Outlining
    if (frame->flags & FRAME_OUTLINE) {
        // Need to do this weird thing to ensure the integer division
        // balances out with the start padding and outline.
        *xf -= GlobalTheme.outline - (GlobalTheme.outline / 2);
        *yf -= GlobalTheme.outline - (GlobalTheme.outline / 2);
    }
}

/*============================================================*
 * Drawing frames
 *============================================================*/
void frame_Draw(const FRAME *frame) {
    // Determine the actual frame bounding box
    int xo = frame->x;
    int yo = frame->y;
    int xf = xo + frame->width;
    int yf = yo + frame->height;
    
    // Draw the frame background
    al_draw_filled_rectangle(xo, yo, xf, yf, GlobalTheme.background);
    
    // Draw the frame outline
    if (frame->flags & FRAME_OUTLINE) {
        al_draw_rectangle(xo, yo, xf, yf, GlobalTheme.foreground, GlobalTheme.outline);
    }
    
    // Draw the frame header
    int hxo, hxf, hyo, hyf;
    if (frame->flags & FRAME_HEADER) {
        if (frame->flags & FRAME_OUTLINE) {
            hxo = xo + GlobalTheme.outline / 2;
            hxf = xf - (GlobalTheme.outline - GlobalTheme.outline / 2);
            hyo = yo + GlobalTheme.outline / 2;
            hyf = hyo + GlobalTheme.header;
        } else {
            hxo = xo;
            hxf = xf;
            hyo = yo;
            hyf = hyo + GlobalTheme.header;
        }
        al_draw_filled_rectangle(hxo, hyo, hxf, hyf, GlobalTheme.highlight);
    }
}

/*============================================================*
 * Text frame sizing
 *============================================================*/
int textFrame_Width(const TEXT_FRAME *frame) {
    // Dynamic width sizing
    if (frame->flags & FRAME_DYNAMIC_WIDTH) {
        // Get the maximum width of all the strings, including
        // the maximum frame width (set to 0 to exclude).
        int maxFontWidth = frame->maxWidth;
        for (int line = 0; line < frame->lines; line++) {
            int temp = al_get_text_width(GlobalTheme.font, frame->data[line].text);
            if (temp > maxFontWidth) {
                maxFontWidth = temp;
            }
        }
        return maxFontWidth;
    }
    
    // Static width sizing
    return frame->maxWidth;
}

int textFrame_Height(const TEXT_FRAME *frame) {
    int fontHeight = al_get_font_line_height(GlobalTheme.font);
    return (fontHeight + GlobalTheme.spacing)*frame->lines - GlobalTheme.spacing;
}

/*============================================================*
 * Drawing text frames
 *============================================================*/
void textFrame_Draw(const TEXT_FRAME *frame) {
    // Get the frame's area
    FRAME base;
    base.x = frame->x;
    base.y = frame->y;
    base.flags = frame->flags;
    
    // Get the base frame width
    int width = textFrame_Width(frame);
    
    // Get the base frame height
    int fontHeight = al_get_font_line_height(GlobalTheme.font);
    int height = textFrame_Height(frame);
    
    // Draw the base frame
    frame_SetSize(&base, width, height);
    frame_Draw(&base);
    
    // Determine the draw origin
    int xo, xf;
    int y, yf;
    frame_GetStart(&base, &xo, &y);
    frame_GetEnd(&base, &xf, &yf);
    
    // Determine other drawing parameters.
    int dy = fontHeight + GlobalTheme.spacing;
    
    // Draw all the text
    int textFlags = 0;
    ALLEGRO_COLOR color;
    for (int line = 0; line < frame->lines; line++) {
        // Apply the entry flags
        textFlags = frame->data[line].flags;
        
        // Get the text color
        if (textFlags & ENTRY_DISABLED) {
            color = GlobalTheme.disabled;
        } else if (textFlags & ENTRY_HIGHLIGHT) {
            color = GlobalTheme.highlight;
        } else {
            color = GlobalTheme.foreground;
        }
        
        // Draw the entry and selection bar
        // To center the text in the selection perfectly we need to add 1 to xo
        if (textFlags & ENTRY_SELECTED) {
            al_draw_filled_rectangle(xo, y, xf, y+fontHeight, color);
            al_draw_text(GlobalTheme.font, GlobalTheme.background, xo+1, y+1, TEXT_FLAGS, frame->data[line].text);
        } else {
            al_draw_text(GlobalTheme.font, color, xo+1, y+1, TEXT_FLAGS, frame->data[line].text);
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
 * Mapping MENU to TEXT_FRAME
 *============================================================*/
static inline void MenuToTextFrame(TEXT_FRAME *frame, const MENU *menu) {
    frame->x = menu->x;
    frame->y = menu->y;
    frame->maxWidth = menu->maxWidth;
    frame->lines = menu->lines;
    frame->flags = menu->flags;
    
    // Determine the subset of data to display.
    frame->data = &menu->data[menu->scroll];
}

/*============================================================*
 * Menu sizing
 *============================================================*/
int menu_Width(const MENU *menu) {
    TEXT_FRAME base;
    MenuToTextFrame(&base, menu);
    return textFrame_Width(&base);
}

int menu_Height(const MENU *menu) {
    TEXT_FRAME base;
    MenuToTextFrame(&base, menu);
    return textFrame_Height(&base);
}

/*============================================================*
 * Drawing menus
 *============================================================*/
void menu_Draw(const MENU *menu) {
    TEXT_FRAME base;
    MenuToTextFrame(&base, menu);
    textFrame_Draw(&base);
}

/*============================================================*
 * Menu execution
 *============================================================*/
MENU_STATUS menu_Run(MENU *menu, MENU_ACTION action) {
    // The currently selected item
    int current = menu->scroll + menu->cursor;
    int maxScroll = menu->maxLines - menu->lines + 1;
    int maxCursor = menu->lines - 1;
    
    // Received a keyboard event
    switch (action) {
    case MENU_ACTION_UP:
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
        return MENU_STATUS_CONTINUE;
    
    case MENU_ACTION_DOWN:
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
        return MENU_STATUS_CONTINUE;
        
    case MENU_ACTION_YES:
        // Index chosen!
        if (menu->data[current].flags & ENTRY_DISABLED) {
            // The item can't be chosen!
            return MENU_STATUS_CONTINUE;
        }
        return MENU_STATUS_CONFIRM;
        
    case MENU_ACTION_NO:
        // Menu cancelled.
        if (menu->flags & FRAME_CANCEL) {
            return MENU_STATUS_CANCEL;
        }
        return MENU_STATUS_CONTINUE;
    
    default:
        // Unregistered key handled.
        return MENU_STATUS_CONTINUE;
    }
}

/*============================================================*/
