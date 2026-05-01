#ifndef ANIMATION_H
#define ANIMATION_H

#include "ui.h"
#include "background.h"

void UpdateAnimation(float dt);
void update_win_state(void);
void DrawAnimation(void);
void DrawMergedEffect(int row, int col, int value, float progress);
void show_everything(void);

#endif

