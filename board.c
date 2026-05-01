// 2048游戏棋盘操作实现
// 包含棋盘初始化、移动逻辑、胜负判定、打印输出等功能 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"

// 初始化棋盘：清零、分数归零、随机生成两个数字（一个2，一个随机2/4）
void init_board(Board *board, int boardsize) {
	memset(board->board, 0, sizeof(board->board));
    board->score = 0;
    add_value(board, boardsize, 2);
    add_random(board, boardsize);
}

// 复制棋盘数据
void copy_board(Board *board, const Board *grid) {
	memcpy(board->board, grid->board, sizeof(board->board));
}

// 从一维数组初始化棋盘，同时设置分数（测试用） 
void init_board_from_flat(Board *board, int boardsize, const int arr[], int score) {
    for (int i = 0; i < boardsize; i++) {
        for (int j = 0; j < boardsize; j++) {
            board->board[i][j] = arr[i * boardsize + j];
        }
    }
    board->score = score;
}

// 在随机空格子放置指定数值，成功返回1，无空格返回0
int add_value(Board *board, int boardsize, int value) {
	int blocks[MAX_BOARD_SIZE * MAX_BOARD_SIZE][2];
    int free_cnt = get_free_blocks(board, blocks, boardsize);
    if (free_cnt == 0) return 0;
    int idx = rand() % free_cnt;
    int x = blocks[idx][0];
    int y = blocks[idx][1];
    board->board[x][y] = value;
    return 1;
}

// 随机添加一个2（90%）或4（10%）
int add_random(Board *board, int boardsize) {
	int value = (rand() % 10 < 9) ? 2 : 4;
    return add_value(board, boardsize, value);
}

