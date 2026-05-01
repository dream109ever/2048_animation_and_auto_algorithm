// 绘制背景板，独立逻辑，不受其他动画影响，每帧更新 

#include <math.h>
#include <stdlib.h>
#include "background.h"

#define MAX_PARTICLES 500		// 最大粒子数量
 
// 经典主题渐变颜色数组（顶部/底部颜色）
static GradientColor classicGradients[] = {
    { { 238, 228, 218, 255 }, { 250, 240, 230, 255 } },
    { { 245, 225, 195, 255 }, { 255, 235, 205, 255 } },
    { { 250, 210, 170, 255 }, { 255, 220, 180, 255 } },
    { { 240, 220, 200, 255 }, { 250, 230, 210, 255 } },
    { { 238, 228, 218, 255 }, { 250, 240, 230, 255 } }
};
// 暗黑主题渐变
static GradientColor darkGradients[] = {
    { { 40, 44, 52, 255 }, { 60, 64, 72, 255 } },
    { { 70, 70, 90, 255 }, { 90, 90, 110, 255 } },
    { { 80, 80, 105, 255 }, { 100, 100, 125, 255 } },
    { { 55, 58, 72, 255 }, { 75, 78, 92, 255 } },
    { { 40, 44, 52, 255 }, { 60, 64, 72, 255 } }
};
// 森林主题渐变
static GradientColor forestGradients[] = {
    { { 95, 115, 73, 255 }, { 135, 155, 93, 255 } },
    { { 135, 165, 95, 255 }, { 175, 205, 115, 255 } },
    { { 145, 185, 100, 255 }, { 185, 225, 120, 255 } },
    { { 115, 145, 85, 255 }, { 155, 185, 105, 255 } },
    { { 95, 115, 73, 255 }, { 135, 155, 93, 255 } }
};
// 暖色主题渐变
static GradientColor warmGradients[] = {
    { { 230, 150, 90, 255 }, { 255, 200, 140, 255 } },
    { { 255, 180, 100, 255 }, { 255, 220, 150, 255 } },
    { { 255, 200, 120, 255 }, { 255, 230, 170, 255 } },
    { { 245, 170, 105, 255 }, { 255, 210, 155, 255 } },
    { { 230, 150, 90, 255 }, { 255, 200, 140, 255 } }
};
// 主题背景配置数组（按枚举顺序）
ThemeBackground themeBgs[THEME_COUNT] = {
    { classicGradients, sizeof(classicGradients)/sizeof(GradientColor), 24.0f },
    { darkGradients,    sizeof(darkGradients)/sizeof(GradientColor),    24.0f },
    { forestGradients,  sizeof(forestGradients)/sizeof(GradientColor),  24.0f },
    { warmGradients,    sizeof(warmGradients)/sizeof(GradientColor),    24.0f }
};
static Particle particles[MAX_PARTICLES];   // 粒子数组
static float particleTimer = 0.0f;     		// 粒子生成计时器

// 初始化背景：清除所有粒子
void InitBackground(void) {
    for (int i = 0; i < MAX_PARTICLES; i++) particles[i].life = 0.0f;
    particleTimer = 0.0f;
}

