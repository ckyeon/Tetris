﻿#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80

#define SINGLE 0
#define PLAYER1 1
#define PLAYER2 2
#define COMPUTER 3

#define ACTIVE_BLOCK ((tetris->block->type+11)*(-1))
#define CEILLING -1
#define EMPTY 0
#define WALL 1
#define INACTIVE_BLOCK (tetris->block->type+11)

#define BLOCK_SIZE 4

#define WIDTH 11
#define HEIGHT 23

typedef struct BLOCKS Blocks;
typedef struct TETRIS Tetris;
typedef struct BTM BattleTetrisManager;

void gotoxy(int x, int y) {
	COORD pos = { 2 * (short)x, (short)y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCULSOR } CURSOR_TYPE;	// 열거형 정의
void setcursortype(CURSOR_TYPE c) {
	CONSOLE_CURSOR_INFO CurInfo;	// typedef를 사용하여 enum 생략

	switch (c) {
	case NOCURSOR:
		CurInfo.dwSize = 1;
		CurInfo.bVisible = FALSE;
		break;
	case SOLIDCURSOR:
		CurInfo.dwSize = 100;
		CurInfo.bVisible = TRUE;
		break;
	case NORMALCULSOR:
		CurInfo.dwSize = 20;
		CurInfo.bVisible = TRUE;
		break;
	}

	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}


void Blocks_init(Blocks* blocks);
int Blocks_getColor(int num);

typedef struct BLOCKS
{
	void(*init)(Blocks* blocks) = Blocks_init;

	const int shape[7][4][BLOCK_SIZE][BLOCK_SIZE] = { // 블럭의 모양을 저장하는 배열
	{{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0}},
	{{0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0},{0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0}},
	{{0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0},{0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0}},
	{{0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},{0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0},{0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},{0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0}},
	{{0,0,0,0,0,0,1,0,1,1,1,0,0,0,0,0},{0,0,0,0,1,1,0,0,0,1,0,0,0,1,0,0},{0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0},{0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,0}},
	{{0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,1,0,0,1,1,0,0},{0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,0},{0,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0}},
	{{0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,1,1,0,0,1,0,0},{0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0},{0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0}}
	};
	int x;
	int y;
	int type;
	int rotation;
	int type_next;
	int (*getColor)(int num) = Blocks_getColor;
} Blocks;

void Blocks_init(Blocks* blocks)
{
	blocks->type_next = rand() % 7;
}

int Blocks_getColor(int num)
{
	if (num < 0)
		num *= -1;

	return num - 9;
}


void Tetris_init(Tetris* tetris, int game_x, int game_y, int status_x, int status_y, int owner);
void Tetris_resetGameCpy(Tetris* tetris);
void Tetris_drawGame(Tetris* tetris);
void Tetris_newBlock(Tetris* tetris, Blocks* block);
void Tetris_move_block(Tetris* tetris, Blocks* block, int dir);
void Tetris_dropBlock(Tetris* tetris);
bool Tetris_checkCrush(Tetris* tetris, int x, int y, int rotation);
void Tetris_initialGame(Tetris* tetris);
void Tetris_gameOver(Tetris* tetris);
void Tetris_set_gameDelay(Tetris* tetris, int cnt);
void Tetris_cnt_gameDelay(Tetris* tetris);
void Tetris_set_gameMsg(Tetris* tetris, int x, int y, int type, int val);
void Tetris_cnt_gameMsg(Tetris* tetris);
void Tetris_getKey(Tetris* tetris, Blocks* block);
void Tetris_checkLine(Tetris* tetris, Blocks* block);
void Tetris_getAttack(Tetris* tetris);

typedef struct TETRIS 
{
	void (*init)(Tetris* tetris, int game_x, int game_y, int status_x, int status_y, int owner) = Tetris_init;

	int fCnt;
	int owner;
	int score;
	int lineCnt;

	const int speed[11] = { 20, 20, 15, 10, 20, 10, 5, 15, 10, 2, 0 };
		int level;

	int game_x;
	int game_y;
	int status_x;
	int status_y;


	int gameOrg[HEIGHT][WIDTH];
	int gameCpy[HEIGHT][WIDTH];
		void (*resetGameCpy)(Tetris* tetris) = Tetris_resetGameCpy;

	void (*drawGame)(Tetris* tetris) = Tetris_drawGame;
	void (*newBlock)(Tetris* tetris, Blocks* block) = Tetris_newBlock;
		Blocks* block;
		void (*move_block)(Tetris* tetris, Blocks* block, int dir) = Tetris_move_block;
		void (*dropBlock)(Tetris* tetris) = Tetris_dropBlock;
		bool (*checkCrush)(Tetris* tetris, int x, int y, int rotation) = Tetris_checkCrush;
			bool crush_on;

	void (*initialGame)(Tetris* tetris) = Tetris_initialGame;

	void (*gameOver)(Tetris* tetris) = Tetris_gameOver;
		bool gameOver_on;
		int gameOverCnt;
		int gameOverP;

	void (*set_gameDelay)(Tetris* tetris, int cnt) = Tetris_set_gameDelay;
		bool gameDelay_on;
		int gameDelayCnt;
		void (*cnt_gameDelay)(Tetris* tetris) = Tetris_cnt_gameDelay;

	void (*set_gameMsg)(Tetris* tetris, int x, int y, int type, int val) = Tetris_set_gameMsg;
		int gameMsgCnt;
		void (*cnt_gameMsg)(Tetris* tetris) = Tetris_cnt_gameMsg;

	void (*getKey)(Tetris* tetris, Blocks* block) = Tetris_getKey;
	int keyCnt;

	void (*checkLine)(Tetris* tetris, Blocks* block) = Tetris_checkLine;
		int pushAttackReg[HEIGHT][WIDTH];
		int pushAttackRegP;

	void (*getAttack)(Tetris* tetris) = Tetris_getAttack;
		int getAttackReg[HEIGHT][WIDTH];
		int getAttackRegP;
		int attackQueue_x;
}Tetris;

void Tetris_init(Tetris* tetris, int game_x, int game_y, int status_x, int status_y, int owner)
{
	tetris->level = 1;
	tetris->score = 0;
	tetris->lineCnt = 0;
	tetris->keyCnt = 0;
	tetris->fCnt = tetris->speed[tetris->level];

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			tetris->gameOrg[i][j] = 0;
			tetris->gameCpy[i][j] = NULL;
			tetris->pushAttackReg[i][j] = -1;
			tetris->getAttackReg[i][j] = -1;
		}
	}
	tetris->pushAttackRegP = HEIGHT - 1;
	tetris->getAttackRegP = HEIGHT - 1;

	tetris->game_x = game_x;
	tetris->game_y = game_y;
	tetris->status_x = status_x;
	tetris->status_y = status_y;
	tetris->owner = owner;

	tetris->crush_on = false;
	tetris->gameDelay_on = false;
	tetris->gameDelayCnt = -1;
	tetris->gameOver_on = false;
	tetris->gameOverCnt = -1;
	tetris->gameMsgCnt = -1;

	tetris->initialGame(tetris);
	tetris->newBlock(tetris, tetris->block);
}

