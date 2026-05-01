// 主要静态ui绘制以及键盘输入处理 

#include "ui.h"

// 绘制主菜单界面，包括标题、大小选择按钮、退出按钮和版本信息
void DrawMenu(void) {
    int textWidth = MeasureText("2048", 100);
    DrawTextSafe("2048", SCREEN_WIDTH / 2 - textWidth / 2, 150, 120, DARKGRAY);
    int btnWidth = 200;
    int btnHeight = 65;
    int startY = 320;
    int spacing = 35;
    int startX = (SCREEN_WIDTH - btnWidth) / 2;
    // 绘制每种棋盘大小的按钮
    for (int i = 0; i < SIZE_COUNT; i++) {
        int y = startY + i * (btnHeight + spacing);
        Rectangle btn = { startX, y, btnWidth, btnHeight };
        Color btnColor = (boardSize == SUPPORTED_SIZES[i]) ? DARKGRAY : LIGHTGRAY;
        DrawRectangleRec(btn, btnColor);
        DrawRectangleLinesEx(btn, 2, DARKGRAY);
        char text[16];
        sprintf(text, "%d x %d", SUPPORTED_SIZES[i], SUPPORTED_SIZES[i]);
        int textW = MeasureText(text, 30);
        DrawTextSafe(text, startX + (btnWidth - textW) / 2, y + (btnHeight - 30) / 2, 30, BLACK);
    }
    // 退出按钮
    Rectangle exitBtn = { SCREEN_WIDTH - 120, SCREEN_HEIGHT - 80, 100, 50 };
    DrawRectangleRec(exitBtn, LIGHTGRAY);
    DrawRectangleLinesEx(exitBtn, 2, DARKGRAY);
    DrawTextSafe("Exit", exitBtn.x + 20, exitBtn.y + 12, 30, BLACK);
    // 版本和作者信息
    int fontSizeInfo = 16;
	int margin = 12;
	const char* line1 = "version: 1.1.0";
	const char* line2 = "by: dream109ever";
	int line1Width = MeasureText(line1, fontSizeInfo);
	int line2Width = MeasureText(line2, fontSizeInfo);
	int infoX = margin;
	int line1Y = SCREEN_HEIGHT - fontSizeInfo * 2 - margin;
	int line2Y = SCREEN_HEIGHT - fontSizeInfo - margin;
	Color verColor = (currentTheme == THEME_DARK) ? BLACK : DARKGRAY;
	DrawTextSafe(line1, infoX, line1Y, fontSizeInfo, verColor);
	DrawTextSafe(line2, infoX, line2Y, fontSizeInfo, verColor);
}

// 处理主菜单的鼠标点击事件：选择棋盘大小、加载存档或退出
void HandleMenuInput(void) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();
        int btnWidth = 200;
        int btnHeight = 65;
        int startY = 320;
        int spacing = 35;
        int startX = (SCREEN_WIDTH - btnWidth) / 2;
        // 检查点击了哪个大小按钮
        for (int i = 0; i < SIZE_COUNT; i++) {
            int y = startY + i * (btnHeight + spacing);
            Rectangle btn = { startX, y, btnWidth, btnHeight };
            if (CheckCollisionPointRec(mouse, btn)) {
                int newSizeIdx = i;
                int newSize = SUPPORTED_SIZES[newSizeIdx];
                GameProgress prog;
                // 如果有存档则询问是否继续，否则直接开始新游戏
                if (loadGameProgressForSize(i, &prog) && prog.boardSize == newSize) {
                    pendingBoardSizeIdx = i;
                    boardSize = prog.boardSize;
                    memcpy(num, prog.board, sizeof(num));
                    currentScore = prog.currentScore;
                    gameOver = prog.gameOver;
                    gameWon = prog.gameWon;
                    showWinDialog = prog.showWinDialog;
                    prefs.currentBoardSizeIdx = i;
                    currentTheme = prefs.themeForSize;
                    currentEffect = prefs.effectForSize;
                    CalculateBoardLayout();
                    gameState = STATE_LOAD_CONFIRM;
                    confirmDelayFrames = 2;
                } else {
                    prefs.currentBoardSizeIdx = i;
                    boardSize = newSize;
                    currentTheme = prefs.themeForSize;
                    currentEffect = prefs.effectForSize;
                    CalculateBoardLayout();
                    resetGame(num);
                    gameState = STATE_PLAYING;
                    savePreferences();
                }
                if (random_color) randomizeColors(boardSize);
                break;
            }
        }
        // 退出按钮
        Rectangle exitBtn = { SCREEN_WIDTH - 120, SCREEN_HEIGHT - 80, 100, 50 };
        if (CheckCollisionPointRec(mouse, exitBtn)) {
            CloseWindow();
            exit(0);
        }
    }
}

