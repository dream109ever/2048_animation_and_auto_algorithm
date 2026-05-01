// 包含全局变量、存档逻辑、初始化等内容 

#include "globals.h"

// 全局变量定义及注释
Color boardBgColor;												// 棋盘背景颜色
Color tileColors[MAX_BOARD_SIZE][MAX_BOARD_SIZE];				// 每个格子独立颜色（随机模式使用）
Theme currentTheme = THEME_CLASSIC;								// 当前主题
Effect currentEffect = EFFECT_FLASH;							// 当前动画效果
GameState gameState = STATE_MENU;								// 游戏状态（菜单、游戏中、结束等）
Font chFont = {0};												// 中文字体资源
int useChineseFont = 0;											// 是否使用中文字体（若加载失败则为0）
int currentAIMode = AI_MODE_OFF;								// 当前AI模式（关闭、慢速、中速、快速、无限制）
int boardSize = 4;												// 棋盘大小（4,5,6,8）
int gameOver = 0;												// 游戏是否结束
int gameWon = 0;												// 是否已获胜（达到2048）
int showWinDialog = 1;											// 是否显示胜利对话框（仅在首次获胜时显示）
int currentScore = 0;											// 当前得分
int random_color = 0;											// 是否启用随机颜色模式
int confirmDelayFrames = 0;										// 确认延迟帧计数（用于按钮防误触）
int pendingBoardSizeIdx = -1;									// 待更改的棋盘大小索引
int tileSize = 0, drawSize = 0, fontSize = 0;					// 数字绘制尺寸和字体大小
int boardOffsetX = 0, boardOffsetY = 0, boardWidth = 0;			// 棋盘在窗口中的位置和宽度
int boardBgX = 0, boardBgY = 0, boardBgW = 0, boardBgH = 0;		// 棋盘背景矩形
int btnX = 0, btnY = 0, btnW = 0, btnH = 0, btnSpacing = 0;		// 主题切换按钮布局
int effectBtnX = 0, effectBtnY = 0, effectBtnW = 0, effectBtnH = 0, effectBtnSpacing = 0;	// 效果切换按钮布局
int aiBtnX = 0, aiBtnY = 0, aiBtnW = 0, aiBtnH = 0, aiBtnSpacing = 0;						// AI模式按钮布局
int randomColorBtnX = 0, randomColorBtnY = 0, randomColorBtnW = 0, randomColorBtnH = 0;		// 随机颜色按钮布局
int num[MAX_BOARD_SIZE][MAX_BOARD_SIZE] = {{0}};				// 当前棋盘数值
char dataPath[1024] = {0};										// 配置数据存储路径
char fullPath[1024] = {0};										// 临时完整路径
char g_savePath[1024] = {0};									// 最佳成绩保存文件路径
char g_gameSavePath[1024] = {0};								// 游戏进度保存文件路径
float gameOverTimer = 0.0f;										// 游戏结束计时器
const int SCREEN_WIDTH = 1300;									// 窗口宽度
const int SCREEN_HEIGHT = 1000;									// 窗口高度
const int SUPPORTED_SIZES[SIZE_COUNT] = {4, 5, 6, 8};			// 支持的棋盘大小
const char* themeNames[4] = { "Classic", "Dark", "Forest", "Warm" };						// 主题名称
const char* effectNames[5] = { "Flash", "Particle", "Ripple", "Scale", "Rings" };			// 效果名称
const char* aiModeNames[5] = { "Off", "Slow", "Middle", "Fast", "No Limit" };				// AI模式名称
const char* fontPaths[] = {										// 中文字体备选路径
	"C:/Windows/Fonts/simhei.ttf",  // 黑体
    "C:/Windows/Fonts/msyh.ttc",    // 微软雅黑
    "C:/Windows/Fonts/simsun.ttc",  // 宋体
};

AnimState anim = {0};											// 动画状态结构体
GamePreferences prefs = {										// 游戏偏好设置（最佳分数、主题、效果等）
    .bestScores = {0, 0, 0, 0},
    .currentBoardSizeIdx = 0,
    .themeForSize = THEME_CLASSIC,
    .effectForSize = EFFECT_FLASH
};