void Tetris_resetGameCpy(Tetris* tetris)
{
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
			tetris->gameCpy[i][j] = 999;
	}
}

void Tetris_drawGame(Tetris* tetris)
{
	for (int j = 0; j < WIDTH - 1; j++)
	{
		if (tetris->gameOrg[3][j] == EMPTY)
			tetris->gameOrg[3][j] = CEILLING;
	}

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (tetris->gameCpy[i][j] != tetris->gameOrg[i][j])
			{
				gotoxy(tetris->game_x + j, tetris->game_y + i);
				if (tetris->gameOrg[i][j] == EMPTY)
				{
					printf("  ");
				}
				else if (tetris->gameOrg[i][j] == CEILLING)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);
					printf(". ");
				}
				else if (tetris->gameOrg[i][j] == WALL)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);
					printf("▩");
				}
				else if (tetris->gameOrg[i][j] < 0)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), tetris->block->getColor(tetris->gameOrg[i][j]));
					printf("▣");
				}
				else if (tetris->gameOrg[i][j] > 0)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), tetris->block->getColor(tetris->gameOrg[i][j]));
					printf("■");
				}
			}
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			tetris->gameCpy[i][j] = tetris->gameOrg[i][j];
		}
	}

	if(tetris->owner == 1)
	{
		gotoxy(tetris->status_x, tetris->status_y + HEIGHT - 2);
		printf("Score: %d", tetris->score);
	}
	if (tetris->owner == 2)
	{
		gotoxy(tetris->status_x + WIDTH + 6, tetris->status_y + HEIGHT - 2);
		printf("Score: %d", tetris->score);
	}
}