// 绘制静态棋盘（所有格子颜色和数字）
void draw_static_board(void) {
	for (int i = 0; i < boardSize; i++) {
		for (int j = 0; j < boardSize; j++) {
			int x = boardOffsetX + j * tileSize;
			int y = boardOffsetY + i * tileSize;
			if (random_color) DrawRectangleRounded((Rectangle){ x, y, drawSize, drawSize }, 0.1f, 0, tileColors[i][j]);
			else DrawRectangleRounded((Rectangle){ x, y, drawSize, drawSize }, 0.1f, 0, GetTileColor(num[i][j]));
			if (num[i][j] != 0) DrawNumberInTile(num[i][j], x, y, drawSize, drawSize, DARKGRAY);
		}
	}
}

// 绘制游戏界面所有静态ui元素（背景、分数、主题/特效/AI按钮等）
void DrawStaticGameUI(void) {
	// 棋盘背景色（根据主题）
	Color bgColor;
    if (currentTheme == THEME_CLASSIC) bgColor = (Color){0, 0, 255, 32};
    else if (currentTheme == THEME_DARK) bgColor = (Color){20, 20, 30, 200};
    else if (currentTheme == THEME_FOREST) bgColor = (Color){60, 80, 40, 100};
    else if (currentTheme == THEME_WARM) bgColor = (Color){100, 60, 30, 80};
    else bgColor = (Color){0, 0, 255, 32};
    if (random_color) DrawRectangleRounded((Rectangle){ boardBgX, boardBgY, boardBgW, boardBgH }, 0.05f, 0, boardBgColor);
    else DrawRectangleRounded((Rectangle){ boardBgX, boardBgY, boardBgW, boardBgH }, 0.05f, 0, bgColor);
    // 空白格子的背景色（根据主题）
    Color noColor;
    if (currentTheme == THEME_CLASSIC) noColor = (Color){205, 193, 180, 255};
    else if (currentTheme == THEME_DARK) noColor = (Color){40, 44, 52, 255};
    else if (currentTheme == THEME_FOREST) noColor = (Color){200, 220, 180, 255};
    else if (currentTheme == THEME_WARM) noColor = (Color){255, 230, 210, 255};
    else noColor = (Color){205, 193, 180, 255};
    // 绘制每个格子（先画背景，数字由 draw_static_board 负责）
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            int x = boardOffsetX + j * tileSize;
            int y = boardOffsetY + i * tileSize;
            if (random_color) DrawRectangleRounded((Rectangle){ x, y, drawSize, drawSize }, 0.1f, 0, tileColors[i][j]);
            else DrawRectangleRounded((Rectangle){ x, y, drawSize, drawSize }, 0.1f, 0, noColor);
        }
    }
    // 当前分数和最高分显示（尽管当前有小小瑕疵） 
    Color scoreBgColor = bgColor;
    char scoreText[50];
    sprintf(scoreText, "Current Score: %d", currentScore);
    int fontSizeScore = 39;
    int padding = 15;
    int currentScoreWidth = MeasureText(scoreText, fontSizeScore);
    Rectangle currentScoreRect = {80 - padding,
        40 - padding / 2,
        currentScoreWidth + padding * 2,
        fontSizeScore + padding
    };
    DrawRectangleRounded(currentScoreRect, 0.2f, 0, scoreBgColor);
    DrawTextSafe(scoreText, 80, 40, fontSizeScore, DARKGRAY);
    sprintf(scoreText, "Best Score: %d", getCurrentBestScore());
    int bestScoreWidth = MeasureText(scoreText, fontSizeScore);
    int bestScoreX = 150 + MeasureText("Current Score: ", 40) + 40;
    Rectangle bestScoreRect = {
        bestScoreX - padding,
        40 - padding / 2,
        bestScoreWidth + padding * 2,
        fontSizeScore + padding
    };
    DrawRectangleRounded(bestScoreRect, 0.2f, 0, scoreBgColor);
    DrawTextSafe(scoreText, bestScoreX, 40, fontSizeScore, DARKGRAY);
    // 主题选择按钮
    for (int i = 0; i < THEME_COUNT; i++) {
        Rectangle btnRect = { btnX, btnY + i * (btnH + btnSpacing), btnW, btnH };
        Color btnColor = (currentTheme == i) ? DARKGRAY : LIGHTGRAY;
        DrawRectangleRec(btnRect, btnColor);
        DrawRectangleLinesEx(btnRect, 2, DARKGRAY);
        int textW = MeasureText(themeNames[i], 20);
        DrawTextSafe(themeNames[i], btnX + (btnW - textW)/2, btnY + i * (btnH + btnSpacing) + (btnH - 20)/2, 20, BLACK);
    }
    // 特效选择按钮
    for (int i = 0; i < EFFECT_COUNT; i++) {
        Rectangle btnRect = { effectBtnX, effectBtnY + i * (effectBtnH + effectBtnSpacing), effectBtnW, effectBtnH };
        Color btnColor = (currentEffect == i) ? DARKGRAY : LIGHTGRAY;
        DrawRectangleRec(btnRect, btnColor);
        DrawRectangleLinesEx(btnRect, 2, DARKGRAY);
        int textW = MeasureText(effectNames[i], 20);
        DrawTextSafe(effectNames[i], effectBtnX + (effectBtnW - textW)/2, effectBtnY + i * (effectBtnH + effectBtnSpacing) + (effectBtnH - 20)/2, 20, BLACK);
    }
    // AI模式按钮
    for (int i = 0; i < AI_MODE_COUNT; i++) {
	    Rectangle btnRect = { aiBtnX, btnY + i * (aiBtnH + aiBtnSpacing), aiBtnW, aiBtnH };
	    Color btnColor = (currentAIMode == i) ? DARKGRAY : LIGHTGRAY;
	    DrawRectangleRec(btnRect, btnColor);
	    DrawRectangleLinesEx(btnRect, 2, DARKGRAY);
	    char text[20];
	    sprintf(text, "Auto: %s", aiModeNames[i]);
	    int textW = MeasureText(text, 18);
	    DrawTextSafe(text, btnRect.x + (aiBtnW - textW)/2 - 10, btnRect.y + (aiBtnH - 18)/2, 20, BLACK);
	}
	// 随机颜色开关按钮
	Rectangle randomColorBtn = { randomColorBtnX, randomColorBtnY, randomColorBtnW, randomColorBtnH };
	Color btnColor = random_color ? DARKGRAY : LIGHTGRAY;
	DrawRectangleRec(randomColorBtn, btnColor);
	DrawRectangleLinesEx(randomColorBtn, 2, DARKGRAY);
	const char* line1 = "Random Color:";
	const char* line2 = random_color ? "ON" : "OFF";
	int fontSize = 20;
	int lineSpacing = 4;
	int line1W = MeasureText(line1, fontSize);
	int line2W = MeasureText(line2, fontSize);
	int totalTextHeight = fontSize * 2 + lineSpacing;
	int startY = randomColorBtnY + (randomColorBtnH - totalTextHeight) / 2;
	DrawTextSafe(line1, randomColorBtnX + (randomColorBtnW - line1W) / 2, startY, fontSize, BLACK);
	DrawTextSafe(line2, randomColorBtnX + (randomColorBtnW - line2W) / 2, startY + fontSize + lineSpacing, fontSize, BLACK);
	// 重置按钮
    int resetBtnX = SCREEN_WIDTH - BACK_BTN_WIDTH - BACK_BTN_MARGIN - RESET_BTN_WIDTH - 15;
    int resetBtnY = SCREEN_HEIGHT - RESET_BTN_HEIGHT - BACK_BTN_MARGIN;
    DrawRectangleRec((Rectangle){ resetBtnX, resetBtnY, RESET_BTN_WIDTH, RESET_BTN_HEIGHT }, LIGHTGRAY);
    DrawRectangleLinesEx((Rectangle){ resetBtnX, resetBtnY, RESET_BTN_WIDTH, RESET_BTN_HEIGHT }, 2, DARKGRAY);
    const char* resetText = "Reset";
    int resetTextW = MeasureText(resetText, 20);
    DrawTextSafe(resetText, resetBtnX + (RESET_BTN_WIDTH - resetTextW)/2, resetBtnY + (RESET_BTN_HEIGHT - 20)/2, 20, BLACK);
    // 返回菜单按钮
    int btnX_menu = SCREEN_WIDTH - BACK_BTN_WIDTH - BACK_BTN_MARGIN;
    int btnY_menu = SCREEN_HEIGHT - BACK_BTN_HEIGHT - BACK_BTN_MARGIN;
    DrawRectangleRec((Rectangle){ btnX_menu, btnY_menu, BACK_BTN_WIDTH, BACK_BTN_HEIGHT }, LIGHTGRAY);
    DrawRectangleLinesEx((Rectangle){ btnX_menu, btnY_menu, BACK_BTN_WIDTH, BACK_BTN_HEIGHT }, 2, DARKGRAY);
    const char* backText = "Menu";
    int textW = MeasureText(backText, 20);
    DrawTextSafe(backText, btnX_menu + (BACK_BTN_WIDTH - textW)/2, btnY_menu + (BACK_BTN_HEIGHT - 20)/2, 20, BLACK);
}

