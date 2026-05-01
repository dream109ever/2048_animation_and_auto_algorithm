#ifndef UI_H
#define UI_H

#include "globals.h"

void DrawMenu(void);
void HandleMenuInput(void);
void draw_static_board(void);
void DrawStaticGameUI(void);
void HandleGameUIClicks(void);
void ai_move(void);
void performMove(int direction, int is_animation);
void performAIMove(void);
void move_forward(void);
void DrawGameOverlay(void);
void HandleGameOverInput(void);
void DrawLoadConfirm(void);
void HandleLoadConfirmInput(void);

#endif