void Tetris_newBlock(Tetris* tetris, Blocks* block)
{
	block->x = (WIDTH / 2) - 1;
	block->y = 0;
	block->type = block->type_next;
	block->type_next = rand() % 7;
	block->rotation = 0;

	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		for (int j = 0; j < BLOCK_SIZE; j++)
		{
			if (block->shape[block->type][block->rotation][i][j] == 1)
				tetris->gameOrg[block->y + i][block->x + j] = ACTIVE_BLOCK;
		}
	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), block->type_next + 2);
	for (int i = 1; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			gotoxy(tetris->status_x + j, tetris->status_y + i + 2);
			
			if (block->shape[block->type_next][0][i][j] == 1)
				printf("■");
			else
				printf("  ");
		}
	}
}

void Tetris_move_block(Tetris* tetris, Blocks* block, int dir)
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		for (int j = 0; j < BLOCK_SIZE; j++)
		{
			if (block->shape[block->type][block->rotation][i][j] == 1)
				tetris->gameOrg[block->y + i][block->x + j] = EMPTY;
		}
	}

	switch (dir)
	{
	case LEFT:
		for (int i = 0; i < BLOCK_SIZE; i++)
		{
			for (int j = 0; j < BLOCK_SIZE; j++)
			{
				if (block->shape[block->type][block->rotation][i][j] == 1)
					tetris->gameOrg[block->y + i][block->x + j - 1] = ACTIVE_BLOCK;
			}
		}
		(block->x)--;
		break;

	case RIGHT:
		for (int i = 0; i < BLOCK_SIZE; i++)
		{
			for (int j = 0; j < BLOCK_SIZE; j++)
			{
				if (block->shape[block->type][block->rotation][i][j] == 1)
					tetris->gameOrg[block->y + i][block->x + j + 1] = ACTIVE_BLOCK;
			}
		}
		(block->x)++;
		break;

	case DOWN:
		for (int i = 0; i < BLOCK_SIZE; i++)
		{
			for (int j = 0; j < BLOCK_SIZE; j++)
			{
				if (block->shape[block->type][block->rotation][i][j] == 1)
					tetris->gameOrg[block->y + i + 1][block->x + j] = ACTIVE_BLOCK;
			}
		}
		(block->y)++;
		break;

	case UP:
		block->rotation = (block->rotation + 1) % 4;
		for (int i = 0; i < BLOCK_SIZE; i++)
		{
			for (int j = 0; j < BLOCK_SIZE; j++)
			{
				if (block->shape[block->type][block->rotation][i][j] == 1)
					tetris->gameOrg[block->y + i][block->x + j] = ACTIVE_BLOCK;
			}
		}
		break;

	case 100:
		block->rotation = (block->rotation + 1) % 4;
		for (int i = 0; i < BLOCK_SIZE; i++)
		{
			for (int j = 0; j < BLOCK_SIZE; j++)
			{
				if (block->shape[block->type][block->rotation][i][j] == 1)
					tetris->gameOrg[block->y + i - 1][block->x + j] = ACTIVE_BLOCK;
			}
		}
		(block->y)--;
		break;
	}
}

void Tetris_dropBlock(Tetris* tetris)
{
	if (tetris->fCnt > 0)
		(tetris->fCnt)--;
	bool can_down = tetris->checkCrush(tetris, tetris->block->x, tetris->block->y + 1, tetris->block->rotation);

	if (tetris->crush_on == true && can_down == true)
	{
		tetris->move_block(tetris, tetris->block, DOWN);
		tetris->fCnt = tetris->speed[tetris->level];
		tetris->crush_on = false;
	}

	if (tetris->crush_on == true && can_down == false && (tetris->fCnt) == 0)
	{
		if ((tetris->gameDelayCnt) == -1)
		{
			tetris->drawGame(tetris);
			tetris->set_gameDelay(tetris, 5);
		}
		else if (tetris->gameDelayCnt == 0)
		{
			for (int i = 0; i < HEIGHT; i++)
			{
				for (int j = 0; j < WIDTH; j++)
				{
					if (tetris->gameOrg[i][j] == ACTIVE_BLOCK)
						tetris->gameOrg[i][j] = INACTIVE_BLOCK;
				}
			}
			tetris->gameDelayCnt = -1;
			tetris->crush_on = false;
			tetris->checkLine(tetris, tetris->block);
			tetris->getAttack(tetris);

			if (tetris->gameOver_on == false)
				tetris->newBlock(tetris, tetris->block);
			tetris->fCnt = tetris->speed[tetris->level];
		}
		return;
	}
	if (tetris->crush_on == false && can_down == true && tetris->fCnt == 0)
	{
		tetris->move_block(tetris, tetris->block, DOWN);
		tetris->fCnt = tetris->speed[tetris->level];
	}
	if (tetris->crush_on == false && can_down == false)
	{
		tetris->crush_on = true;
		tetris->fCnt = tetris->speed[1];
	}
}