// 处理游戏界面内鼠标点击（主题、特效、重置、返回等）
void HandleGameUIClicks(void) {
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 mousePos = GetMousePosition();
		// 主题按钮
		for (int i = 0; i < THEME_COUNT; i++) {
	        Rectangle btnRect = { btnX, btnY + i * (btnH + btnSpacing), btnW, btnH };
	        if (CheckCollisionPointRec(mousePos, btnRect)) {
	            if (currentTheme != i) {
	                currentTheme = i;
	                prefs.themeForSize = i;
	                savePreferences();
	            }
	            return;
	        }
	    }
	    // 特效按钮
	    for (int i = 0; i < EFFECT_COUNT; i++) {
	        Rectangle btnRect = { effectBtnX, effectBtnY + i * (effectBtnH + effectBtnSpacing), effectBtnW, effectBtnH };
	        if (CheckCollisionPointRec(mousePos, btnRect)) {
	            if (currentEffect != i) {
	                currentEffect = i;
	                prefs.effectForSize = i;
	                savePreferences();
	            }
	            return;
	        }
	    }
	    // 重置按钮
	    int resetBtnX = SCREEN_WIDTH - BACK_BTN_WIDTH - BACK_BTN_MARGIN - RESET_BTN_WIDTH - 15;
	    int resetBtnY = SCREEN_HEIGHT - RESET_BTN_HEIGHT - BACK_BTN_MARGIN;
	    Rectangle resetBtn = { resetBtnX, resetBtnY, RESET_BTN_WIDTH, RESET_BTN_HEIGHT };
	    if (CheckCollisionPointRec(mousePos, resetBtn)) {
	        resetGame(num);
	        anim.active = 0;
			gameOverTimer = 0.0f;
	        saveGameProgress();
	        return;
	    }
	    // 菜单按钮
	    int backBtnX = SCREEN_WIDTH - BACK_BTN_WIDTH - BACK_BTN_MARGIN;
	    int backBtnY = SCREEN_HEIGHT - BACK_BTN_HEIGHT - BACK_BTN_MARGIN;
	    Rectangle backBtn = { backBtnX, backBtnY, BACK_BTN_WIDTH, BACK_BTN_HEIGHT };
	    if (CheckCollisionPointRec(mousePos, backBtn)) {
	        saveGameProgress();
	        gameState = STATE_MENU;
	        anim.active = 0;
	        currentAIMode = AI_MODE_OFF;
	        return;
	    }
	    // AI模式按钮
	    for (int i = 0; i < AI_MODE_COUNT; i++) {
		    Rectangle btnRect = { aiBtnX, btnY + i * (aiBtnH + aiBtnSpacing), aiBtnW, aiBtnH };
		    if (CheckCollisionPointRec(mousePos, btnRect)) {
		        currentAIMode = i;
		        break;
		    }
		}
		// 随机颜色开关
		Rectangle randomColorBtn = { randomColorBtnX, randomColorBtnY, randomColorBtnW, randomColorBtnH };
		if (CheckCollisionPointRec(mousePos, randomColorBtn)) {
		    random_color = !random_color;
		    if (random_color) randomizeColors(boardSize);
		    draw_static_board();
		}
	}
}