// 外部接口：在给定二维数组中随机添加一个数字（原地修改）
// 主要为兼容外部数组的add函数 ，因为board内部逻辑使用Board类型，而外部动画使用数组类型 
void add(int num[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int boardsize) {
	Board board;
	memcpy(&board.board, num, sizeof(int) * MAX_BOARD_SIZE * MAX_BOARD_SIZE);
	add_random(&board, boardsize);
	memcpy(num, &board.board, sizeof(int) * MAX_BOARD_SIZE * MAX_BOARD_SIZE);
}

// 获取所有空格子的坐标，存入blocks二维数组，返回空格数量
int get_free_blocks(const Board *board, int blocks[MAX_BOARD_SIZE * MAX_BOARD_SIZE][2], int boardsize) {
	int count = 0;
    for (int i = 0; i < boardsize; i++) {
        for (int j = 0; j < boardsize; j++) {
            if (board->board[i][j] == 0) {
                blocks[count][0] = i;
                blocks[count][1] = j;
                ++count;
                if (count >= MAX_BOARD_SIZE * MAX_BOARD_SIZE) return count;
            }
        }
    }
    return count;
}

// 在指定坐标放置数值，若该格为空则成功返回1
int add_xy(Board *board, int x, int y, int value) {
	if (board->board[x][y] == 0) {
		board->board[x][y] = value;
		return 1;
	}
	return 0;
}

// 清空指定坐标的数值
void remove_xy(Board *board, int x, int y) {
	board->board[x][y] = 0;
}

// 向左移动并合并，is_change决定是否实际修改棋盘和分数
// 返回值表示棋盘是否发生变化（用于判断是否需要添加新块）
// 基本思路是压缩->合并->再压缩 
int move_left(Board *board, int is_change, int boardsize) {
	int single_score = 0;
    int is_ok = 0;
    int temp_board[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    int last_board[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    memcpy(temp_board, board->board, sizeof(board->board));
    memcpy(last_board, board->board, sizeof(board->board));
    // 对每一行进行左移合并
    for (int i = 0; i < boardsize; i++) {
        int row[MAX_BOARD_SIZE];
        for (int j = 0; j < boardsize; j++) row[j] = temp_board[i][j];
        // 去除零元素
        int no_zero[MAX_BOARD_SIZE] = {0};
        int nz_position = 0;
        for (int j = 0; j < boardsize; j++) if (row[j] != 0) no_zero[nz_position++] = row[j];
        for (int j = 0; j < boardsize; j++) row[j] = (j < nz_position) ? no_zero[j] : 0;
        // 相邻合并
        for (int j = 0; j < boardsize - 1; j++) {
            if (row[j] == row[j + 1] && row[j] != 0) {
                single_score += row[j] * 2;
                row[j] += row[j + 1];
                row[j + 1] = 0;
            }
        }
        // 再次去零
        nz_position = 0;
        for (int j = 0; j < boardsize; j++) if (row[j] != 0) no_zero[nz_position++] = row[j];
        for (int j = 0; j < boardsize; j++) row[j] = (j < nz_position) ? no_zero[j] : 0;
        for (int j = 0; j < boardsize; j++) temp_board[i][j] = row[j];
    }
    // 判断是否有变化
    if (memcmp(temp_board, last_board, sizeof(board->board)) != 0) is_ok = 1;
    if (is_change) {
        memcpy(board->board, temp_board, sizeof(board->board));
        board->score += single_score;
    }
    return is_ok;
}

// 向右移动：先水平翻转，调用左移，再翻转回来
int move_right(Board *board, int is_change, int boardsize) {
	for (int i = 0; i < boardsize; i++) {
        for (int j = 0; j < boardsize / 2; j++) {
            int temp = board->board[i][j];
            board->board[i][j] = board->board[i][boardsize - 1 - j];
            board->board[i][boardsize - 1 - j] = temp;
        }
    }
    int ret = move_left(board, is_change, boardsize);
    for (int i = 0; i < boardsize; i++) {
        for (int j = 0; j < boardsize / 2; j++) {
            int temp = board->board[i][j];
            board->board[i][j] = board->board[i][boardsize - 1 - j];
            board->board[i][boardsize - 1 - j] = temp;
        }
    }
    return ret;
}

// 顺时针旋转90度
// 个人觉得这个旋转算法是很好的一个 
void rotate_right(Board *board, int boardsize) {
	int row, temp;
	row = (boardsize % 2 == 0) ? boardsize / 2 : boardsize / 2 + 1;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < boardsize / 2; j++) {
            temp = board->board[i][j];
            board->board[i][j] = board->board[boardsize - 1 - j][i];
            board->board[boardsize - 1 - j][i] = board->board[boardsize - 1 - i][boardsize - 1 - j];
            board->board[boardsize - 1 - i][boardsize - 1 - j] = board->board[j][boardsize - 1 - i];
            board->board[j][boardsize - 1 - i] = temp;
        }
    }
}

// 逆时针旋转90度
void rotate_left(Board *board, int boardsize) {
	int row, temp;
	row = (boardsize % 2 == 0) ? boardsize / 2 : boardsize / 2 + 1;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < boardsize / 2; j++) {
            temp = board->board[i][j];
            board->board[i][j] = board->board[j][boardsize - 1 - i];
            board->board[j][boardsize - 1 - i] = board->board[boardsize - 1 - i][boardsize - 1 - j];
            board->board[boardsize - 1 - i][boardsize - 1 - j] = board->board[boardsize - 1 - j][i];
            board->board[boardsize - 1 - j][i] = temp;
        }
    }
}

// 向上移动：逆时针旋转后左移，再顺时针旋转回来
int move_up(Board *board, int is_change, int boardsize) {
	rotate_left(board, boardsize);
    int ret = move_left(board, is_change, boardsize);
    rotate_right(board, boardsize);
    return ret;
}

// 向下移动：顺时针旋转后左移，再逆时针旋转回来
int move_down(Board *board, int is_change, int boardsize) {
	rotate_right(board, boardsize);
    int ret = move_left(board, is_change, boardsize);
    rotate_left(board, boardsize);
    return ret;
}

// 统一移动接口，direction为方向常量
int move(Board *board, int direction, int is_change, int boardsize) {
	switch (direction) {
        case MOVE_LEFT:  return move_left(board, is_change, boardsize);
        case MOVE_UP:    return move_up(board, is_change, boardsize);
        case MOVE_RIGHT: return move_right(board, is_change, boardsize);
        case MOVE_DOWN:  return move_down(board, is_change, boardsize);
        default: return 0;
    }
}