// 根据数值和当前主题返回格子颜色
Color GetTileColor(int value) {
    if (currentTheme == THEME_CLASSIC) {
        switch (value) {
            case 0: return (Color){205, 193, 180, 255};
            case 2: return (Color){238, 228, 218, 255};
            case 4: return (Color){237, 224, 200, 255};
            case 8: return (Color){242, 177, 121, 255};
            case 16: return (Color){245, 149, 99, 255};
            case 32: return (Color){246, 124, 95, 255};
            case 64: return (Color){246, 94, 59, 255};
            case 128: return (Color){237, 207, 114, 255};
            case 256: return (Color){237, 204, 97, 255};
            case 512: return (Color){237, 200, 80, 255};
            case 1024: return (Color){237, 197, 63, 255};
            case 2048: return (Color){237, 194, 46, 255};
            case 4096: return (Color){237, 190, 30, 255};
            case 8192: return (Color){230, 170, 20, 255};
            case 16384: return (Color){220, 140, 10, 255};
            case 32768: return (Color){210, 110, 0, 255};
            case 65536: return (Color){200, 80, 0, 255};
            case 131072: return (Color){180, 50, 0, 255};
            case 262144: return (Color){240, 210, 80, 255};
            case 524288: return (Color){245, 220, 90, 255};
            case 1048576: return (Color){250, 230, 100, 255};
            case 2097152: return (Color){255, 240, 110, 255};
            case 4194304: return (Color){255, 245, 120, 255};
            case 8388608: return (Color){255, 250, 130, 255};
            case 16777216: return (Color){255, 255, 140, 255};
            default: return (Color){60, 58, 50, 255};
        }
    }
    else if (currentTheme == THEME_DARK) {
        switch (value) {
            case 0: return (Color){40, 44, 52, 255};
	        case 2: return (Color){60, 50, 80, 255};
	        case 4: return (Color){80, 60, 100, 255};
	        case 8: return (Color){100, 70, 120, 255};
	        case 16: return (Color){120, 80, 140, 255};
	        case 32: return (Color){140, 90, 160, 255};
	        case 64: return (Color){160, 100, 180, 255};
	        case 128: return (Color){180, 110, 200, 255};
	        case 256: return (Color){200, 120, 220, 255};
	        case 512: return (Color){210, 130, 230, 255};
	        case 1024: return (Color){220, 140, 240, 255};
	        case 2048: return (Color){230, 150, 250, 255};
	        case 4096: return (Color){200, 80, 200, 255};
	        case 8192: return (Color){210, 70, 210, 255};
	        case 16384: return (Color){220, 60, 220, 255};
	        case 32768: return (Color){230, 50, 230, 255};
	        case 65536: return (Color){240, 60, 240, 255};
	        case 131072: return (Color){245, 70, 245, 255};
	        case 262144: return (Color){250, 80, 250, 255};
	        case 524288: return (Color){252, 90, 252, 255};
	        case 1048576: return (Color){254, 100, 254, 255};
	        case 2097152: return (Color){255, 110, 255, 255};
	        case 4194304: return (Color){255, 120, 255, 255};
	        case 8388608: return (Color){255, 130, 255, 255};
	        case 16777216: return (Color){255, 140, 255, 255};
	        default: return (Color){30, 30, 40, 255};
        }
    }
    else if (currentTheme == THEME_FOREST) {
        switch (value) {
            case 0: return (Color){200, 220, 180, 255};
            case 2: return (Color){180, 210, 140, 255};
            case 4: return (Color){160, 200, 120, 255};
            case 8: return (Color){140, 190, 100, 255};
            case 16: return (Color){120, 180, 80, 255};
            case 32: return (Color){100, 170, 70, 255};
            case 64: return (Color){90, 160, 60, 255};
            case 128: return (Color){110, 170, 70, 255};
            case 256: return (Color){130, 180, 80, 255};
            case 512: return (Color){150, 190, 90, 255};
            case 1024: return (Color){170, 200, 100, 255};
            case 2048: return (Color){190, 210, 110, 255};
            case 4096: return (Color){200, 215, 115, 255};
            case 8192: return (Color){210, 220, 120, 255};
            case 16384: return (Color){220, 225, 125, 255};
            case 32768: return (Color){230, 230, 130, 255};
            case 65536: return (Color){240, 235, 135, 255};
            case 131072: return (Color){250, 240, 140, 255};
            case 262144: return (Color){245, 240, 145, 255};
            case 524288: return (Color){240, 245, 150, 255};
            case 1048576: return (Color){235, 250, 155, 255};
            case 2097152: return (Color){230, 250, 160, 255};
            case 4194304: return (Color){225, 255, 165, 255};
            case 8388608: return (Color){220, 255, 170, 255};
            case 16777216: return (Color){215, 255, 175, 255};
            default: return (Color){50, 80, 30, 255};
        }
    }
    else if (currentTheme == THEME_WARM) {
        switch (value) {
            case 0: return (Color){255, 230, 210, 255};
            case 2: return (Color){255, 210, 170, 255};
            case 4: return (Color){255, 190, 130, 255};
            case 8: return (Color){255, 170, 100, 255};
            case 16: return (Color){255, 150, 80, 255};
            case 32: return (Color){255, 140, 70, 255};
            case 64: return (Color){255, 130, 60, 255};
            case 128: return (Color){255, 140, 70, 255};
            case 256: return (Color){255, 150, 80, 255};
            case 512: return (Color){255, 160, 90, 255};
            case 1024: return (Color){255, 170, 100, 255};
            case 2048: return (Color){255, 180, 110, 255};
            case 4096: return (Color){255, 185, 115, 255};
            case 8192: return (Color){255, 190, 120, 255};
            case 16384: return (Color){255, 195, 125, 255};
            case 32768: return (Color){255, 200, 130, 255};
            case 65536: return (Color){255, 205, 135, 255};
            case 131072: return (Color){255, 210, 140, 255};
            case 262144: return (Color){255, 215, 145, 255};
            case 524288: return (Color){255, 220, 150, 255};
            case 1048576: return (Color){255, 225, 155, 255};
            case 2097152: return (Color){255, 230, 160, 255};
            case 4194304: return (Color){255, 235, 165, 255};
            case 8388608: return (Color){255, 240, 170, 255};
            case 16777216: return (Color){255, 245, 175, 255};
            default: return (Color){180, 80, 40, 255};
        }
    }
    return (Color){205, 193, 180, 255};
}