// AI自动移动一步（基于当前棋盘状态选择最优动作）
// 这是一个困扰本人许久的bug，现象为：
// 1、如果移动合并动画结合这个自动运行，则在自动运行几步后卡死；
// 2、如果仅将注释部分去掉，即先正常运算得到正确方向，再用随机值代替，则程序运行顺畅，但随机移动无法得到高分；
// 3、如果不展示移动合并动画，而是展示瞬间合成，则4*4棋盘测试最大速度为每秒300步，8*8棋盘前期最大速度可达每秒1000步，后期稳定在每秒200步左右；
// 恳请看到这里的大佬出手[拜谢] 
void ai_move(void) {
	if (!anim.active) {
		Board board;
		board.score = 0;
		memcpy(&board.board, num, sizeof(num));
		int action = get_action(&board, boardSize);
//		int action = rand() % 4;
		move(&board, action, 1, boardSize);
		add_random(&board, boardSize);
		currentScore += board.score;
		if (currentScore > getCurrentBestScore()) setCurrentBestScore(currentScore);
		memcpy(num, &board.board, sizeof(num));
	}
}

// 执行一次移动（方向），可选择是否播放动画
void performMove(int direction, int is_animation) {
    if (anim.active) return;
    int temp[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    int srcR[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    int srcC[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    int temps[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    int mergedTemp[MAX_BOARD_SIZE][MAX_BOARD_SIZE] = {0};
    int totalScore = 0;
    memcpy(temp, num, sizeof(temp));
    for (int i = 0; i < boardSize; i++) for (int j = 0; j < boardSize; j++) srcR[i][j] = srcC[i][j] = -1;
    if (direction == 5 || direction == 2) {
        for (int j = 0; j < boardSize; j++) {
            int col[MAX_BOARD_SIZE], newCol[MAX_BOARD_SIZE], srcRowForCol[MAX_BOARD_SIZE], newsCol[MAX_BOARD_SIZE];
            int merged[MAX_BOARD_SIZE] = {0};
            for (int i = 0; i < boardSize; i++) col[i] = num[i][j];
            totalScore += processLine(col, newCol, srcRowForCol, newsCol, merged, direction);
            for (int i = 0; i < boardSize; i++) {
                temp[i][j] = newCol[i];
                temps[i][j] = newsCol[i];
                if (newCol[i] != 0) {
                    srcR[i][j] = srcRowForCol[i];
                    srcC[i][j] = j;
                }
                mergedTemp[i][j] = merged[i];
            }
        }
    } else {
        for (int i = 0; i < boardSize; i++) {
            int line[MAX_BOARD_SIZE], newLine[MAX_BOARD_SIZE], srcColForLine[MAX_BOARD_SIZE], newsLine[MAX_BOARD_SIZE];
            int merged[MAX_BOARD_SIZE] = {0};
            for (int j = 0; j < boardSize; j++) line[j] = num[i][j];
            totalScore += processLine(line, newLine, srcColForLine, newsLine, merged, direction);
            for (int j = 0; j < boardSize; j++) {
                temp[i][j] = newLine[j];
                temps[i][j] = newsLine[j];
                if (newLine[j] != 0) {
                    srcR[i][j] = i;
                    srcC[i][j] = srcColForLine[j];
                }
                mergedTemp[i][j] = merged[j];
            }
        }
    }
    int move = memcmp(num, temps, sizeof(num)) != 0;
    if (move) {
    	if (is_animation) {
    		anim.active = 1;
	        anim.progress = 0.0f;
	        anim.duration = 0.15f;
	        anim.direction = direction;
	        anim.moveScore = totalScore;
	        memcpy(anim.oldBoard, num, sizeof(num));
	        memcpy(anim.newBoard, temp, sizeof(temp));
	        memcpy(anim.newsBoard, temps, sizeof(temp));
	        memcpy(anim.srcRow, srcR, sizeof(srcR));
	        memcpy(anim.srcCol, srcC, sizeof(srcC));
	        memcpy(anim.merged, mergedTemp, sizeof(anim.merged));
		} else {
			ai_move();
		}
    }
}

// 根据当前AI模式执行一次AI移动（调用get_action并执行）
void performAIMove(void) {
	if (anim.active) return;
    Board board;
    memcpy(board.board, num, sizeof(num));
    int action = get_action(&board, boardSize);
    int is_show_move = 0;
    switch (action) {
        case 0: performMove(5, is_show_move); break;
        case 1: performMove(2, is_show_move); break;
        case 2: performMove(1, is_show_move); break;
        case 3: performMove(3, is_show_move); break;
        default: break;
    }
}

// 处理玩家输入（键盘或AI自动移动），更新帧率并执行移动（注释部分试图使用计时器，但是感觉不太好用） 
void move_forward(void) {
	if (currentAIMode == AI_MODE_OFF) {
		SetTargetFPS(60);
		if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_KP_5)) 			performMove(5, 1);
		else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || IsKeyPressed(KEY_KP_2)) 	performMove(2, 1);
		else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_KP_1)) 	performMove(1, 1);
		else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_KP_3)) 	performMove(3, 1);
	} 
