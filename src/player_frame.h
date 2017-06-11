
#ifndef _PLAYER_FRAME_H_
#define _PLAYER_FRAME_H_

typedef enum {
    TEAM_MENU_STATE_MAIN,
} TEAM_MENU_STATE;

typedef struct {
    PLAYER *player;
    float scroll;
    TEAM_MENU_STATE state;
    
    // Selection
    int boxSelect;
    int teamSelect;
    int column;
} TEAM_MENU;

typedef enum {
    TEAM_MENU_CANCEL,
    TEAM_MENU_LEFT,
    TEAM_MENU_RIGHT,
    TEAM_MENU_UP,
    TEAM_MENU_DOWN,
    TEAM_MENU_NEUTRAL,
    TEAM_MENU_SELECT,
} TEAM_MENU_ACTION;

static void playerFrame_CreateTeam(TEAM_MENU *menu, PLAYER *player) {
    menu->player = player;
    menu->scroll = 0.0;
    menu->boxSelect = 0;
    menu->teamSelect = 0;
    menu->column = 0;
    menu->state = TEAM_MENU_STATE_MAIN;
}

extern void playerFrame_DrawTeam(const TEAM_MENU *menu);

extern void playerFrame_UpdateTeam(TEAM_MENU *menu, float dt);

extern void playerFrame_InteractTeam(TEAM_MENU *menu, TEAM_MENU_ACTION action);

#endif