// 随机化所有格子颜色（仅当 random_color 开启时使用）
void randomizeColors(int boardsize) {
    for (int i = 0; i < boardsize; i++) {
        for (int j = 0; j < boardsize; j++) {
            tileColors[i][j] = (Color){ rand() % 256, rand() % 256, rand() % 256, rand() % 256 };
        }
    }
    boardBgColor = (Color){ rand() % 256, rand() % 256, rand() % 256, 200 };
}

// 根据棋盘大小和屏幕尺寸计算所有界面元素的布局位置（数值仅仅是找了一个差不多的，没啥规律） 
void CalculateBoardLayout(void) {
    const int fixedBoardSize = 820;
    boardBgX = (SCREEN_WIDTH - 350 - fixedBoardSize) / 2;
    boardBgY = (SCREEN_HEIGHT + 50 - fixedBoardSize) / 2;
    boardBgW = fixedBoardSize;
    boardBgH = fixedBoardSize;
    switch (boardSize) {
        case 4:
            tileSize = 195;
            drawSize = 170;
            fontSize = (int)(tileSize * 0.3f);
            break;
        case 5:
            tileSize = 158;
            drawSize = 138;
            fontSize = (int)(tileSize * 0.4f);
            break;
        case 6:
            tileSize = 132;
            drawSize = 115;
            fontSize = (int)(tileSize * 0.5f);
            break;
        case 8:
            tileSize = 100;
            drawSize = 85;
            fontSize = (int)(tileSize * 0.55f);
            break;
        default:
            tileSize = 195;
            drawSize = 170;
            fontSize = (int)(tileSize * 0.3f);
            break;
    }
    boardWidth = tileSize * boardSize;
    int offset = (fixedBoardSize - boardWidth + tileSize - drawSize) / 2;
    boardOffsetX = boardBgX + offset;
    boardOffsetY = boardBgY + offset;
    // 主题切换按钮位置
    btnW = 100;
    btnH = 60;
    btnSpacing = 25;
    btnX = boardOffsetX + boardWidth + 70;
    btnY = boardOffsetY - 10;
    // 效果切换按钮位置
    effectBtnW = 100;
    effectBtnH = 60;
    effectBtnSpacing = 25;
    effectBtnX = btnX;
    effectBtnY = btnY + THEME_COUNT * (btnH + btnSpacing) + 40;
    // AI模式按钮位置
    aiBtnX = btnX + btnW + 35;
    aiBtnW = 160;
    aiBtnH = 60;
    aiBtnSpacing = 25;
    // 随机颜色模式按钮位置
    randomColorBtnX = aiBtnX;
	randomColorBtnY = aiBtnY + AI_MODE_COUNT * (aiBtnH + aiBtnSpacing) + 140;
	randomColorBtnW = aiBtnW;
	randomColorBtnH = 70;
}