bool Tetris_checkCrush(Tetris* tetris, int x, int y, int rotation)
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		for (int j = 0; j < BLOCK_SIZE; j++)
		{
			if (tetris->block->shape[tetris->block->type][rotation][i][j] == 1 && tetris->gameOrg[y + i][x + j] > 0)
				return false;
		}
	}
	return true;
}

void Tetris_initialGame(Tetris* tetris)
{
	for (int j = 1; j < WIDTH; j++)
	{
		tetris->gameOrg[3][j] = CEILLING;
	}
	for (int i = 1; i < HEIGHT; i++)
	{
		tetris->gameOrg[i][0] = WALL;
		tetris->gameOrg[i][WIDTH - 1] = WALL;
	}
	for (int j = 0; j < WIDTH; j++)
	{
		tetris->gameOrg[HEIGHT - 1][j] = WALL;
	}

	gotoxy(tetris->status_x, tetris->status_y);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);

	switch (tetris->owner)
	{
	case 0:
		printf("< PLAYER >");
		tetris->attackQueue_x = tetris->game_x - 1;
		break;
	case 1:
		printf("<PLAYER1>");
		tetris->attackQueue_x = tetris->game_x - 1;
		break;
	case 2:
		printf("<PLAYER2>");
		tetris->attackQueue_x = tetris->game_x + WIDTH;
		break;
	case 3:
		printf("<PLAYER3>");
		tetris->attackQueue_x = tetris->game_x + WIDTH;
		break;
	}
	gotoxy(tetris->status_x, tetris->status_y + 1);
	printf("Next: ");
	gotoxy(tetris->status_x, tetris->status_y + 6);
	printf("Speed:%2d", tetris->speed[tetris->level]);
}

void Tetris_gameOver(Tetris* tetris)
{
	tetris->gameOverCnt++;
	
	if (tetris->gameOverCnt == 0)
		tetris->gameOverP = 0;

	if (tetris->gameOverP < HEIGHT - 1 && tetris->gameOverCnt % 5 == 0)
	{
		for (int j = 1; j < WIDTH - 1; j++)
		{
			if (tetris->gameOrg[tetris->gameOverP][j] > 10)
			{
				tetris->gameOrg[tetris->gameOverP][j] = 16;
			}
		}
		tetris->gameOverP++;
	}
}

void Tetris_set_gameDelay(Tetris* tetris, int cnt)
{
	tetris->gameDelay_on = true;
	tetris->gameDelayCnt = cnt;
}

void Tetris_cnt_gameDelay(Tetris* tetris)
{
	if (tetris->gameDelayCnt > 0)
		tetris->gameDelayCnt--;
	else if (tetris->gameDelayCnt == 0)
		tetris->gameDelay_on = false;
}

void Tetris_set_gameMsg(Tetris* tetris, int x, int y, int type, int val)
{
	tetris->gameMsgCnt = 10;

	gotoxy(tetris->game_x + x, tetris->game_y + y);
	switch (type)
	{
	case 0:		
		printf("%d COMBO!", val);
		break;
	case 1:
		printf("SPEED UP!!");
		break;
	case 2:
		printf("SPEED DOWN!!");
		break;
	}
}

void Tetris_cnt_gameMsg(Tetris* tetris)
{
	if (tetris->gameMsgCnt > 0)
		tetris->gameMsgCnt--;
	else if (tetris->gameMsgCnt == 0)
	{
		tetris->resetGameCpy(tetris);
		tetris->gameMsgCnt = -1;
	}
}

