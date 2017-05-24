
#include "window.h"
#include "word.h"
#include "player.h"
#include "word_frame.h"
#include "player_frame.h"

#define PADDING 8

#define BORDER 10

void playerFrame_DrawTeam(const TEAM_MENU *menu) {
    const PLAYER *player = menu->player;
    
    // Draw the player data
    float dy = WORD_HUD_HEIGHT_EXTENDED + PADDING;
    
    // Get the scroll endpoints
    int start = (int)((menu->scroll - BORDER) / dy);
    if (start < 0) {
        start = 0;
    }
    int end = (int)((menu->scroll + WINDOW_HEIGHT + dy) / dy);
    if (end > player->nWords) {
        end = player->nWords;
    }
    
    // Draw the word HUDs for everything in the box
    float x = BORDER;
    float y = BORDER + start*dy - menu->scroll;
    for (int i = start; i < end; i++) {
        wordFrame_DrawHUD(&player->words[i], x, y, HUD_EXTENDED);
        y += dy;
    }
    
}

#define ACCELERATION 1.0

void playerFrame_InteractTeam(TEAM_MENU *menu, TEAM_MENU_ACTION action) {
    switch (action) {
    case TEAM_MENU_UP:
        menu->acceleration = -ACCELERATION;
        break;
    case TEAM_MENU_DOWN:
        menu->acceleration = ACCELERATION;
        break;
    
    case TEAM_MENU_NEUTRAL:
        menu->acceleration = 0.0;
        break;
    
    default:
        break;
    }
}

void playerFrame_UpdateTeam(TEAM_MENU *menu, float dt) {
    const PLAYER *player = menu->player;
    
    // Maximum scroll
    float dy = WORD_HUD_HEIGHT_EXTENDED + PADDING;
    float scrollMin = dy - WINDOW_HEIGHT + BORDER;
    float scrollMax = (player->nWords - 1) * dy;
    if (scrollMax < 0) {
        scrollMax = 0;
    }
    
    switch (menu->state) {
    case TEAM_MENU_STATE_MAIN:
        // Menu physics update
        menu->velocity += menu->acceleration;
        menu->velocity -= 4*dt*menu->velocity;
        menu->scroll += menu->velocity;
        
        // Menu scroll endpoints
        if (menu->scroll > scrollMax) {
            menu->scroll = scrollMax;
            menu->velocity = 0.0;
        } else if (menu->scroll < scrollMin) {
            menu->scroll = scrollMin;
            menu->velocity = 0.0;
        }
    default:
        break;
    }
}