//	else if (currentAIMode == AI_MODE_SLOW) {
//		aiMoveInterval = 0.1f;
//		if (!gameOver && !anim.active && currentAIMode != AI_MODE_OFF) {
//	        if (aiMoveInterval <= 0.0f) {
//	            performAIMove();
//	        } else {
//	            aiMoveTimer += GetFrameTime();
//	            if (aiMoveTimer >= aiMoveInterval) {
//	                aiMoveTimer = 0.0f;
//	                performAIMove();
//	            }
//	        }
//	    }
//	} 
	else {
		switch (currentAIMode) {
			case AI_MODE_SLOW:    SetTargetFPS(10);		break;
		    case AI_MODE_MIDDLE:  SetTargetFPS(30);		break;
		    case AI_MODE_FAST:    SetTargetFPS(300);	break;
		    case AI_MODE_NOLIMIT: SetTargetFPS(0);		break;
		}
		ai_move();
		if (random_color) randomizeColors(boardSize);
	}
}

// 绘制游戏结束或胜利的遮罩层，显示提示文字
void DrawGameOverlay(void) {
    if (!gameOver) return;
    gameOverTimer += GetFrameTime();
    if (gameOverTimer < 0.4f) return;		// 短暂延迟
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 180});
    if (gameWon) {
        const char* msg = "You Win!";
        int msgFontSize = 80;
        int msgWidth = MeasureText(msg, msgFontSize);
        const char* continueMsg = "Press C to Continue";
        const char* restartMsg = "Press R to Restart";
        int smallFont = 30;
        int continueWidth = MeasureText(continueMsg, smallFont);
        int restartWidth = MeasureText(restartMsg, smallFont);
        int maxWidth = (continueWidth > restartWidth) ? continueWidth : restartWidth;
        int boxPadding = 30;
        int boxWidth = maxWidth + boxPadding * 2;
        int spacing = 15;
        int totalHeight = msgFontSize + spacing + smallFont + spacing + smallFont;
        int boxHeight = totalHeight + boxPadding * 2;
        int boxX = (GetScreenWidth() - boxWidth) / 2;
        int boxY = (GetScreenHeight() - boxHeight) / 2;
        DrawRectangleRounded((Rectangle){boxX, boxY, boxWidth, boxHeight}, 0.2f, 0, (Color){20, 20, 30, 200});
        int msgX = boxX + (boxWidth - msgWidth) / 2;
        int msgY = boxY + boxPadding;
        DrawTextSafe(msg, msgX, msgY, msgFontSize, WHITE);
        int continueX = boxX + (boxWidth - continueWidth) / 2;
        int continueY = msgY + msgFontSize + spacing;
        DrawTextSafe(continueMsg, continueX, continueY, smallFont, LIGHTGRAY);
        int restartX = boxX + (boxWidth - restartWidth) / 2;
        int restartY = continueY + smallFont + spacing;
        DrawTextSafe(restartMsg, restartX, restartY, smallFont, LIGHTGRAY);
    } else {
        const char* msg = "Game Over";
        int msgFontSize = 80;
        int msgWidth = MeasureText(msg, msgFontSize);
        const char* restartMsg = "Press R to Restart";
        int restartFontSize = 30;
        int restartWidth = MeasureText(restartMsg, restartFontSize);
        int spacing = 20;
        int totalHeight = msgFontSize + spacing + restartFontSize;
        int boxPadding = 30;
        int boxWidth = (msgWidth > restartWidth ? msgWidth : restartWidth) + boxPadding * 2;
        int boxHeight = totalHeight + boxPadding * 2;
        int boxX = (GetScreenWidth() - boxWidth) / 2;
        int boxY = (GetScreenHeight() - boxHeight) / 2;
        DrawRectangleRounded((Rectangle){boxX, boxY, boxWidth, boxHeight}, 0.2f, 0, (Color){20, 20, 30, 200});
        int msgX = boxX + (boxWidth - msgWidth) / 2;
        int msgY = boxY + boxPadding;
        DrawTextSafe(msg, msgX, msgY, msgFontSize, WHITE);
        int restartX = boxX + (boxWidth - restartWidth) / 2;
        int restartY = msgY + msgFontSize + spacing;
        DrawTextSafe(restartMsg, restartX, restartY, restartFontSize, LIGHTGRAY);
    }
}