void Tetris_getKey(Tetris* tetris, Blocks* block)
{
	tetris->dropBlock(tetris);
	if (tetris->keyCnt > 0)
	{
		tetris->keyCnt--;
		return;
	}
	tetris->keyCnt = 3;

	if (((tetris->owner == SINGLE || tetris->owner == PLAYER2) && GetAsyncKeyState(VK_LEFT)) || (tetris->owner == PLAYER1 && GetAsyncKeyState('A')))
		if (tetris->checkCrush(tetris, block->x - 1, block->y, block->rotation) == true)
			tetris->move_block(tetris, block, LEFT);

	if (((tetris->owner == SINGLE || tetris->owner == PLAYER2) && GetAsyncKeyState(VK_RIGHT)) || (tetris->owner == PLAYER1 && GetAsyncKeyState('D')))
		if (tetris->checkCrush(tetris, block->x + 1, block->y, block->rotation) == true)
			tetris->move_block(tetris, block, RIGHT);

	if (((tetris->owner == SINGLE || tetris->owner == PLAYER2) && GetAsyncKeyState(VK_DOWN)) || (tetris->owner == PLAYER1 && GetAsyncKeyState('S')))
		if (tetris->checkCrush(tetris, block->x, block->y + 1, block->rotation) == true)
			tetris->move_block(tetris, block, DOWN);

	if (((tetris->owner == SINGLE || tetris->owner == PLAYER2) && GetAsyncKeyState(VK_UP)) || (tetris->owner == PLAYER1 && GetAsyncKeyState('W')))
		if (tetris->checkCrush(tetris, block->x, block->y, (block->rotation + 1) % 4) == true)
			tetris->move_block(tetris, block, UP);
		else if (tetris->checkCrush(tetris, block->x, block->y - 1, (block->rotation + 1) % 4) == true)
			tetris->move_block(tetris, block, 100);

	if ((tetris->owner == SINGLE && (GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState('L'))) 
		|| (tetris->owner == PLAYER2 && GetAsyncKeyState('L')) 
		|| (tetris->owner == PLAYER1 && GetAsyncKeyState('G')))
	{
		while (tetris->crush_on == false)
		{
			tetris->dropBlock(tetris);
			tetris->score += tetris->level;
		}
		tetris->fCnt = 0;
	}
}

void Tetris_checkLine(Tetris* tetris, Blocks* block)
{
	int block_amount;
	int combo = 0;

	for (int i = HEIGHT - 2; i > 3;)
	{
		block_amount = 0;
		for (int j = 1; j < WIDTH - 1; j++)
		{
			if (tetris->gameOrg[i][j] > 0)
				block_amount++;
		}


		if (block_amount == WIDTH - 2)
		{
			tetris->score += 100 * tetris->level;
			tetris->lineCnt++;
			combo++;

			if (tetris->pushAttackRegP > 0)
			{
				for (int m = 1; m < WIDTH - 1; m++)
				{
					if (m > block->x && m < block->x + 4 && block->shape[block->type][block->rotation][i - block->y][m - block->x] == 1)
						tetris->pushAttackReg[tetris->pushAttackRegP][m] = 0;
					else
						tetris->pushAttackReg[tetris->pushAttackRegP][m] = 16;
				}
				tetris->pushAttackRegP--;
				block->y++;
			}

			for (int k = i; k > 1; k--)
			{
				for (int l = 1; l < WIDTH - 1; l++)
				{
					if (tetris->gameOrg[k - 1][l] != CEILLING)
						tetris->gameOrg[k][l] = tetris->gameOrg[k - 1][l];
					if (tetris->gameOrg[k - 1][l] == CEILLING)
						tetris->gameOrg[k][l] = EMPTY;
				}
			}
		}
		else
			i--;
	}

	if (combo)
	{
		if (combo > 1)
		{
			tetris->drawGame(tetris);
			tetris->set_gameMsg(tetris, (WIDTH / 2) - 1, block->y - 2, 0, combo);
			tetris->score += (combo * tetris->level * 100);
		}
		else
		{
			if (tetris->pushAttackRegP > 0)
			{
				tetris->pushAttackRegP++;
				for (int m = 1; m < WIDTH - 1; m++)
				{
					tetris->pushAttackReg[tetris->pushAttackRegP][m] = -1;
				}
			}
		}

		if (tetris->lineCnt > 5 && tetris->level < 9)
		{
			tetris->level++;
			tetris->lineCnt = 0;

			if (tetris->speed[tetris->level] < tetris->speed[tetris->level - 1])
				tetris->set_gameMsg(tetris, (WIDTH / 2) - 1, 5, 1, 0);
			else
				tetris->set_gameMsg(tetris, (WIDTH / 2) - 1, 5, 2, 0);
		
			gotoxy(tetris->status_x, tetris->status_y + 6);
			printf("Speed:%2d", tetris->speed[tetris->level]);
		}
	}

	for (int j = 1; j < WIDTH - 2; j++)
		if (tetris->gameOrg[3][j] > 0)
			tetris->gameOver_on = true;
}