// 保存偏好设置到文件（二进制格式）
void savePreferences(void) {
    FILE* fp = fopen(g_savePath, "wb");
    if (fp) {
        fwrite(&prefs, sizeof(GamePreferences), 1, fp);
        fclose(fp);
    }
}

// 加载偏好设置，并试图兼容旧版本文件格式（不能用还是建议把旧文件直接删了） 
void loadPreferences(void) {
    FILE* fp = fopen(g_savePath, "rb");
    if (fp) {
        size_t read = fread(&prefs, sizeof(GamePreferences), 1, fp);
        if (read != 1) {
            int oldBest[4];
            int oldBoardIdx, oldTheme, oldEffect;
            fseek(fp, 0, SEEK_SET);
            int items = fread(oldBest, sizeof(int), 4, fp);
            items += fread(&oldBoardIdx, sizeof(int), 1, fp);
            items += fread(&oldTheme, sizeof(int), 1, fp);
            items += fread(&oldEffect, sizeof(int), 1, fp);
            if (items == 7) {
                for (int i = 0; i < 4; i++) prefs.bestScores[i] = oldBest[i];
                prefs.currentBoardSizeIdx = oldBoardIdx;
                prefs.themeForSize = oldTheme;
                prefs.effectForSize = oldEffect;
                savePreferences();
            } else {
                prefs = (GamePreferences){
                    .bestScores = {0,0,0,0},
                    .currentBoardSizeIdx = 0,
                    .themeForSize = THEME_CLASSIC,
                    .effectForSize = EFFECT_FLASH
                };
                savePreferences();
            }
        }
        fclose(fp);
    } else {
        savePreferences();
    }
}

// 初始化保存文件路径（Windows 下使用 %APPDATA%，Linux 下使用 ~/.config/）
void init_save_files(void) {
#ifdef _WIN32
    const char* baseDir = getenv("APPDATA");
    if (baseDir) snprintf(dataPath, sizeof(dataPath), "%s\\My2048", baseDir);
    else strcpy(dataPath, ".");
    if (_access(dataPath, 0) != 0) _mkdir(dataPath);
#else
    const char* baseDir = getenv("HOME");
    if (baseDir) snprintf(dataPath, sizeof(dataPath), "%s/.config/my2048", baseDir);
    else strcpy(dataPath, ".");
    if (access(dataPath, F_OK) != 0) mkdir(dataPath, 0755);
#endif
    snprintf(g_savePath, sizeof(g_savePath), "%s/2048_best.dat", dataPath);
    snprintf(g_gameSavePath, sizeof(g_gameSavePath), "%s/game_save.dat", dataPath);
	loadPreferences();
}