// 处理游戏结束或胜利时的键盘输入（C继续，R重置）
void HandleGameOverInput(void) {
    if (!gameOver) return;
    if (gameWon) {
        if (IsKeyPressed(KEY_C)) {
            gameOver = 0;
            gameWon = 0;
            showWinDialog = 0;
            gameOverTimer = 0.0f;
        } else if (IsKeyPressed(KEY_R)) {
            resetGame(num);
            anim.active = 0;
            gameOverTimer = 0.0f;
        }
    } else {
        if (IsKeyPressed(KEY_R)) {
            resetGame(num);
            anim.active = 0;
            gameOverTimer = 0.0f;
        }
    }
    currentAIMode = AI_MODE_OFF;
}

// 绘制载入存档确认对话框
void DrawLoadConfirm(void) {
    DrawStaticGameUI();
    draw_static_board();
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 180});
    int boxWidth = 400;
    int boxHeight = 200;
    int boxX = (SCREEN_WIDTH - boxWidth)/2;
    int boxY = (SCREEN_HEIGHT - boxHeight)/2;
    DrawRectangleRounded((Rectangle){boxX, boxY, boxWidth, boxHeight}, 0.2f, 0, (Color){50,50,70,255});
    const char* msg = "Continue previous game?";
    int msgFont = 30;
    int msgW = MeasureText(msg, msgFont);
    DrawTextSafe(msg, boxX + (boxWidth-msgW)/2, boxY+40, msgFont, WHITE);
    int btnWid = 100;
    int btnHgt = 50;
    int btnSpacing = 50;
    int btnYesX = boxX + (boxWidth - btnWid*2 - btnSpacing)/2;
    int btnNoX = btnYesX + btnWid + btnSpacing;
    int btnYpos = boxY + boxHeight - btnHgt - 30;
    DrawRectangleRec((Rectangle){btnYesX, btnYpos, btnWid, btnHgt}, LIGHTGRAY);
    DrawRectangleLinesEx((Rectangle){btnYesX, btnYpos, btnWid, btnHgt}, 2, DARKGRAY);
    DrawTextSafe("Yes", btnYesX+35, btnYpos+15, 25, BLACK);
    DrawRectangleRec((Rectangle){btnNoX, btnYpos, btnWid, btnHgt}, LIGHTGRAY);
    DrawRectangleLinesEx((Rectangle){btnNoX, btnYpos, btnWid, btnHgt}, 2, DARKGRAY);
    DrawTextSafe("No", btnNoX+35, btnYpos+15, 25, BLACK);
}