// 更新背景：生成新粒子，更新现有粒子的位置和生命周期
void UpdateBackground(float dt) {
    particleTimer += dt;
    // 每隔约0.02秒尝试生成一个新粒子
    if (particleTimer > 0.02f) {
        particleTimer = 0.0f;
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (particles[i].life <= 0.0f) {
            	// 随机位置和速度
            	particles[i].pos = (Vector2){ (float)(rand() % SCREEN_WIDTH), (float)(rand() % SCREEN_HEIGHT) };
                particles[i].vel = (Vector2){ (float)(rand() % 100 - 50) * 0.5f, (float)(rand() % 100 - 50) * 0.5f };
                // 根据当前主题确定基础色相
            	float baseHue = 0.0f;
            	int rand_grcol = rand() % 100;
            	if (rand_grcol < 40) {
            		switch (currentTheme) {
					    case THEME_CLASSIC:
					        baseHue = 210.0f;
					        break;
					    case THEME_DARK:
					        baseHue = 45.0f;
					        break;
					    case THEME_FOREST:
					        baseHue = 60.0f;
					        break;
					    case THEME_WARM:
					        baseHue = 180.0f;
					        break;
					}
				} else if (rand_grcol < 80) {
					switch (currentTheme) {
				        case THEME_CLASSIC:
				            baseHue = 35.0f;
				            break;
				        case THEME_DARK:
				            baseHue = 260.0f;
				            break;
				        case THEME_FOREST:
				            baseHue = 85.0f;
				            break;
				        case THEME_WARM:
				            baseHue = 45.0f;
				            break;
				        default:
				            baseHue = 0.0f;
				    }
				} else {
					baseHue = (float)(rand() % 360);
				}
				// 色相微调，避免过于单一
				float hue = baseHue + (rand() % 120 - 60);
			    if (hue < 0) hue += 360;
			    if (hue >= 360) hue -= 360;
			    float saturation = 0.6f + (rand() % 40) / 100.0f;
			    float brightness = 0.7f + (rand() % 30) / 100.0f;
			    Color col = ColorFromHSV(hue, saturation, brightness);
			    particles[i].color = col;
                particles[i].maxLife = 1.3f + (rand() % 100) / 100.0f;
                particles[i].life = particles[i].maxLife;
                break;
            }
        }
    }
    // 更新所有存活粒子
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life > 0.0f) {
            particles[i].pos.x += particles[i].vel.x * dt;
            particles[i].pos.y += particles[i].vel.y * dt;
            particles[i].life -= dt;
            // 边界环绕
            if (particles[i].pos.x < 0) particles[i].pos.x = SCREEN_WIDTH;
            if (particles[i].pos.x > SCREEN_WIDTH) particles[i].pos.x = 0;
            if (particles[i].pos.y < 0) particles[i].pos.y = SCREEN_HEIGHT;
            if (particles[i].pos.y > SCREEN_HEIGHT) particles[i].pos.y = 0;
        }
    }
}

// 绘制背景：渐变背景 + 粒子效果
void DrawBackground(void) {
    ThemeBackground* bg = &themeBgs[currentTheme];
    // 计算当前渐变插值位置（周期循环）
    float t = fmod(GetTime(), bg->cycleTime) / bg->cycleTime;
    int idx1 = (int)(t * bg->count) % bg->count;
    int idx2 = (idx1 + 1) % bg->count;
    float subT = (t * bg->count) - idx1;
    // 插值计算顶部和底部颜色
    Color topColor = (Color){
        (unsigned char)(bg->gradients[idx1].top.r + (bg->gradients[idx2].top.r - bg->gradients[idx1].top.r) * subT),
        (unsigned char)(bg->gradients[idx1].top.g + (bg->gradients[idx2].top.g - bg->gradients[idx1].top.g) * subT),
        (unsigned char)(bg->gradients[idx1].top.b + (bg->gradients[idx2].top.b - bg->gradients[idx1].top.b) * subT),
        255
    };
    Color bottomColor = (Color){
        (unsigned char)(bg->gradients[idx1].bottom.r + (bg->gradients[idx2].bottom.r - bg->gradients[idx1].bottom.r) * subT),
        (unsigned char)(bg->gradients[idx1].bottom.g + (bg->gradients[idx2].bottom.g - bg->gradients[idx1].bottom.g) * subT),
        (unsigned char)(bg->gradients[idx1].bottom.b + (bg->gradients[idx2].bottom.b - bg->gradients[idx1].bottom.b) * subT),
        255
    };
    // 绘制垂直渐变矩形
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, topColor, bottomColor);
    // 绘制所有粒子（透明度随生命衰减）
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life > 0.0f) {
            unsigned char alpha = (unsigned char)(255 * (particles[i].life / particles[i].maxLife));
            Color col = particles[i].color;
            col.a = alpha;
            DrawCircleV(particles[i].pos, 3.0f, col);
        }
    }
}

