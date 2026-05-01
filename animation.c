// 动画更新与最终效果展示 

#include <math.h>
#include <string.h>
#include "animation.h"

// 更新动画：根据时间进度移动方块、合并、更新分数，动画结束时将新棋盘生效并添加随机块
void UpdateAnimation(float dt) {
    if (!anim.active) return;
    anim.progress += dt / anim.duration;
    if (anim.progress >= 1.0f) {
        anim.progress = 1.0f;
        memcpy(num, anim.newsBoard, sizeof(num));
        add(num, boardSize);
        currentScore += anim.moveScore;
        if (currentScore > getCurrentBestScore()) setCurrentBestScore(currentScore);
        anim.active = 0;
        memset(anim.merged, 0, sizeof(anim.merged));
    }
}

// 更新胜利/失败状态：根据是否显示胜利弹窗分别判断
void update_win_state(void) {
	Board board;
    memcpy(&board, num, sizeof(num));
	if (showWinDialog) {
        gameWon = isGameWon(&board, boardSize);
        if (gameWon) gameOver = 1;
        else gameOver = isGameOver(&board, boardSize);
    } else {
        gameOver = isGameOver(&board, boardSize);
    }
}

// 绘制动画：分三层绘制（旧方块淡出、移动中的方块、合并特效）
void DrawAnimation(void) {
    if (!anim.active) return;
    float t = anim.progress;
    int halfTile = tileSize / 2;
    // 第一层：旧方块（逐渐透明）
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            int val = anim.oldBoard[i][j];
            if (val == 0) continue;
            int x = boardOffsetX + j * tileSize;
            int y = boardOffsetY + i * tileSize;
            Color bgColor = GetTileColor(val);
            bgColor.a = (unsigned char)((1.0f - t) * 255);
            if (random_color) DrawRectangleRounded((Rectangle){ x, y, drawSize, drawSize }, 0.1f, 0, tileColors[i][j]);
            else DrawRectangleRounded((Rectangle){ x, y, drawSize, drawSize }, 0.1f, 0, bgColor);
            if (random_color) randomizeColors(boardSize);
            Color textColor = DARKGRAY;
            textColor.a = (unsigned char)((1.0f - t) * 255);
            DrawNumberInTile(val, x, y, drawSize, drawSize, textColor);
        }
    }
    // 第二层：移动中的新方块（从源位置插值到目标位置）
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            int val = anim.newBoard[i][j];
            if (val == 0) continue;
            int srcR = anim.srcRow[i][j];
            int srcC = anim.srcCol[i][j];
            if (srcR < 0 || srcC < 0) continue;
            int srcX = boardOffsetX + srcC * tileSize;
            int srcY = boardOffsetY + srcR * tileSize;
            int dstX = boardOffsetX + j * tileSize;
            int dstY = boardOffsetY + i * tileSize;
            int curX = srcX + (int)((dstX - srcX) * t);
            int curY = srcY + (int)((dstY - srcY) * t);
            if (random_color) DrawRectangleRounded((Rectangle){ curX, curY, drawSize, drawSize }, 0.1f, 0, tileColors[i][j]);
            else DrawRectangleRounded((Rectangle){ curX, curY, drawSize, drawSize }, 0.1f, 0, GetTileColor(val));
            if (random_color) randomizeColors(boardSize);
            DrawNumberInTile(val, curX, curY, drawSize, drawSize, DARKGRAY);
        }
    }
    // 第三层：合并特效
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            if (anim.merged[i][j]) {
                int mergedValue = anim.newsBoard[i][j];
                DrawMergedEffect(i, j, mergedValue, t);
            }
        }
    }
}