void Tetris_getAttack(Tetris* tetris)
{
	if (tetris->getAttackRegP < HEIGHT - 1)
	{
		int line = (HEIGHT - 1) - (tetris->getAttackRegP);

		if (tetris->getAttackRegP < HEIGHT - 1)
		{
			for (int i = 4; i < HEIGHT - 1; i++)	// 천장부터 바닥 전까지
			{
				for (int j = 1; j < WIDTH - 1; j++)
				{
					if (i - line > 0)
						tetris->gameOrg[i - line][j] = tetris->gameOrg[i][j];
					tetris->gameOrg[i][j] = EMPTY;
				}
			}
		}

		for (int i = tetris->getAttackRegP + 1; i < HEIGHT; i++)
		{
			for (int j = 1; j < WIDTH - 1; j++)
			{
				tetris->gameOrg[i - 1][j] = tetris->getAttackReg[i][j];
				tetris->getAttackReg[i][j] = -1;
			}
		}

		tetris->getAttackRegP += line;
		for (int i = 1; i < 10; i++)
		{
			gotoxy(tetris->attackQueue_x, tetris->game_y + i);
			printf("  ");
		}
	}
}


void BTM_init(BattleTetrisManager* btm);
void BTM_resetManager(BattleTetrisManager* btm);
void BTM_gamePlay(Tetris* A);
void BTM_getKey(BattleTetrisManager* btm);
void BTM_pushAttack(Tetris* A, Tetris* B);
void BTM_checkWinner(BattleTetrisManager* btm, Tetris* A, Tetris* B);

typedef struct BTM {
	void (*init)(BattleTetrisManager* btm) = BTM_init;
	Tetris *p1;
	Tetris *p2;

	void (*resetManager)(BattleTetrisManager* btm) = BTM_resetManager;

	void (*gamePlay)(Tetris* A) = BTM_gamePlay;
	void (*getKey)(BattleTetrisManager* btm) = BTM_getKey;
	void (*pushAttack)(Tetris* A, Tetris* B) = BTM_pushAttack;
	void (*checkWinner)(BattleTetrisManager* btm, Tetris* A, Tetris* B) = BTM_checkWinner;
		bool winner_on;

}BattleTetrisManager;

void BTM_init(BattleTetrisManager* btm)
{
	btm->p1 = NULL;
	btm->p2 = NULL;
}

void BTM_resetManager(BattleTetrisManager* btm)
{
	system("cls");
	btm->winner_on = false;

	btm->p1->block->init(btm->p1->block);
	btm->p1->init(btm->p1, 2, 1, 14, 2, PLAYER1);

	btm->p2->block->init(btm->p2->block);
	btm->p2->init(btm->p2, 27, 1, 22, 2, PLAYER2);

	gotoxy(31, 26);
	printf("Game Exit: ESC");

	GetAsyncKeyState(VK_LEFT);
	GetAsyncKeyState(VK_RIGHT);
	GetAsyncKeyState(VK_UP);
	GetAsyncKeyState(VK_DOWN);
	GetAsyncKeyState(VK_SPACE);
	GetAsyncKeyState('L');

	GetAsyncKeyState('A');
	GetAsyncKeyState('D');
	GetAsyncKeyState('S');
	GetAsyncKeyState('W');
	GetAsyncKeyState('G');
}

