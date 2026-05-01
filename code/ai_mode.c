// 使用带Alpha-Beta剪枝的Minimax实现ai自动算法 

#include <math.h>
#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include "ai_mode.h" 

// -----------------评估函数-----------------
// 统计空白格子数，返回浮点数
float calculate_empty(const Board *board, int boardsize) {
	int empty = 0;
    for (int i = 0; i < boardsize; i++)
        for (int j = 0; j < boardsize; j++)
            if (board->board[i][j] == 0) empty++;
    return (float)empty;
}

// 找出棋盘中的最大数字
float calculate_maxnum(const Board *board, int boardsize) {
    int max = 0;
    for (int i = 0; i < boardsize; i++)
        for (int j = 0; j < boardsize; j++)
            if (board->board[i][j] > max) max = board->board[i][j];
    return (float)max;
}

// 计算平滑度（相邻数字的对数差绝对值之和的负值）
float calculate_smoothness(const Board *board, int boardsize) {
	float smooth = 0.0f;
    for (int i = 0; i < boardsize; i++) {
        for (int j = 0; j < boardsize; j++) {
            if (board->board[i][j] != 0) {
                float val = log2f(board->board[i][j] + 1);
                if (i + 1 < boardsize) {
                    float v2 = log2f(board->board[i + 1][j] + 1);
                    smooth -= fabsf(v2 - val);
                }
                if (j + 1 < boardsize) {
                    float v2 = log2f(board->board[i][j + 1] + 1);
                    smooth -= fabsf(v2 - val);
                }
                if (i - 1 >= 0) {
                    float v2 = log2f(board->board[i - 1][j] + 1);
                    smooth -= fabsf(v2 - val);
                }
                if (j - 1 >= 0) {
                    float v2 = log2f(board->board[i][j - 1] + 1);
                    smooth -= fabsf(v2 - val);
                }
            }
        }
    }
    return smooth;
}

// 计算单调性（行和列的递增/递减趋势）
float calculate_monotonicity(const Board *board, int boardsize) {
    float totals[4] = {0};
    for (int i = 0; i < boardsize; i++) {
        int current = 0;
        int next = current + 1;
        while (next < boardsize) {
            while (next < boardsize && board->board[i][next] == 0) next++;
            if (next >= boardsize) next--;
            float current_val = (board->board[i][current] != 0) ? log2f(board->board[i][current]) : 0;
            float next_val = (board->board[i][next] != 0) ? log2f(board->board[i][next]) : 0;
            if (current_val > next_val)
                totals[0] += next_val - current_val;
            else
                totals[1] += current_val - next_val;
            current = next;
            next++;
        }
    }
    for (int j = 0; j < boardsize; j++) {
        int current = 0;
        int next = current + 1;
        while (next < boardsize) {
            while (next < boardsize && board->board[next][j] == 0) next++;
            if (next >= boardsize) next--;
            float current_val = (board->board[current][j] != 0) ? log2f(board->board[current][j]) : 0;
            float next_val = (board->board[next][j] != 0) ? log2f(board->board[next][j]) : 0;
            if (current_val > next_val)
                totals[2] += next_val - current_val;
            else
                totals[3] += current_val - next_val;
            current = next;
            next++;
        }
    }
    float max1 = (totals[0] > totals[1]) ? totals[0] : totals[1];
    float max2 = (totals[2] > totals[3]) ? totals[2] : totals[3];
    return max1 + max2;
}