// 检查是否获胜（出现2048）
// 仅针对标准2048游戏，因为大棋盘合成2048要显而易见容易得多 
int isGameWon(Board *board, int boardsize) {
    for (int i = 0; i < boardsize; i++) for (int j = 0; j < boardsize; j++) if (board->board[i][j] == 2048) return 1;
    return 0;
}

// 检查是否结束：无空格且相邻无相等
int isGameOver(Board *board, int boardsize) {
    for (int i = 0; i < boardsize; i++) for (int j = 0; j < boardsize; j++) if (board->board[i][j] == 0) return 0;
    for (int i = 0; i < boardsize; i++) for (int j = 0; j < boardsize - 1; j++) if (board->board[i][j] == board->board[i][j + 1]) return 0;
    for (int j = 0; j < boardsize; j++) for (int i = 0; i < boardsize - 1; i++) if (board->board[i][j] == board->board[i + 1][j]) return 0;
    return 1;
}

// 普通打印棋盘
void print_board(Board *board, int boardsize) {
	printf(" ");
	for (int i = 0; i < boardsize; i++) printf("------- ");
	printf("\n");
	for (int i = 0; i < boardsize; i++) {
		printf("|");
		for (int j = 0; j < boardsize; j++) printf("  \t|");
		printf("\n|");
		for (int j = 0; j < boardsize; j++) {
			if (board->board[i][j] == 0) printf("  \t|");
			else printf("%d\t|", board->board[i][j]);
		}
		printf("\n|");
		for (int j = 0; j < boardsize; j++) printf("  \t|");
		printf("\n ");
		for (int j = 0; j < boardsize; j++) printf("------- ");
		printf("\n");
	}
}

// 快速打印：使用缓冲区一次性输出，减少IO调用
void print_board_fast(Board *board, int boardsize) {
    char buffer[8192];
    char *p = buffer;
    int remaining = sizeof(buffer);
    int len;
    len = snprintf(p, remaining, " ");
    p += len; remaining -= len;
    for (int i = 0; i < boardsize; i++) {
        len = snprintf(p, remaining, "------- ");
        p += len; remaining -= len;
    }
    len = snprintf(p, remaining, "\n");
    p += len; remaining -= len;
    for (int i = 0; i < boardsize; i++) {
        len = snprintf(p, remaining, "|");
        p += len; remaining -= len;
        for (int j = 0; j < boardsize; j++) {
            len = snprintf(p, remaining, "  \t|");
            p += len; remaining -= len;
        }
        len = snprintf(p, remaining, "\n|");
        p += len; remaining -= len;
        for (int j = 0; j < boardsize; j++) {
            if (board->board[i][j] == 0) {
                len = snprintf(p, remaining, "  \t|");
            } else {
                len = snprintf(p, remaining, "%d\t|", board->board[i][j]);
            }
            p += len; remaining -= len;
        }
        len = snprintf(p, remaining, "\n|");
        p += len; remaining -= len;
        for (int j = 0; j < boardsize; j++) {
            len = snprintf(p, remaining, "  \t|");
            p += len; remaining -= len;
        }
        len = snprintf(p, remaining, "\n ");
        p += len; remaining -= len;
        for (int j = 0; j < boardsize; j++) {
            len = snprintf(p, remaining, "------- ");
            p += len; remaining -= len;
        }
        len = snprintf(p, remaining, "\n");
        p += len; remaining -= len;
    }
    fputs(buffer, stdout);
}