// 初始化中文字体（尝试加载黑体，失败则用默认字体）
void init_font(void) {
	chFont = LoadFontEx(fontPaths[0], 64, NULL, 0);
    if (chFont.texture.id != 0) {
    	SetTextureFilter(chFont.texture, TEXTURE_FILTER_BILINEAR);
	    useChineseFont = 1;
	} else {
	    useChineseFont = 0;
	    chFont = GetFontDefault();
	}
}

// 删除当前棋盘大小的游戏进度（用于重置或切换大小）
void deleteGameProgressForCurrentSize(void) {
    MultiGameProgress multi = {0};
    multi.version = 2;
    FILE* fp = fopen(g_gameSavePath, "rb");
    if (fp) {
        fread(&multi, sizeof(MultiGameProgress), 1, fp);
        fclose(fp);
        if (multi.version != 2) {
            memset(&multi, 0, sizeof(MultiGameProgress));
            multi.version = 2;
        }
    } else {
        multi.version = 2;
    }
    int idx = -1;
    for (int i = 0; i < SIZE_COUNT; i++) {
        if (boardSize == SUPPORTED_SIZES[i]) {
            idx = i;
            break;
        }
    }
    if (idx != -1) {
        memset(&multi.progresses[idx], 0, sizeof(GameProgress));
        multi.progresses[idx].boardSize = boardSize;
    }
    fp = fopen(g_gameSavePath, "wb");
    if (fp) {
        fwrite(&multi, sizeof(MultiGameProgress), 1, fp);
        fclose(fp);
    }
}

// 加载指定棋盘大小的游戏进度，返回是否有效进度
int loadGameProgressForSize(int sizeIdx, GameProgress* prog) {
    if (sizeIdx < 0 || sizeIdx >= SIZE_COUNT) return 0;
    FILE* fp = fopen(g_gameSavePath, "rb");
    if (!fp) return 0;
    MultiGameProgress multi = {0};
    size_t read = fread(&multi, sizeof(MultiGameProgress), 1, fp);
    fclose(fp);
    if (read != 1 || multi.version != 2) return 0;
    *prog = multi.progresses[sizeIdx];
    if (prog->boardSize != SUPPORTED_SIZES[sizeIdx]) {
        memset(prog, 0, sizeof(GameProgress));
        return 0;
    }
    int hasProgress = 0;
    for (int i = 0; i < MAX_BOARD_SIZE && !hasProgress; i++)
        for (int j = 0; j < MAX_BOARD_SIZE && !hasProgress; j++)
            if (prog->board[i][j] != 0) hasProgress = 1;
    if (prog->currentScore > 0) hasProgress = 1;
    return hasProgress;
}