// DFS 标记同值的连通块
void mark_island(const Board *board, int x, int y, int value, int marked[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int boardsize) {
    if (x < 0 || x >= boardsize || y < 0 || y >= boardsize) return;
    if (board->board[x][y] == 0) return;
    if (board->board[x][y] != value) return;
    if (marked[x][y]) return;
    marked[x][y] = 1;
    mark_island(board, x + 1, y, value, marked, boardsize);
    mark_island(board, x - 1, y, value, marked, boardsize);
    mark_island(board, x, y + 1, value, marked, boardsize);
    mark_island(board, x, y - 1, value, marked, boardsize);
}

// 计算连通块数量（用于评估）
float calculate_islands(const Board *board, int boardsize) {
    int marked[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    for (int i = 0; i < boardsize; i++) for (int j = 0; j < boardsize; j++) marked[i][j] = 1;
    float islands = 0;
    for (int i = 0; i < boardsize; i++) {
        for (int j = 0; j < boardsize; j++) {
            if (board->board[i][j] != 0) {
            	marked[i][j] = 0;
			}
        }
    }
    for (int i = 0; i < boardsize; i++) {
        for (int j = 0; j < boardsize; j++) {
            if (board->board[i][j] != 0 && !marked[i][j]) {
                islands++;
                mark_island(board, i, j, board->board[i][j], marked, boardsize);
            }
        }
    }
    return islands;
}

// 综合评估函数（权重可调）
float calculate_evaluation(const Board *board, int boardsize) {
	float empty = calculate_empty(board, boardsize);
    float emptyWeight = 2.7f + (logf(17) - logf(empty + 1)) * 0.1f;
    float maxnumWeight = 1.0f;
    float smoothWeight = 0.1f;
    float monoWeight = 1.0f;
    return emptyWeight * logf(empty + 1) +
           maxnumWeight * calculate_maxnum(board, boardsize) +
           smoothWeight * calculate_smoothness(board, boardsize) +
           monoWeight * calculate_monotonicity(board, boardsize);
}

// -----------------搜索算法-----------------
// 搜索最佳移动
AI2048Result search_best(Board *this_board, int depth, float alpha, float beta, int playerTurn, int boardsize) {
	AI2048Result result;
    result.move = -1;
    result.score = 0.0f;
    if (playerTurn) {				// 玩家（AI）回合，最大化得分
        float bestScore = alpha;
        int bestMove = -1;
        for (int direction = 0; direction < 4; direction++) {
            Board newBoard;
            copy_board(&newBoard, this_board);
            int changed = move(&newBoard, direction, 1, boardsize);
            if (changed) {
                AI2048Result subResult;
                if (depth == 0) {
                    subResult.move = direction;
                    subResult.score = calculate_evaluation(&newBoard, boardsize);
                } else {
                    subResult = search_best(&newBoard, depth - 1, bestScore, beta, 0, boardsize);
                }
                if (subResult.score > bestScore) {
                    bestScore = subResult.score;
                    bestMove = direction;
                }
                if (bestScore > beta) {
                    result.move = bestMove;
                    result.score = beta;
                    return result;
                }
            }
        }
        result.move = bestMove;
        result.score = bestScore;
        return result;
    } else {						// 随机放置新数字（对手），最小化得分
        float bestScore = beta;
        Board newBoard;
        copy_board(&newBoard, this_board);
        int freeBlocks[MAX_BOARD_SIZE * MAX_BOARD_SIZE][2];
        int freeCount = get_free_blocks(&newBoard, freeBlocks, boardsize);
        float *score_2 = (float*)malloc(freeCount * sizeof(float));
        float *score_4 = (float*)malloc(freeCount * sizeof(float));
        // 评估每个空格放置2或4后的平滑度和岛屿数
        for (int i = 0; i < freeCount; i++) {
            Board tempBoard;
            copy_board(&tempBoard, &newBoard);
            add_xy(&tempBoard, freeBlocks[i][0], freeBlocks[i][1], 2);
            score_2[i] = -calculate_smoothness(&tempBoard, boardsize) + calculate_islands(&tempBoard, boardsize);
            copy_board(&tempBoard, &newBoard);
            add_xy(&tempBoard, freeBlocks[i][0], freeBlocks[i][1], 4);
            score_4[i] = -calculate_smoothness(&tempBoard, boardsize) + calculate_islands(&tempBoard, boardsize);
        }
        // 找出最差的情况（得分最高即为AI不希望出现的）
        float maxScore = -FLT_MAX;
        for (int i = 0; i < freeCount; i++) {
            if (score_2[i] > maxScore) maxScore = score_2[i];
            if (score_4[i] > maxScore) maxScore = score_4[i];
        }
        int worstCount = 0;
        int worstSituations[16][3];
        for (int i = 0; i < freeCount; i++) {
            if (score_2[i] == maxScore) {
                worstSituations[worstCount][0] = freeBlocks[i][0];
                worstSituations[worstCount][1] = freeBlocks[i][1];
                worstSituations[worstCount][2] = 2;
                worstCount++;
            }
            if (score_4[i] == maxScore) {
                worstSituations[worstCount][0] = freeBlocks[i][0];
                worstSituations[worstCount][1] = freeBlocks[i][1];
                worstSituations[worstCount][2] = 4;
                worstCount++;
            }
        }
        // 继续搜索，取最小值
        for (int k = 0; k < worstCount; k++) {
            Board newBoard2;
            copy_board(&newBoard2, this_board);
            add_xy(&newBoard2, worstSituations[k][0], worstSituations[k][1], worstSituations[k][2]);
            AI2048Result subResult = search_best(&newBoard2, depth, alpha, bestScore, 1, boardsize);
            if (subResult.score < bestScore) {
                bestScore = subResult.score;
            }
            if (bestScore < alpha) {
                free(score_2); free(score_4);
                result.move = -1;
                result.score = alpha;
                return result;
            }
        }
        free(score_2); free(score_4);
        result.move = -1;
        result.score = bestScore;
        return result;
    }
}

// 返回最佳移动方向（根据棋盘大小调整搜索深度）
int getBestMove(Board *board, int boardsize) {
//	return rand() % 4;				// 本项目未解之bug，望后佬能从蛛丝马迹中修复它 
	int depth = 0;
	switch(boardsize) {
		case 4:
			depth = 4;
			break;
		case 5:
			depth = 2;
			break;
		case 6:
			depth = 1;
			break;
		case 8:
			depth = 1;
			break;
		default:
			depth = 1;
			break;
	}
	AI2048Result result = search_best(board, depth, -1000000.0f, 1000000.0f, 1, boardsize);
	// 若返回无效，降低深度重试
	while (depth > 0) {
		if (result.move == -1) result = search_best(board, --depth, -1000000.0f, 1000000.0f, 1, boardsize);
		else break;
	}
	if (result.move == -1) result.move = rand() % 4;
	// 验证移动是否合法
	if (result.move != -1) {
        Board testBoard;
        copy_board(&testBoard, board);
        if (move(&testBoard, result.move, 0, boardsize)) {
            return result.move;
        }
    }
    // 若仍无效，遍历四个方向找第一个可移动的
    for (int d = 0; d < 4; d++) {
        Board testBoard;
        copy_board(&testBoard, board);
        if (move(&testBoard, d, 0, boardsize)) {
            return d;
        }
    }
	return -1;
}

// 外部接口：获取AI动作 
int get_action(Board *board, int boardsize) {
    return getBestMove(board, boardsize);
}

// -----------------算法展示-----------------
// 测试四个方向是否可移动（测试bug用） 
// 这个bug是当前自动算法对某些棋盘，分明可以移动但返回不可移动而误判失败（未修复，但影响不大） 
void test_moves(Board *board, int boardsize) {
    for (int d = 0; d < 4; d++) {
        Board copy;
        copy_board(&copy, board);
        int changed = move(&copy, d, 0, boardsize);
        printf("方向 %d: %s\n", d, changed ? "可移动" : "不可移动");
    }
}

// 演示AI自动移动指定步数，每隔 each_steps 步打印棋盘（测试用）
// 本文件及board.c已完整实现4种规模下2048游戏的黑窗口及算法展示 
void show_ai(Board *board, int steps, int each_steps, int test, int boardsize, int choose) {
	const char *directions[] = {"左", "上", "右", "下"};
	printf("初始棋盘：\n");
	choose_print(board, boardsize, choose);
	for (int i = 0; i < steps; i++) {
		int ai_move = getBestMove(board, boardsize);
		if (test != -1 && i >= test) {
			test_moves(board, boardsize);
		}
		if (ai_move == -1) {
			printf("无法移动，游戏结束。");
			printf("最终得分：%d\n", board->score);
            break;
		}
		move(board, ai_move, 1, boardsize);
		if (i % each_steps == 0) {
			printf("第 %d 步，执行移动方向: %s\n", i + 1, directions[ai_move]);
			add_random(board, boardsize);
			if (i == steps - 1) {
				printf("最终棋盘：\n");
				choose_print(board, boardsize, choose);
				printf("最终得分：%d\n", board->score);
			} else {
				printf("移动后的棋盘：\n");
				choose_print(board, boardsize, choose);
//				printf("按回车继续...\n");
//				getchar();
			}
		} else {
			add_random(board, boardsize);
		}
	}
}