// 最快打印：针对特定棋盘大小（4,5,6,8）直接使用硬编码格式字符串
void print_board_fastest(Board *board, int boardsize) {
    switch(boardsize) {
    	case 4:
    		printf(" ------- ------- ------- ------- \n"
			       "|  \t|  \t|  \t|  \t|\n"
			       "|%d\t|%d\t|%d\t|%d\t|\n"
			       "|  \t|  \t|  \t|  \t|\n"
			       " ------- ------- ------- ------- \n"
			       "|  \t|  \t|  \t|  \t|\n"
			       "|%d\t|%d\t|%d\t|%d\t|\n"
			       "|  \t|  \t|  \t|  \t|\n"
			       " ------- ------- ------- ------- \n"
			       "|  \t|  \t|  \t|  \t|\n"
			       "|%d\t|%d\t|%d\t|%d\t|\n"
			       "|  \t|  \t|  \t|  \t|\n"
			       " ------- ------- ------- ------- \n"
			       "|  \t|  \t|  \t|  \t|\n"
			       "|%d\t|%d\t|%d\t|%d\t|\n"
			       "|  \t|  \t|  \t|  \t|\n"
			       " ------- ------- ------- ------- \n", 
			       board->board[0][0], board->board[0][1], board->board[0][2], board->board[0][3],
			       board->board[1][0], board->board[1][1], board->board[1][2], board->board[1][3],
			       board->board[2][0], board->board[2][1], board->board[2][2], board->board[2][3],
			       board->board[3][0], board->board[3][1], board->board[3][2], board->board[3][3]);
			break;
		case 5:
		    printf(" ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- \n",
		           board->board[0][0], board->board[0][1], board->board[0][2], board->board[0][3], board->board[0][4],
		           board->board[1][0], board->board[1][1], board->board[1][2], board->board[1][3], board->board[1][4],
		           board->board[2][0], board->board[2][1], board->board[2][2], board->board[2][3], board->board[2][4],
		           board->board[3][0], board->board[3][1], board->board[3][2], board->board[3][3], board->board[3][4],
		           board->board[4][0], board->board[4][1], board->board[4][2], board->board[4][3], board->board[4][4]);
		    break;
		case 6:
		    printf(" ------- ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- ------- \n",
		           board->board[0][0], board->board[0][1], board->board[0][2], board->board[0][3], board->board[0][4], board->board[0][5],
		           board->board[1][0], board->board[1][1], board->board[1][2], board->board[1][3], board->board[1][4], board->board[1][5],
		           board->board[2][0], board->board[2][1], board->board[2][2], board->board[2][3], board->board[2][4], board->board[2][5],
		           board->board[3][0], board->board[3][1], board->board[3][2], board->board[3][3], board->board[3][4], board->board[3][5],
		           board->board[4][0], board->board[4][1], board->board[4][2], board->board[4][3], board->board[4][4], board->board[4][5],
		           board->board[5][0], board->board[5][1], board->board[5][2], board->board[5][3], board->board[5][4], board->board[5][5]);
		    break;
		case 8:
		    printf(" ------- ------- ------- ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- ------- ------- ------- \n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           "|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|\n"
		           "|  \t|  \t|  \t|  \t|  \t|  \t|  \t|  \t|\n"
		           " ------- ------- ------- ------- ------- ------- ------- ------- \n",
		           board->board[0][0], board->board[0][1], board->board[0][2], board->board[0][3], board->board[0][4], board->board[0][5], board->board[0][6], board->board[0][7],
		           board->board[1][0], board->board[1][1], board->board[1][2], board->board[1][3], board->board[1][4], board->board[1][5], board->board[1][6], board->board[1][7],
		           board->board[2][0], board->board[2][1], board->board[2][2], board->board[2][3], board->board[2][4], board->board[2][5], board->board[2][6], board->board[2][7],
		           board->board[3][0], board->board[3][1], board->board[3][2], board->board[3][3], board->board[3][4], board->board[3][5], board->board[3][6], board->board[3][7],
		           board->board[4][0], board->board[4][1], board->board[4][2], board->board[4][3], board->board[4][4], board->board[4][5], board->board[4][6], board->board[4][7],
		           board->board[5][0], board->board[5][1], board->board[5][2], board->board[5][3], board->board[5][4], board->board[5][5], board->board[5][6], board->board[5][7],
		           board->board[6][0], board->board[6][1], board->board[6][2], board->board[6][3], board->board[6][4], board->board[6][5], board->board[6][6], board->board[6][7],
		           board->board[7][0], board->board[7][1], board->board[7][2], board->board[7][3], board->board[7][4], board->board[7][5], board->board[7][6], board->board[7][7]);
		    break;
		default:
			break;
	}
}

// 根据选择调用不同的打印函数（测试用，实际上后两种打印速度差不多，都显著快于第一种） 
void choose_print(Board *board, int boardsize, int choose) {
	switch(choose) {
		case 1:
			print_board(board, boardsize);
		case 2:
			print_board_fast(board, boardsize);
		default:
			print_board_fastest(board, boardsize);
	}
}