// 处理载入确认对话框的鼠标点击
void HandleLoadConfirmInput(void) {
    if (confirmDelayFrames > 0) {
        confirmDelayFrames--;
        return;
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();
        int boxWidth = 400;
        int boxHeight = 200;
        int boxX = (SCREEN_WIDTH - boxWidth)/2;
        int boxY = (SCREEN_HEIGHT - boxHeight)/2;
        int btnWid = 100;
        int btnHgt = 50;
        int btnSpacing = 50;
        int btnYesX = boxX + (boxWidth - btnWid*2 - btnSpacing)/2;
        int btnNoX = btnYesX + btnWid + btnSpacing;
        int btnYpos = boxY + boxHeight - btnHgt - 30;
        Rectangle btnYes = {btnYesX, btnYpos, btnWid, btnHgt};
        Rectangle btnNo  = {btnNoX, btnYpos, btnWid, btnHgt};
        if (CheckCollisionPointRec(mouse, btnYes)) {
            gameState = STATE_PLAYING;
            anim.active = 0;
            pendingBoardSizeIdx = -1;
        } else if (CheckCollisionPointRec(mouse, btnNo)) {
            prefs.currentBoardSizeIdx = pendingBoardSizeIdx;
            boardSize = SUPPORTED_SIZES[pendingBoardSizeIdx];
            currentTheme = prefs.themeForSize;
            currentEffect = prefs.effectForSize;
            CalculateBoardLayout();
            resetGame(num);
            gameState = STATE_PLAYING;
            anim.active = 0;
            pendingBoardSizeIdx = -1;
        }
    }
}

