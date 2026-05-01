#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef _WIN32
    #include <direct.h>
    #include <io.h>
#else
    #include <sys/stat.h>
    #include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "raylib.h"
#include "ai_mode.h" 

#define NUM_SIZE_OPTIONS 4
#define BACK_BTN_WIDTH  100
#define BACK_BTN_HEIGHT 50
#define BACK_BTN_MARGIN 20
#define RESET_BTN_WIDTH  100
#define RESET_BTN_HEIGHT 50
#define RESET_BTN_MARGIN 20
#define SIZE_COUNT 4

typedef enum {
    THEME_CLASSIC,
    THEME_DARK,
    THEME_FOREST,
    THEME_WARM,
    THEME_COUNT
} Theme;
typedef enum {
    EFFECT_FLASH,
    EFFECT_PARTICLE,
    EFFECT_RIPPLE,
    EFFECT_SCALE,
    EFFECT_RINGS,
    EFFECT_COUNT
} Effect;
typedef enum {
    AI_MODE_OFF = 0,
    AI_MODE_SLOW,
    AI_MODE_MIDDLE,
    AI_MODE_FAST,
    AI_MODE_NOLIMIT,
    AI_MODE_COUNT
} AIMode;
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_LOAD_CONFIRM
} GameState;
typedef struct {
    int bestScores[4];
    int currentBoardSizeIdx;
    int themeForSize;
    int effectForSize;
} GamePreferences;
typedef struct {
    int boardSize;
    int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    int currentScore;
    int gameOver;
    int gameWon;
    int showWinDialog;
} GameProgress;
typedef struct {
    GameProgress progresses[NUM_SIZE_OPTIONS];
    int version;
} MultiGameProgress;
typedef struct {
    int active;
    int oldBoard[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    int newBoard[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    int newsBoard[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    int srcRow[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    int srcCol[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    int oldValue[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    float progress;
    float duration;
    int direction;
    int moveScore;
    int merged[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
} AnimState;
typedef struct {
    Vector2 pos;
    Vector2 vel;
    Color color;
    float life;
    float maxLife;
} Particle;
typedef struct {
    Color top;
    Color bottom;
} GradientColor;
typedef struct {
    GradientColor* gradients;
    int count;
    float cycleTime;
} ThemeBackground;

extern Color boardBgColor;
extern Color tileColors[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
extern Theme currentTheme;
extern Effect currentEffect;
extern GameState gameState;
extern Font chFont;
extern int useChineseFont;
extern int currentAIMode;
extern int boardSize;
extern int gameOver;
extern int gameWon;
extern int showWinDialog;
extern int currentScore;
extern int random_color;
extern int confirmDelayFrames;
extern int pendingBoardSizeIdx;
extern int tileSize, drawSize, fontSize;
extern int boardOffsetX, boardOffsetY, boardWidth;
extern int boardBgX, boardBgY, boardBgW, boardBgH;
extern int btnX, btnY, btnW, btnH, btnSpacing;
extern int effectBtnX, effectBtnY, effectBtnW, effectBtnH, effectBtnSpacing;
extern int aiBtnX, aiBtnY, aiBtnW, aiBtnH, aiBtnSpacing;
extern int randomColorBtnX, randomColorBtnY, randomColorBtnW, randomColorBtnH;
extern int num[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
extern char dataPath[1024];
extern char fullPath[1024];
extern char g_savePath[1024];
extern char g_gameSavePath[1024];
extern float gameOverTimer;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int SUPPORTED_SIZES[SIZE_COUNT];
extern const char* themeNames[4];
extern const char* effectNames[5];
extern const char* aiModeNames[5];
extern AnimState anim;
extern GamePreferences prefs;

Color GetTileColor(int value);
void randomizeColors(int boardSize);
void CalculateBoardLayout(void);
void savePreferences(void);
void loadPreferences(void);
void init_save_files(void);
void init_font(void);
void deleteGameProgressForCurrentSize(void);
int loadGameProgressForSize(int sizeIdx, GameProgress* prog);
void saveGameProgress(void);
int getCurrentBestScore(void);
void setCurrentBestScore(int score);
void resetGame(int num[MAX_BOARD_SIZE][MAX_BOARD_SIZE]);
void init_game(void);
int processLine(int line[MAX_BOARD_SIZE], int newLine[MAX_BOARD_SIZE], int srcIdx[MAX_BOARD_SIZE], int newsLine[MAX_BOARD_SIZE], int merged[MAX_BOARD_SIZE], int direction);
void DrawTextSafe(const char* text, int x, int y, int fontSize, Color color);
void DrawTextCentered(const char* text, int rectX, int rectY, int rectW, int rectH, int fontSize, Color color);
void DrawNumberInTile(int value, int x, int y, int tileWidth, int tileHeight, Color color);

#endif