// 绘制合并特效：根据当前特效类型（闪光、粒子、涟漪、缩放、光环）渲染动画效果
void DrawMergedEffect(int row, int col, int value, float progress) {
    int x = boardOffsetX + col * tileSize;
    int y = boardOffsetY + row * tileSize;
    int halfTile = tileSize / 2;
    int centerX = x + halfTile;
    int centerY = y + halfTile;
    switch (currentEffect) {
        case EFFECT_FLASH: {
            unsigned char alpha = (unsigned char)(255 * (1.0f - fabs(progress - 0.5f) * 2));
            DrawRectangleRounded((Rectangle){x, y, drawSize, drawSize}, 0.1f, 0, (Color){200, 220, 255, alpha});
            float scale = 1.0f - fabs(progress - 0.5f) * 1.2f;
            int fontSizeEffect = (int)(fontSize * scale);
            if (fontSizeEffect > 0) {
                char numText[16];
                sprintf(numText, "%d", value);
                int len = strlen(numText);
                if (len > 5 && fontSizeEffect > 20) fontSizeEffect = 20;
                if (len > 6 && fontSizeEffect > 16) fontSizeEffect = 16;
                int textWidth = MeasureText(numText, fontSizeEffect);
                int textX = x + (drawSize - textWidth) / 2;
                int textY = y + (drawSize - fontSize) / 2;
                DrawTextSafe(numText, textX, textY, fontSizeEffect, (Color){255, 215, 0, 255});
            }
            break;
        }
        case EFFECT_PARTICLE: {
            int numParticles = 12;
		    for (int p = 0; p < numParticles; p++) {
		        float angle = (p / (float)numParticles) * 2 * 3.14159f;
		        angle += (rand() % 100) / 100.0f * 0.5f;
		        float distance = progress * 120;
		        int px = centerX + (int)(cos(angle) * distance);
		        int py = centerY + (int)(sin(angle) * distance);
		        int radius = (int)(tileSize * 0.07f * (1.0f - progress));
		        if (radius < 2) radius = 2;
		        unsigned char alpha = (unsigned char)(255 * (1.0f - progress * 0.8f));
		        Color particleColor;
		        if (currentTheme == THEME_DARK) {
		            particleColor = (Color){255, 220, 100, alpha};
		        } else {
		            particleColor = (Color){200, 100, 50, alpha};
		        }
		        DrawCircle(px, py, radius, particleColor);
		    }
		    break;
        }
        case EFFECT_RIPPLE: {
            float maxRadius = tileSize * 0.6f;
		    float radius = progress * maxRadius;
		    unsigned char alpha = (unsigned char)(200 * (1.0f - progress));
		    unsigned char fillAlpha = (unsigned char)(80 * (1.0f - progress));
		    Color outerColor, middleColor;
		    if (currentTheme == THEME_DARK) {
		        outerColor = (Color){255, 255, 255, alpha};
		        middleColor = (Color){255, 200, 100, alpha};
		    } else {
		        switch (currentTheme) {
		            case THEME_CLASSIC:
		                outerColor = (Color){120, 110, 100, alpha};
		                middleColor = (Color){200, 120, 50, alpha};
		                break;
		            case THEME_FOREST:
		                outerColor = (Color){70, 100, 60, alpha};
		                middleColor = (Color){140, 180, 70, alpha};
		                break;
		            case THEME_WARM:
		                outerColor = (Color){130, 80, 50, alpha};
		                middleColor = (Color){220, 120, 40, alpha};
		                break;
		            default:
		                outerColor = (Color){100, 100, 100, alpha};
		                middleColor = (Color){180, 120, 60, alpha};
		                break;
		        }
		    }
		    Color fillColor = outerColor;
		    fillColor.a = fillAlpha;
		    DrawCircle(centerX, centerY, radius, fillColor);
		    DrawCircleLines(centerX, centerY, radius, outerColor);
		    DrawCircleLines(centerX, centerY, radius * 0.7f, middleColor);
		    DrawCircleLines(centerX, centerY, radius * 0.4f, (Color){255, 180, 50, alpha});
		    break;
        }
        case EFFECT_SCALE: {
            float scale;
		    if (progress < 0.5f) scale = 0.5f + progress * 2.0f;
		    else scale = 1.5f - (progress - 0.5f) * 1.0f;
		    if (scale < 0.3f) scale = 0.3f;
		    int fontSizeEffect = (int)(fontSize * scale);
		    if (fontSizeEffect > 0) {
		        char numText[16];
		        sprintf(numText, "%d", value);
		        int textWidth = MeasureText(numText, fontSizeEffect);
		        int textX = x + (drawSize - textWidth) / 2;
		        int textY = y + (drawSize - fontSizeEffect) / 2;
		        unsigned char alpha;
		        if (progress < 0.5f) alpha = (unsigned char)(255 * (progress / 0.5f));
		        else alpha = (unsigned char)(255 * (1.0f - (progress - 0.5f) * 1.2f));
		        if (alpha < 50) alpha = 50;
		        Color col = (Color){255, 215, 0, alpha};
		        DrawTextSafe(numText, textX, textY, fontSizeEffect, col);
		    }
            break;
        }
        case EFFECT_RINGS: {
            float maxRadius = tileSize * 0.48f;
			float radius = maxRadius * (1.0f - progress);
			unsigned char alpha = (unsigned char)(100 * (1.0f - progress));
			DrawCircle(centerX, centerY, radius, (Color){255, 150, 100, alpha});
			DrawCircle(centerX, centerY, radius * 0.6f, (Color){255, 210, 120, alpha});
            break;
        }
        default:
            break;
    }
}

// 主游戏循环：初始化窗口，处理菜单、游戏、存档确认等状态，更新和绘制每一帧
void show_everything(void) {
	SetTraceLogLevel(LOG_NONE);					// 禁止向黑窗口输出日志 
	init_game();
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "2048");
    // 设置显示图标 
    Image icon = LoadImage("test.ico");
    if (icon.data != NULL) {
    	SetWindowIcon(icon);
    	UnloadImage(icon);
	}
    init_font();
    InitBackground();
    // 设置基础帧率 
    SetTargetFPS(60);
    if (random_color) randomizeColors(boardSize);
    // 每帧循环一次 
    while (!WindowShouldClose()) {
    	// 更新动画和各种状态 
    	float dt = GetFrameTime();
    	UpdateBackground(dt);
    	UpdateAnimation(dt);
    	update_win_state();
    	// 开始绘制 
        BeginDrawing();
		DrawBackground();
		Color bgColor;
        if (gameState == STATE_MENU) {
	        DrawMenu();
	    } else if (gameState == STATE_PLAYING) {
	    	DrawStaticGameUI();
	    	if (anim.active) DrawAnimation();
			else draw_static_board();
			if (gameOver) DrawGameOverlay();
		} else if (gameState == STATE_LOAD_CONFIRM) {
		    DrawLoadConfirm();
		}
        EndDrawing();
        // 处理键盘输入 
        if (gameState == STATE_MENU) {
	        HandleMenuInput();
	    } else if (gameState == STATE_PLAYING) {
	    	if (IsKeyPressed(KEY_ESCAPE)) {
	    		saveGameProgress();
	            gameState = STATE_MENU;
	            anim.active = 0;
	        }
	        if (gameOver) HandleGameOverInput();
			else move_forward();
			HandleGameUIClicks();
	    } else if (gameState == STATE_LOAD_CONFIRM) {
	    	HandleLoadConfirmInput();
		}
		// 保存偏好和进度 
	    savePreferences();
	    saveGameProgress();
	}
    CloseWindow();
}

