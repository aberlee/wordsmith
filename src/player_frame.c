
// Allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

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
        wordFrame_DrawHUD(&player->words[i], x, y, HUD_EXTENDED, menu->boxSelect == i && menu->column == 0);
        y += dy;
    }
	
	// Draw words in the current team
	float teamX = BORDER+WORD_HUD_WIDTH+BORDER;
	float teamY = BORDER;
	for (int i = 0; i < player->nTeam; i++) {
		wordFrame_DrawHUD(&player->words[player->team[i]], teamX, teamY, HUD_FULL, menu->teamSelect == player->team[i] && menu->column == 1);
        teamY += WORD_HUD_HEIGHT_FULL+3;
    }
    
}

#define ACCELERATION 1.0

void playerFrame_InteractTeam(TEAM_MENU *menu, TEAM_MENU_ACTION action) {
	const PLAYER *player = menu->player;
	
    switch (action) {
    case TEAM_MENU_UP:
        if (menu->column == 0 && menu->boxSelect > 0) {
            menu->boxSelect--;
        } else if (menu->column == 1 && menu->teamSelect > 0) {
            menu->teamSelect--;
        }
        break;
		
    case TEAM_MENU_DOWN:
		if (menu->column == 0 && menu->boxSelect < player->nWords-1) {
            menu->boxSelect++;
        } else if (menu->column == 1 && menu->teamSelect < player->nTeam-1) {
            menu->teamSelect++;
        }
        break;
    
    case TEAM_MENU_LEFT:
        menu->column = 0;
        break;
    
    case TEAM_MENU_RIGHT:
        menu->column = 1;
        break;
    
    case TEAM_MENU_NEUTRAL:
        break;
    
    default:
        break;
    }
}

void playerFrame_UpdateTeam(TEAM_MENU *menu, float dt) {
    const PLAYER *player = menu->player;
    
    // Total menu items height
    float dy = WORD_HUD_HEIGHT_EXTENDED + PADDING;
	float span = player->nWords*dy;
	float spanMax = WINDOW_HEIGHT - BORDER;
	if (span > spanMax) {
		span = spanMax;
	}
	
	// Minimum scroll
    float scrollMin = 0;
	
	// Maximum scroll
    float scrollMax = player->nWords*dy - span;
    if (scrollMax < 0) {
        scrollMax = 0;
    }
    
    switch (menu->state) {
    case TEAM_MENU_STATE_MAIN: {
		// Get the target scroll
		float targetScroll = menu->boxSelect*dy;
		if (targetScroll > scrollMax) {
			targetScroll = scrollMax;
		} else if (targetScroll < scrollMin) {
			targetScroll = scrollMin;
		}
		
		// Velocity computation
		float velocity = targetScroll - menu->scroll;
		if (velocity > 8.0) {
			velocity = 8.0;
		} else if (velocity < -8.0) {
			velocity = -8.0;
		}
		
        // Menu physics update
        menu->scroll += velocity;
        if (menu->scroll > scrollMax) {
            menu->scroll = scrollMax;
        } else if (menu->scroll < scrollMin) {
            menu->scroll = scrollMin;
        }
	}
    default:
        break;
    }
}