void BTM_gamePlay(Tetris* A)
{
	if (A->gameMsgCnt >= 0)
		A->cnt_gameMsg(A);

	if (A->gameDelay_on == true)
	{
		A->cnt_gameDelay(A);
	}
	else
	{
		if (A->gameOver_on == false)
		{
			A->getKey(A, A->block);
		}
		else
		{
			A->gameOver(A);
		}

		A->drawGame(A);
	}
}

void BTM_getKey(BattleTetrisManager* btm)
{
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		gotoxy(0, 24);
		printf("Thanks for playing :)");
		exit(0);
	}
}

void BTM_pushAttack(Tetris* A, Tetris* B)
{
	if (A->pushAttackRegP < HEIGHT - 1)
	{
		int line = (HEIGHT - 1) - (A->pushAttackRegP);

		for (int i = B->getAttackRegP + 1; i < HEIGHT; i++)
		{
			for (int j = 0; j < WIDTH - 1; j++)
			{
				if (i - line > 0)
					B->getAttackReg[i - line][j] = B->getAttackReg[i][j];
			}
		}

		for (int i = A->pushAttackRegP + 1; i < HEIGHT; i++)
		{
			for (int j = 0; j < WIDTH - 1; j++)
			{
				B->getAttackReg[i][j] = A->pushAttackReg[i][j];
				A->pushAttackReg[i][j] = -1;
			}
		}
		B->getAttackRegP -= line;
		A->pushAttackRegP += line;

		int queue = HEIGHT - 1 - B->getAttackRegP;
		int queueP = 0;

		while (queue >= 5) 
		{
			gotoxy(B->attackQueue_x, B->game_y + 1 + queueP);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x000C);
			printf("★");
			queue -= 5;
			queueP++;
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);
		
		while (queue >= 1) { //한줄은 원 한개
			gotoxy(B->attackQueue_x, B->game_y + 1 + queueP);
			printf("○");
			queue--;
			queueP++;
		}
		
		while (queueP < 10) { //최대 10칸까지 표시 나머지칸들은 빈칸으로 표시
			gotoxy(B->attackQueue_x, B->game_y + 1 + queueP);
			printf("  ");
			queueP++;
		}
	}
}

void BTM_checkWinner(BattleTetrisManager* btm, Tetris* A, Tetris* B)
{
	if (btm->winner_on == true)
	{
		if(GetAsyncKeyState(VK_RETURN))
			btm->resetManager(btm);
	}

	int whoWin;
	if (btm->winner_on == false && A->gameOver_on == true)
	{
		btm->winner_on = true;
		whoWin = B->owner;
	}
	else if (btm->winner_on == false && B->gameOver_on == true)
	{
		btm->winner_on = true;
		whoWin = A->owner;
	}
	else return;

	gotoxy(15, 10);
	switch (whoWin)
	{
	case 0:
		printf(" <<  PLAYER WIN  >>");
		break;
	case 1:
		printf(" << PLAYER 1 WIN >>");
		break;
	case 2:
		printf(" << PLAYER 2 WIN >>");
		break;
	case 3:
		printf(" << COMPUTER WIN >>");
		break;
	}
	gotoxy(14, 12);
	printf("Press <ENTER> to restart");
}