// 保存当前游戏进度到文件（仅当处于游戏中状态）
void saveGameProgress(void) {
    if (gameState != STATE_PLAYING) return;
    MultiGameProgress multi = {0};
    multi.version = 2;
    FILE* fp = fopen(g_gameSavePath, "rb");
    if (fp) {
        fread(&multi, sizeof(MultiGameProgress), 1, fp);
        fclose(fp);
        if (multi.version != 2) {
            memset(&multi, 0, sizeof(MultiGameProgress));
            multi.version = 2;
        }
    } else {
        multi.version = 2;
    }
    int idx = -1;
    for (int i = 0; i < SIZE_COUNT; i++) {
        if (boardSize == SUPPORTED_SIZES[i]) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return;
    multi.progresses[idx].boardSize = boardSize;
    memcpy(multi.progresses[idx].board, num, sizeof(num));
    multi.progresses[idx].currentScore = currentScore;
    multi.progresses[idx].gameOver = gameOver;
    multi.progresses[idx].gameWon = gameWon;
    multi.progresses[idx].showWinDialog = showWinDialog;
    fp = fopen(g_gameSavePath, "wb");
    if (fp) {
        fwrite(&multi, sizeof(MultiGameProgress), 1, fp);
        fclose(fp);
    }
}

// 获取当前棋盘大小的最佳分数
int getCurrentBestScore(void) {
    return prefs.bestScores[prefs.currentBoardSizeIdx];
}

// 设置当前棋盘大小的最佳分数（若大于原值则更新）
void setCurrentBestScore(int score) {
    if (score > prefs.bestScores[prefs.currentBoardSizeIdx]) {
        prefs.bestScores[prefs.currentBoardSizeIdx] = score;
        savePreferences();
    }
}

// 重置游戏（清空棋盘，随机生成两个初始块）
void resetGame(int num[MAX_BOARD_SIZE][MAX_BOARD_SIZE]) {
	Board board;
	init_board(&board, boardSize);
    memcpy(num, &board.board, sizeof(int) * 64);
    currentScore = 0;
    gameOver = 0;
    gameWon = 0;
    showWinDialog = 1;
    gameOverTimer = 0.0;
    currentAIMode = AI_MODE_OFF;
    if (random_color) randomizeColors(boardSize);
}

// 初始化游戏（加载配置、设置棋盘大小、布局等）
void init_game(void) {
	init_save_files();
	boardSize = SUPPORTED_SIZES[prefs.currentBoardSizeIdx];
	currentTheme = prefs.themeForSize;
	currentEffect = prefs.effectForSize;
    gameState = STATE_MENU;
	CalculateBoardLayout();
	resetGame(num);
}

// 处理一行（或一列）的移动合并，返回本次得分。支持方向（左/右/上/下）由调用者转换
// line: 原始行数组，newLine: 移动合并后的新数组，srcIdx: 原始索引映射（用于动画）
// newsLine: 合并后的临时值（用于动画），merged: 标记是否合并，direction: 2或3表示反向（右或下）
// （个人认为最最复杂的一个） 
int processLine(int line[MAX_BOARD_SIZE], int newLine[MAX_BOARD_SIZE], int srcIdx[MAX_BOARD_SIZE], int newsLine[MAX_BOARD_SIZE], int merged[MAX_BOARD_SIZE], int direction) {
    int rev = (direction == 2 || direction == 3);
    int tempVal[MAX_BOARD_SIZE] = {0};
    int tempsVal[MAX_BOARD_SIZE] = {0};
    int tempSrc[MAX_BOARD_SIZE] = {-1, -1, -1, -1, -1, -1, -1, -1};
    int pos = 0;
    int scoreGain = 0;
    for (int i = 0; i < boardSize; i++) merged[i] = 0;
    if (rev) {
    	for (int j = boardSize - 1; j >= 0; j--) {
	        if (line[j] != 0) {
	            tempVal[pos] = line[j];
	            tempsVal[pos] = line[j];
	            tempSrc[pos] = j;
	            pos++;
	        }
	    }
	} else {
		for (int j = 0; j < boardSize; j++) {
	        if (line[j] != 0) {
	            tempVal[pos] = line[j];
	            tempsVal[pos] = line[j];
	            tempSrc[pos] = j;
	            pos++;
	        }
	    }
	}
    for (int i = 0; i < pos - 1; i++) {
        if (tempVal[i] == tempVal[i + 1]) {
            tempsVal[i] *= 2;
            scoreGain += tempsVal[i];
            merged[i] = 1;
            tempSrc[i] = tempSrc[i + 1];
            for (int j = i + 1; j < pos - 1; j++) {
                tempVal[j] = tempVal[j + 1];
                tempsVal[j] = tempsVal[j + 1];
                tempSrc[j] = tempSrc[j + 1];
            }
            pos--;
        }
    }
    if (rev) {
    	int newMerged[MAX_BOARD_SIZE] = {0};
	    for (int i = 0; i < pos; i++) newMerged[boardSize - 1 - i] = merged[i];
	    for (int j = 0; j < boardSize; j++) merged[j] = newMerged[j];
	    for (int i = 0; i < boardSize; i++) {
	        if (i < pos) {
	            newLine[boardSize - 1 - i] = tempVal[i];
	            newsLine[boardSize - 1 - i] = tempsVal[i];
	            srcIdx[boardSize - 1 - i] = tempSrc[i];
	        } else {
	            newLine[boardSize - 1 - i] = 0;
	            newsLine[boardSize - 1 - i] = 0;
	            srcIdx[boardSize - 1 - i] = -1;
	        }
	    }
	} else {
		for (int i = 0; i < boardSize; i++) {
	        if (i < pos) {
	            newLine[i] = tempVal[i];
	            newsLine[i] = tempsVal[i];
	            srcIdx[i] = tempSrc[i];
	        } else {
	            newLine[i] = 0;
	            newsLine[i] = 0;
	            srcIdx[i] = -1;
	        }
	    }
	}
    return scoreGain;
}

// 安全绘制文字（支持中文字体）
void DrawTextSafe(const char* text, int x, int y, int fontSize, Color color) {
    if (useChineseFont) {
    	Vector2 size = MeasureTextEx(chFont, text, fontSize, 0);
        DrawTextEx(chFont, text, (Vector2){x, y}, fontSize, 0, color);
    } else {
        DrawText(text, x, y, fontSize, color);
    }
}

// 在指定矩形区域内居中绘制文字
void DrawTextCentered(const char* text, int rectX, int rectY, int rectW, int rectH, int fontSize, Color color) {
    if (useChineseFont) {
        Vector2 size = MeasureTextEx(chFont, text, fontSize, 0);
        int textX = rectX + (rectW - (int)size.x) / 2;
        int textY = rectY + (rectH - fontSize) / 2;
        DrawTextEx(chFont, text, (Vector2){textX, textY}, fontSize, 0, color);
    } else {
        int textWidth = MeasureText(text, fontSize);
        int textX = rectX + (rectW - textWidth) / 2;
        int textY = rectY + (rectH - fontSize) / 2;
        DrawText(text, textX, textY, fontSize, color);
    }
}

// 在格子内绘制数字（处理多行显示和自适应字体大小）
void DrawNumberInTile(int value, int x, int y, int tileWidth, int tileHeight, Color color) {
    if (value == 0) return;
    char numStr[32];
    sprintf(numStr, "%d", value);
    int len = strlen(numStr);
    const int MAX_PER_LINE = 6;
    char line1[16] = {0};
    char line2[16] = {0};
    int lines = 1;
    if (len > MAX_PER_LINE) {
        int firstLen = len / 2;
        if (len % 2 != 0) firstLen++;
        strncpy(line1, numStr, firstLen);
        line1[firstLen] = '\0';
        strcpy(line2, numStr + firstLen);
        lines = 2;
    } else {
        strcpy(line1, numStr);
    }
    // 根据棋盘大小决定最大字体
    int maxFontSize;
    switch (boardSize) {
        case 4:  maxFontSize = (int)(tileSize * 0.3f); break;
        case 5:  maxFontSize = (int)(tileSize * 0.4f); break;
        case 6:  maxFontSize = (int)(tileSize * 0.5f); break;
        case 8:  maxFontSize = (int)(tileSize * 0.55f); break;
        default: maxFontSize = (int)(tileSize * 0.3f); break;
    }
    if (maxFontSize > fontSize) maxFontSize = fontSize;
    int currentFontSize = maxFontSize;
    int lineHeight = (lines == 1) ? tileHeight : (tileHeight / 2);
    // 自适应缩小字体直到整行能容纳
    for (int lineIdx = 0; lineIdx < lines; lineIdx++) {
        char* text = (lineIdx == 0) ? line1 : line2;
        int textWidth;
        do {
        	textWidth = MeasureTextEx(chFont, text, currentFontSize, 0).x;
            if (textWidth > tileWidth - 10) {
                currentFontSize--;
                if (currentFontSize < 8) currentFontSize = 8;
            } else {
                break;
            }
        } while (currentFontSize > 8);
    }
    int totalHeight = lines * lineHeight;
    int startY = y + (tileHeight - totalHeight) / 2;
    for (int lineIdx = 0; lineIdx < lines; lineIdx++) {
        char* text = (lineIdx == 0) ? line1 : line2;
        int textWidth = MeasureTextEx(chFont, text, currentFontSize, 0).x;
        int textX = x + (tileWidth - textWidth) / 2;
        int textY = startY + lineIdx * lineHeight + (lineHeight - currentFontSize) / 2;
        DrawTextSafe(text, textX, textY, currentFontSize, color);
    }
}

