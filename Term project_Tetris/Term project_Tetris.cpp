#include <stdio.h>
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

#define ACTIVE_BLOCK ((block.type+11)*(-1))
#define CEILLING -1
#define EMPTY 0
#define WALL 1
#define INACTIVE_BLOCK (block.type+11)

typedef struct BLOCKS Blocks;
typedef struct TETRIS Tetris;


void gotoxy(int x, int y) {
	COORD pos = { 2 * x, y };
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

	const int shape[7][4][4][4] = { // 블럭의 모양을 저장하는 배열
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

	static const int WIDTH = 11;
	static const int HEIGHT = 23;

	int gameOrg[HEIGHT][WIDTH];
	int gameCpy[HEIGHT][WIDTH];
		void (*resetGameCpy)(Tetris* tetris) = Tetris_resetGameCpy;

	void (*drawGame)(Tetris* tetris) = Tetris_drawGame;
	void (*newBlock)(void);
		Blocks block;
		void (*move_block)(int dir);
		void (*dropBlock)(void);
		bool (*checkCrush)(int x, int y, int rotation);
			bool crush_on;

	void (*initialGame)(void);

	void (*gameOver)(void);
		bool gameOver_on;
		int gameOverCnt;
		int gameOverP;

	void (*gameDelay)(int cnt);
		bool gameDelay_on;
		int gamedelayCnt;
		void (*gameDelay_off)(void);

	void (*gameMsg)(int x, int y, int type, int val);
		int gameMsgCnt;
		void (*gameMsg_erase)(void);

	void (*getKey)(void);
	int keyCnt;

	void (*checkLine)(void);
		int pushAttackReg[HEIGHT][WIDTH];
		int pushAttackRegp;

	void (*getAttack)(void);
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

	for (int i = 0; i < tetris->HEIGHT; i++)
	{
		for (int j = 0; j < tetris->WIDTH; j++)
		{
			tetris->gameOrg[i][j] = 0;
			tetris->gameCpy[i][j] = NULL;
			tetris->pushAttackReg[i][j] = -1;
			tetris->getAttackReg[i][j] = -1;
		}
	}
	tetris->pushAttackRegp = tetris->HEIGHT - 1;
	tetris->getAttackRegP = tetris->HEIGHT - 1;

	tetris->game_x = game_x;
	tetris->game_y = game_y;
	tetris->status_x = status_x;
	tetris->status_y = status_y;
	tetris->owner = owner;

	tetris->crush_on = false;
	tetris->gameDelay_on = false;
	tetris->gamedelayCnt = -1;
	tetris->gameOver_on = false;
	tetris->gameOverCnt = -1;
	tetris->gameMsgCnt = -1;

	tetris->initialGame();
	tetris->newBlock();
}

void Tetris_resetGameCpy(Tetris* tetris)
{
	for (int i = 0; i < tetris->HEIGHT; i++)
	{
		for (int j = 0; j < tetris->WIDTH; j++)
			tetris->gameCpy[i][j] = 999;
	}
}

void Tetris_drawGame(Tetris* tetris)
{
	for (int j = 0; j < tetris->WIDTH - 1; j++)
	{
		if (tetris->gameOrg[3][j] == EMPTY)
			tetris->gameOrg[3][j] = CEILLING;
	}

	for (int i = 0; i < tetris->HEIGHT; i++)
	{
		for (int j = 0; j < tetris->WIDTH; j++)
		{
			if (tetris->gameCpy[i][j] != tetris->gameOrg[i][j])
			{
				gotoxy(tetris->game_x + j, tetris->game_y + i);
				if (tetris->gameOrg[i][j] == EMPTY)
					printf("  ");
				else if (tetris->gameOrg[i][j] == CEILLING)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);
					printf(", ");
				}
				else if (tetris->gameOrg[i][j] == WALL)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);
					printf("▩");
				}
				else if (tetris->gameOrg[i][j] < 0)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), tetris->block.getColor(tetris->gameOrg[i][j]));
					printf("▣");
				}
				else if (tetris->gameOrg[i][j] > 0)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), tetris->block.getColor(tetris->gameOrg[i][j]));
					printf("■");
				}
			}
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);

	for (int i = 0; i < tetris->HEIGHT; i++)
	{
		for (int j = 0; j < tetris->WIDTH; j++)
		{
			tetris->gameCpy[i][j] = tetris->gameOrg[i][j];
		}
	}
}


int main(void)
{

	return 0;
}