void titleMenu(void)
{
	int text_battle_x = 3;
	int text_battle_y = 2;
	
	int text_tetris_x = 16;
	int text_tetris_y = 9;

	int text_pressKey_x = 20;
	int text_pressKey_y = 15;
	
	int text_battle_order[9] = { 0,1,2,3,4,2,1,4,2 };
	int text_battle[5][6][25] = {
		9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
		9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
		9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
		9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
		9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
		9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,

		1,1,0,0,0,1,0,0,1,1,1,0,1,1,1,0,1,0,0,0,1,1,1,0,0,
		1,0,1,0,1,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,
		1,1,0,0,1,1,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,1,1,0,0,
		1,0,1,0,1,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,
		1,1,0,0,1,0,1,0,0,1,0,0,0,1,0,0,1,1,1,0,1,1,1,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,1,1,0,0,0,1,0,0,1,1,1,0,1,1,1,0,1,0,0,0,1,1,1,0,
		0,1,0,1,0,1,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,0,
		0,1,1,0,0,1,1,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,1,1,0,
		0,1,0,1,0,1,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,0,
		0,1,1,0,0,1,0,1,0,0,1,0,0,0,1,0,0,1,1,1,0,1,1,1,0,

		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,1,0,0,0,1,0,0,1,1,1,0,1,1,1,0,1,0,0,0,1,1,1,0,0,
		1,0,1,0,1,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,
		1,1,0,0,1,1,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,1,1,0,0,
		1,0,1,0,1,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,
		1,1,0,0,1,0,1,0,0,1,0,0,0,1,0,0,1,1,1,0,1,1,1,0,0,

		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,1,1,0,0,0,1,0,0,1,1,1,0,1,1,1,0,1,0,0,0,1,1,1,
		0,0,1,0,1,0,1,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,
		0,0,1,1,0,0,1,1,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,1,1,
		0,0,1,0,1,0,1,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,
		0,0,1,1,0,0,1,0,1,0,0,1,0,0,0,1,0,0,1,1,1,0,1,1,1
	};

	int text_tetris[5][21] = {
		1,1,1,0,1,1,1,0,1,1,1,0,1,1,0,0,1,0,1,1,1,
		0,1,0,0,1,0,0,0,0,1,0,0,1,0,1,0,1,0,1,0,0,
		0,1,0,0,1,1,1,0,0,1,0,0,1,1,0,0,1,0,1,1,1,
		0,1,0,0,1,0,0,0,0,1,0,0,1,0,1,0,1,0,0,0,1,
		0,1,0,0,1,1,1,0,0,1,0,0,1,0,1,0,1,0,1,1,1
	};

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0003);
	for (int k = 1; k < 9; k++) 
	{
		for (int i = 0; i < 6; i++) 
		{
			for (int j = 0; j < 25; j++) 
			{
				if (text_battle[text_battle_order[k]][i][j] != text_battle[text_battle_order[k - 1]][i][j]) 
				{
					gotoxy(text_battle_x + j, text_battle_y + i);

					if (text_battle[text_battle_order[k]][i][j] == 1) 
						printf("■");

					else if (text_battle[text_battle_order[k]][i][j] == 0) 
						printf("  ");
				}
			}
		}
		Sleep(100);
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);
	Sleep(200);

	gotoxy(2, 16); printf("┌─── <  Key Instructions  >───┐");
	gotoxy(2, 17); printf("│ PLAYER 1            PLAYER 2│");
	gotoxy(2, 18); printf("│    T         UP        ↑   │");
	gotoxy(2, 19); printf("│    H        RIGHT      →   │");
	gotoxy(2, 20); printf("│    F        LEFT       ←   │");
	gotoxy(2, 21); printf("│    G      SOFT DROP    ↓   │");
	gotoxy(2, 22); printf("│    Q      HARD DROP     L   │");
	gotoxy(2, 23); printf("└─────────────────────────────┘");


	for (int cnt = 0;; cnt++) 
	{
		Sleep(10);

		if ((cnt + 0) % 60 == 0) 
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x000F);
			gotoxy(text_pressKey_x, text_pressKey_y); 
			printf(" < Press Enter Key to Start >");
		}

		if ((cnt + 30) % 60 == 0) 
		{
			gotoxy(text_pressKey_x, text_pressKey_y);
			printf("                             ");
		}

		if (cnt % 75 == 0) 
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cnt % 7 + 2);
			for (int i = 0; i < 5; i++) 
			{
				for (int j = 0; j < 21; j++) 
				{
					gotoxy(text_tetris_x + j, text_tetris_y + i);
					if (text_tetris[i][j] == 1) 
						printf("▣");
				}
			}
		}

		if (GetAsyncKeyState(VK_RETURN)) 
			break;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);
}


int main(void)
{
	srand((unsigned)time(NULL));
	setcursortype(NOCURSOR);
	titleMenu();


	BattleTetrisManager GM;
	GM.init(&GM);

	Tetris player1;
	Tetris player2;
	GM.p1 = &player1;
	GM.p2 = &player2;

	Blocks p1_block, p2_block;
	GM.p1->block = &p1_block;
	GM.p2->block = &p2_block;

	GM.resetManager(&GM);

	while (1)
	{
		Sleep(20);
		GM.getKey(&GM);

		GM.gamePlay(GM.p1);
		GM.gamePlay(GM.p2);

		GM.pushAttack(GM.p1, GM.p2);
		GM.pushAttack(GM.p2, GM.p1);

		GM.checkWinner(&GM, GM.p1, GM.p2);
	}

	return 0;
}