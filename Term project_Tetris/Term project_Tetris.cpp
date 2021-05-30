#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>

// 키보드 입력값
#define LEFT 75	
#define RIGHT 77
#define UP 72
#define DOWN 80	// SOFT DROP

// 플레이어 구분
#define PLAYER1 1
#define PLAYER2 2

// 블럭의 상태 구분
#define ACTIVE_BLOCK ((tetris->block->type + 11) * (-1))	// 움직이고 있는 블럭은 음수로 저장
#define INACTIVE_BLOCK (tetris->block->type + 11)	// 멈춰있는 블럭은 양수로 저장
#define CEILLING -1
#define EMPTY 0
#define WALL 1

// 블럭의 종류 7개
#define BLOCK_TYPE 7
// 블럭의 회전 상태 4개
#define BLOCK_ROTATION 4
// 블럭의 사이즈 4x4
#define BLOCK_SIZE 4

// 게임판 크기
#define WIDTH 11
#define HEIGHT 23


// 구조체 원형 정의 (*구조체 포인터를 매개변수로 가지는 구조체 멤버를 위함)
typedef struct BLOCKS Blocks;
typedef struct TETRIS Tetris;
typedef struct BTM BattleTetrisManager;


// 커서 위치를 변경하는 함수
void gotoxy(int x, int y) {
	COORD pos = { 2 * (short)x, (short)y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// 열거형 정의
typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCULSOR } CURSOR_TYPE;

// 커서의 타입을 설정하는 함수 (*여기서는 커서를 숨기는 NOCURSOR만 사용)
void setcursortype(CURSOR_TYPE c) {
	CONSOLE_CURSOR_INFO CurInfo = { NULL };	// 커서의 구성 정보를 저장하는 구조체

	switch (c) {
	case NOCURSOR:
		CurInfo.dwSize = 1;			// 굵기
		CurInfo.bVisible = FALSE;	// true = 커서 보임, false = 커서 숨김
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

	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);	// 구조체의 정보를 콘솔의 커서에 적용
}


// 함수 원형 선언 (*아래의 함수의 주소를 저장하는 구조체(Blocks)의 멤버(함수 포인터)를 위함)
// 함수 포인터를 사용한 이유는 C언어의 구조체를 C++의 클래스처럼 사용하기 위함
void Blocks_init(Blocks* blocks);
int Blocks_getColor(int num);

typedef struct BLOCKS
{
	void(*init)(Blocks* blocks) = Blocks_init;	// 구조체(Blocks)를 초기화하는 함수 (C++의 생성자)

	const int shape[BLOCK_TYPE][BLOCK_ROTATION][BLOCK_SIZE][BLOCK_SIZE] = // 블럭의 모양을 저장하는 배열
	{ 
		{{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0}},
		{{0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0},{0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0}},
		{{0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0},{0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0}},
		{{0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},{0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0},{0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},{0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0}},
		{{0,0,0,0,0,0,1,0,1,1,1,0,0,0,0,0},{0,0,0,0,1,1,0,0,0,1,0,0,0,1,0,0},{0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0},{0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,0}},
		{{0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,1,0,0,1,1,0,0},{0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,0},{0,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0}},
		{{0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,1,1,0,0,1,0,0},{0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0},{0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0}}
	};

	int x;			// 블럭의 x 좌표 저장
	int y;			// 블럭의 y 좌표 저장
	int rotation;	// 블럭의 회전 저장

	int type;		// 블럭의 type 저장
	int type_next;	// 다음 블럭의 type 저장 (status에 다음 블럭 정보를 표시하기 위함)

	int (*getColor)(int num) = Blocks_getColor;	// 블럭의 색을 반환하는 함수

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


// 함수 원형 선언 (*아래의 함수의 주소를 저장하는 구조체(Tetris)의 멤버(함수 포인터)를 위함)
// 함수 포인터를 사용한 이유는 C언어의 구조체를 C++의 클래스처럼 사용하기 위함
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
	// 구조체(Tetris)를 초기화하는 함수 (C++의 생성자)
	void (*init)(Tetris* tetris, int game_x, int game_y, int status_x, int status_y, int owner) = Tetris_init;	


	int fCnt;		// 프레임 카운터 (0이 되면 블럭이 한칸 떨어짐) 
					// speed[level] 값으로 초기화 되며 작을 수록 게임 진행 속도가 빨라짐
	
	int owner;		// 플레이어 구분
	int score;		// 점수 저장
	int lineCnt;	// 제거한 줄 수 저장


	const int speed[11] = { 20, 20, 15, 10, 20, 10, 5, 15, 10, 2, 0 };	// 게임 속도 저장
		int level;	// 게임의 레벨 저장
					// speed[level] 형식으로 사용하여 레벨이 올라 갈수록 게임 진행 속도가 빨라짐


	// 플레이어 1과 플레이어 2의 좌표가 다르기 때문에 변수로 각자 저장
	// 게임판의 좌표 저장
	int game_x;		
	int game_y;
	// 상태창의 좌표 저장
	int status_x;
	int status_y;


	// 게임 전체가 깜박이는 것을 방지하기 위해 gameOrg와 gameCpy를 비교해 변화가 있는 부분만 새로 그림
	int gameOrg[HEIGHT][WIDTH];	// 게임판을 저장하는 배열
	int gameCpy[HEIGHT][WIDTH];	// 게임판의 카피를 저장하는 배열 (gameOrg가 변화하기 바로 전 게임판을 저장하고 있음)
		// gameCpy를 리셋하는 함수
		void (*resetGameCpy)(Tetris* tetris) = Tetris_resetGameCpy;	


	// gameOrg와 gameCpy를 비교해 변화가 있는 부분만 그리는 함수
	void (*drawGame)(Tetris* tetris) = Tetris_drawGame;	
	

	// 새로운 블록을 하나 만드는 함수
	void (*newBlock)(Tetris* tetris, Blocks* block) = Tetris_newBlock;	
		
		// block 객체의 주소를 저장(해당 플레이어가 움직이는 블럭의 정보가 담긴다)
		Blocks* block;	
		
		// 플레이어의 입력에 따라 블럭을 이동시키는 함수
		void (*move_block)(Tetris* tetris, Blocks* block, int dir) = Tetris_move_block;	
		
		// fCnt가 0이 될 때마다 블록을 한칸 떨어트리는 함수
		void (*dropBlock)(Tetris* tetris) = Tetris_dropBlock;	
		
		// 블럭이 이동 가능한지 판별하는 함수 (충돌하면 false 반환, 이동 가능하면 true 반환)
		bool (*checkCrush)(Tetris* tetris, int x, int y, int rotation) = Tetris_checkCrush;	
			
			// 충돌 상태 저장 (충돌 중이면 true, 아니면 false)
			bool crush_on;	


	// gameOrg(게임판)을 초기화 하는 함수
	void (*initialGame)(Tetris* tetris) = Tetris_initialGame;


	// 게임 오버를 진행하는 함수
	void (*gameOver)(Tetris* tetris) = Tetris_gameOver;
		bool gameOver_on;	// 게임 오버 상태를 저장
		int gameOverCnt;	// 게임 오버 애니메이션 속도 조절을 위해 Cnt 저장
		int gameOverP;		// 게임 오버 애니메이션 속도 조절을 위해 변화시킨 줄 수 저장


	// 게임 속도 조절을 위해 게임 딜레이를 세팅하는 함수
	void (*set_gameDelay)(Tetris* tetris, int cnt) = Tetris_set_gameDelay;
		bool gameDelay_on;	// 게임 딜레이이가 켜져있는지 저장 (켜져있으면 true, 꺼져있으면 false)
		int gameDelayCnt;	// 게임 딜레이 Cnt 저장
		
		// 게임 딜레이의 Cnt를 감소 시키고 0이 되면 끄는 함수 
		void (*cnt_gameDelay)(Tetris* tetris) = Tetris_cnt_gameDelay;	


	// 게임판에 메시지를 세팅하는 함수
	void (*set_gameMsg)(Tetris* tetris, int x, int y, int type, int val) = Tetris_set_gameMsg;
		int gameMsgCnt;	// 게임 메시지 Cnt (게임 메시지가 떠있는 시간)

		// 게임 메시지의 Cnt를 감소 시키고 0이 되면 끄는 함수
		void (*cnt_gameMsg)(Tetris* tetris) = Tetris_cnt_gameMsg;


	// keyCnt가 0일 때 키를 입력 받는 함수
	void (*getKey)(Tetris* tetris, Blocks* block) = Tetris_getKey;
		int keyCnt;	// 키 입력 속도 조절을 위해 Cnt 저장 (조금만 누르고 있어도 많이 입력되는 것을 방지)


	// 한 줄이 완성되었는지 확인하고, 완성되었으면 줄을 지우는 함수
	void (*checkLine)(Tetris* tetris, Blocks* block) = Tetris_checkLine;
		int pushAttackReg[HEIGHT][WIDTH];	// 콤보 시 지운 줄을 저장 (밑에서 부터 저장 됨)
		/* 
			이 배열이 채워져 있으면 BattleTetirsManager가
			상대의 getAttackReg로 저장된 줄을 이동시킨다. 
		*/
		int pushAttackRegP;	// pushAttackReg의 높이 저장 (밑에서 부터 저장하므로)


	// getAttackReg를 확인해 공격 받을 줄이 있으면 공격 받음 (밑에 줄이 올라옴)
	void (*getAttack)(Tetris* tetris) = Tetris_getAttack;
		int getAttackReg[HEIGHT][WIDTH];	// 상대방으로 부터 받는 줄을 임시 저장 (밑에서 부터 저장)
		int getAttackRegP;	// getAttackReg의 높이 저장 (밑에서 부터 저장하므로)
		int attackQueue_x;	// 몇 줄 공격 받는지 화면에 나타내는 x 좌표 저장

}Tetris;

void Tetris_init(Tetris* tetris, int game_x, int game_y, int status_x, int status_y, int owner)
{
	// 게임 기본 변수 초기화
	tetris->level = 1;
	tetris->score = 0;
	tetris->lineCnt = 0;
	tetris->keyCnt = 0;
	tetris->fCnt = tetris->speed[tetris->level];

	// 게임판과 공격 관련 배열 초기화
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

	// 공격 관련 변수 초기화
	tetris->pushAttackRegP = HEIGHT - 1;
	tetris->getAttackRegP = HEIGHT - 1;

	// 게임판 좌표, 상태창 좌표, 플레이어 변수 초기화
	tetris->game_x = game_x;
	tetris->game_y = game_y;
	tetris->status_x = status_x;
	tetris->status_y = status_y;
	tetris->owner = owner;

	// 충돌과 딜레이, 메시지 변수 초기화
	tetris->crush_on = false;
	tetris->gameDelay_on = false;
	tetris->gameDelayCnt = -1;
	tetris->gameOver_on = false;
	tetris->gameOverCnt = -1;
	tetris->gameOverP = 0;
	tetris->gameMsgCnt = -1;

	// 게임판에 천장과 벽 생성
	tetris->initialGame(tetris);
	// 첫 번째 블록 생성
	tetris->newBlock(tetris, tetris->block);
}

void Tetris_resetGameCpy(Tetris* tetris)
{
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)	// gameCpy 배열 초기화
			tetris->gameCpy[i][j] = 999;
	}
}

void Tetris_drawGame(Tetris* tetris)
{
	for (int j = 0; j < WIDTH - 1; j++)	// 블럭이 지나가서 천장이 지워지면 천장을 그림
	{
		if (tetris->gameOrg[3][j] == EMPTY)
			tetris->gameOrg[3][j] = CEILLING;
	}

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			// gameCpy와 gameOrg의 값이 다른 곳만 그림
			if (tetris->gameCpy[i][j] != tetris->gameOrg[i][j])	
			{
				gotoxy(tetris->game_x + j, tetris->game_y + i);
				if (tetris->gameOrg[i][j] == EMPTY)
				{
					printf("  ");
				}
				else if (tetris->gameOrg[i][j] == CEILLING)	// 천장은 ". "으로 표시
				{
					// 하얀색 선택
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);
					printf(". ");
				}
				else if (tetris->gameOrg[i][j] == WALL)	// 벽은 "▩"으로 표시
				{
					// 하얀색 선택
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);
					printf("▩");
				}
				else if (tetris->gameOrg[i][j] < 0)	// 움직이는(ACTIVE) 블럭은 "▣"으로 표시
				{
					// 블럭 type에 따라서 색 선택
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), tetris->block->getColor(tetris->gameOrg[i][j]));
					printf("▣");
				}
				else if (tetris->gameOrg[i][j] > 0)	// 멈춘(INACTIVE) 블럭은 "■"으로 표시
				{
					// 블럭 type에 따라서 색 선택
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), tetris->block->getColor(tetris->gameOrg[i][j]));
					printf("■");
				}
			}
		}
	}
	// 랜덤으로 색 선택했을 경우를 위해 다시 하얀색 선택
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{	
			// 다 그리고 난 후 gameCpy는 바로 전 화면을 저장해야 하므로 gameOrg의 값을 저장
			tetris->gameCpy[i][j] = tetris->gameOrg[i][j];
		}
	}

	// 사용자에 따라 Score의 위치를 다르게 표시
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
	// 기존 블럭의 값을 기본값으로 초기화
	block->x = (WIDTH / 2) - 1;
	block->y = 0;
	block->type = block->type_next;
	block->type_next = rand() % 7;	// 랜덤으로 다음블럭 지정
	block->rotation = 0;

	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		for (int j = 0; j < BLOCK_SIZE; j++)
		{
			// shape 배열에 저장 된 블럭의 모양따라 ACTIVE_BLOCK 활성화
			if (block->shape[block->type][block->rotation][i][j] == 1)
				tetris->gameOrg[block->y + i][block->x + j] = ACTIVE_BLOCK;
		}
	}

	/*
	다음 블럭의 색이 type_next + 2인 이유는 ACTIVE 블록을 type + 11로 저장하고, 
	getColor에서 ACTIVE 값을 -9 해서 반환하기 때문이다.
	*/
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), block->type_next + 2);
	for (int i = 1; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)	
		{
			gotoxy(tetris->status_x + j, tetris->status_y + i + 2);
			
			// 상태창에 다음 블럭을 표시
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
			// 현재 위치하는 블럭을 gameOrg에서 지운다. (ACTIVE를 EMPTY로 바꾼다)
			if (block->shape[block->type][block->rotation][i][j] == 1)
				tetris->gameOrg[block->y + i][block->x + j] = EMPTY;
		}
	}

	switch (dir)	// LEFT, RIGHT, UP, DOWN 중 하나가 dir에 들어있다.
	{
	case LEFT:
		for (int i = 0; i < BLOCK_SIZE; i++)
		{
			for (int j = 0; j < BLOCK_SIZE; j++)
			{
				// 블럭의 모양을 gameOrg에 현재 블럭 좌표의 왼쪽으로 한 칸 옮겨서 그린다.
				if (block->shape[block->type][block->rotation][i][j] == 1)
					tetris->gameOrg[block->y + i][block->x + j - 1] = ACTIVE_BLOCK;
			}
		}
		block->x--;	// 블럭의 좌표 수정
		break;

	case RIGHT:
		for (int i = 0; i < BLOCK_SIZE; i++)
		{
			for (int j = 0; j < BLOCK_SIZE; j++)
			{
				// 블럭의 모양을 gameOrg에 현재 블럭 좌표의 오른쪽으로 한 칸 옮겨서 그린다.
				if (block->shape[block->type][block->rotation][i][j] == 1)
					tetris->gameOrg[block->y + i][block->x + j + 1] = ACTIVE_BLOCK;
			}
		}
		block->x++;	// 블럭의 좌표 수정
		break;

	case DOWN:
		for (int i = 0; i < BLOCK_SIZE; i++)
		{
			for (int j = 0; j < BLOCK_SIZE; j++)
			{
				// 블럭의 모양을 gameOrg에 현재 블럭 좌표의 아래쪽으로 한 칸 옮겨서 그린다.
				if (block->shape[block->type][block->rotation][i][j] == 1)
					tetris->gameOrg[block->y + i + 1][block->x + j] = ACTIVE_BLOCK;
			}
		}
		block->y++;	// 블럭의 좌표 수정
		break;

	case UP:	// 블럭의 회전
		// 블럭의 회전값을 하나 증가시킨다. (회전값: 블럭을 저장하는 배열의 rotation 인덱스)
		block->rotation = (block->rotation + 1) % 4;
		for (int i = 0; i < BLOCK_SIZE; i++)
		{
			for (int j = 0; j < BLOCK_SIZE; j++)
			{
				// gameOrg에 제자리에서 한 번 회전한 블럭을 그린다.
				if (block->shape[block->type][block->rotation][i][j] == 1)
					tetris->gameOrg[block->y + i][block->x + j] = ACTIVE_BLOCK;
			}
		}
		break;

	case 100:	// 블럭과 맞닿아 있을 때 회전할 경우 한칸 위로 상승해서 회전하는 동작
		// 블럭의 회전값을 하나 증가시킨다. (회전값: 블럭을 저장하는 배열의 rotation 인덱스)
		block->rotation = (block->rotation + 1) % 4;
		for (int i = 0; i < BLOCK_SIZE; i++)
		{
			for (int j = 0; j < BLOCK_SIZE; j++)
			{
				// 현재 블럭 좌표의 위쪽으로 한 칸 옮겨 gameOrg에 한 번 회전한 블럭을 그린다.
				if (block->shape[block->type][block->rotation][i][j] == 1)
					tetris->gameOrg[block->y + i - 1][block->x + j] = ACTIVE_BLOCK;
			}
		}
		block->y--;	// 블록의 좌표 수정
		break;
	}
}

void Tetris_dropBlock(Tetris* tetris)
{
	if (tetris->fCnt > 0)	// fCnt가 0 보다 클 경우 1 감소
		(tetris->fCnt)--;

	// 블럭을 한칸 내리는게 가능한지 저장
	// checkCrush: 블럭의 현재 모양만큼 충돌하는지 체크해서 반환하는 함수
	bool can_down = tetris->checkCrush(tetris, tetris->block->x, tetris->block->y + 1, tetris->block->rotation);

	// 충돌 중이였으나 좌/우로 움직여서 충돌이 아니게 된 경우
	if (tetris->crush_on == true && can_down == true)	
	{
		tetris->move_block(tetris, tetris->block, DOWN);	// 블록을 한 칸 낙하시킨다
		tetris->fCnt = tetris->speed[tetris->level];	// 현재 레벨의 speed만큼 fCnt 초기화
		tetris->crush_on = false;	// crush_on 변수 false
	}

	// 충돌 중이고 내려갈 수 없는 경우
	if (tetris->crush_on == true && can_down == false && (tetris->fCnt) == 0)
	{
		// gameDelay가 꺼져있는 경우
		if ((tetris->gameDelayCnt) == -1)	
		{
			tetris->drawGame(tetris);	// gameOrg와 gameCpy를 비교해서 바뀐 부분을 그린다.
			tetris->set_gameDelay(tetris, 5);	// 게임 Delay를 세팅해 블럭을 회전시키거나 좌우로 움직일 시간을 준다.
		}

		// gameDelay가 전부 카운트 된 경우
		else if (tetris->gameDelayCnt == 0)
		{
			for (int i = 0; i < HEIGHT; i++)
			{
				for (int j = 0; j < WIDTH; j++)
				{
					// 활성화 된 블록을 비활성화 블록으로 변경 
					if (tetris->gameOrg[i][j] == ACTIVE_BLOCK)
						tetris->gameOrg[i][j] = INACTIVE_BLOCK;
				}
			}
			tetris->gameDelayCnt = -1;	// 딜레이를 끈다.
			tetris->crush_on = false;	// 충돌이 끝났으므로 변수값 변경
			tetris->checkLine(tetris, tetris->block);	// 라인이 완성되었는지 체크
			tetris->getAttack(tetris);	// 공격받을 것이 있으면 공격받는다

			if (tetris->gameOver_on == false)	// 게임 오버가 되지 않았으면
				tetris->newBlock(tetris, tetris->block);	// 새로운 블럭을 생성한다.
			tetris->fCnt = tetris->speed[tetris->level];	// 현재 레벨의 speed만큼 fCnt 초기화
		}
		return;	// 함수 종료
	}

	// 충돌 중이지도 않고, 내려갈 수 있는 경우
	if (tetris->crush_on == false && can_down == true && tetris->fCnt == 0)
	{
		tetris->move_block(tetris, tetris->block, DOWN);	// 블록을 한 칸 낙하시킨다
		tetris->fCnt = tetris->speed[tetris->level];	// 현재 레벨의 speed만큼 fCnt 초기화
	}

	// 프레임 사이에 블럭의 옆으로 움직여 내려갈 수 없는 경우거나 어떤 이유로 내려갈 수 없는 경우
	if (tetris->crush_on == false && can_down == false)
	{
		tetris->crush_on = true;	// 충돌 중으로 변경
		tetris->fCnt = tetris->speed[1];	// 옆으로 끼어 들어갈 때 여유를 주기 위해 fCnt를 넉넉한 값으로 초기화
	}
}

bool Tetris_checkCrush(Tetris* tetris, int x, int y, int rotation)
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		for (int j = 0; j < BLOCK_SIZE; j++)
		{
			// 현재 블럭 모양이 x, y 좌표에 있는 블럭과 충돌이 나는지 판단 (충돌하면 false 반환)
			if (tetris->block->shape[tetris->block->type][rotation][i][j] == 1 && tetris->gameOrg[y + i][x + j] > 0)
				return false;
		}
	}
	// 반복문을 탈출하면 충돌하지 않는 것이므로 true 반환
	return true;
}

void Tetris_initialGame(Tetris* tetris)
{
	for (int j = 1; j < WIDTH; j++)
	{
		// 게임판의 위에서 4번째 칸을 천장으로 설정
		tetris->gameOrg[3][j] = CEILLING;
	}
	for (int i = 1; i < HEIGHT; i++)
	{
		// 게임판의 세로 벽 설정
		tetris->gameOrg[i][0] = WALL;
		tetris->gameOrg[i][WIDTH - 1] = WALL;
	}
	for (int j = 0; j < WIDTH; j++)
	{
		// 게임판의 가로 벽 설정
		tetris->gameOrg[HEIGHT - 1][j] = WALL;
	}

	// 커서를 상태창의 위치로 변경한 후 하얀색으로 변경
	gotoxy(tetris->status_x, tetris->status_y);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);

	switch (tetris->owner)	// 플레이어를 구별한 후 인터페이스 출력
	{
	case 1:	// 플레이어 1일 경우
		printf("<PLAYER1>");
		tetris->attackQueue_x = tetris->game_x - 1;	// 공격받는 표시를 그릴 x 좌표값 저장
		break;

	case 2: // 플레이어 2일 경우
		printf("<PLAYER2>");
		tetris->attackQueue_x = tetris->game_x - 1;	// 공격받는 표시를 그릴 x 좌표값 저장
		break;
	}

	// 각 사용자의 상태창에 인터페이스 출력
	gotoxy(tetris->status_x, tetris->status_y + 1);
	printf("Next: ");
	gotoxy(tetris->status_x, tetris->status_y + 6);
	printf("Speed:%2d", tetris->speed[tetris->level]);
}

void Tetris_gameOver(Tetris* tetris)
{
	// 애니메이션 속도 조절
	tetris->gameOverCnt++;

	// gameOverCnt가 5가 되고, 하얀색으로 변경한 줄 수가 게임판의 범위를 벗어나지 않았을 경우
	if (tetris->gameOverP < HEIGHT - 1 && tetris->gameOverCnt % 5 == 0)
	{
		for (int j = 1; j < WIDTH - 1; j++)
		{
			if (tetris->gameOrg[tetris->gameOverP][j] > 10)		// INACTIVE 블럭이면
			{
				tetris->gameOrg[tetris->gameOverP][j] = 16;		// 블럭을 하얀색으로 변경
			}
		}
		tetris->gameOverP++;	// 하얀색으로 변경한 줄 수를 1씩 증가
	}
}

void Tetris_set_gameDelay(Tetris* tetris, int cnt)
{
	tetris->gameDelay_on = true;	// 게임 딜레이를 켬 (true 저장)
	tetris->gameDelayCnt = cnt;		// 설정한 딜레이(cnt에 담긴 값)만큼 딜레이 Cnt 세팅(저장)
}

void Tetris_cnt_gameDelay(Tetris* tetris)
{
	if (tetris->gameDelayCnt > 0)	// 딜레이 Cnt가 0보다 크면 1씩 감소 (아직 딜레이가 종료될 떄가 아닌 경우)
		tetris->gameDelayCnt--;

	else if (tetris->gameDelayCnt == 0)	// 딜레이 Cnt가 0이 되면
		tetris->gameDelay_on = false;	// 게임 딜레이를 종료 (false 저장)
}

void Tetris_set_gameMsg(Tetris* tetris, int x, int y, int type, int val)
{
	tetris->gameMsgCnt = 10;	// 게임 메시지가 떠있을 시간

	// 메시지가 출력될 위치로 커서 이동
	gotoxy(tetris->game_x + x, tetris->game_y + y);	

	switch (type)	// 전달된 메시지 타입에 따라 메시지 출력
	{
	case 0:	// 콤보 성공 시
		printf("%d COMBO!", val);
		break;
	case 1:	// 게임 스피드가 증가한 경우 (블럭 떨어지는 속도)
		printf("SPEED UP!!");
		break;
	case 2:	// 게임 스피드가 감소한 경우 (블럭 떨어지는 속도)
		printf("SPEED DOWN!!");
		break;
	}
}

void Tetris_cnt_gameMsg(Tetris* tetris)
{
	if (tetris->gameMsgCnt > 0)	// 메시지 Cnt가 0보다 크면 1씩 감소 (아직 메시지가 사라질 때가 아닌 경우)
		tetris->gameMsgCnt--;

	else if (tetris->gameMsgCnt == 0)	// 메시지 Cnt가 0이 되면
	{
		tetris->resetGameCpy(tetris);	// gameCpy를 초기화 한 후 (메시지를 게임판 안에 그리므로 초기화 하지 않으면 지속적으로 메시지가 남고, 블럭 판정이 된다.)
		tetris->gameMsgCnt = -1;		// 메시지 Cnt를 -1로 만들어 종료시킨다.
	}
}

void Tetris_getKey(Tetris* tetris, Blocks* block)
{
	tetris->dropBlock(tetris);	// 블럭을 한 칸 낙하시킨 후 입력을 받는다. 
								// (키 입력이 블럭이 낙하나는 것보다 우선순위가 되지 않게 만든다. 즉, INACTIVE 블럭과 닿았을 때 계속 키입력이 됨을 방지)
	
	if (tetris->keyCnt > 0)	// keyCnt가 0이 되지 않으면 입력을 받지 않는다. (연속적으로 너무 빠르게 입력 되는 것을 방지)
	{
		tetris->keyCnt--;
		return;
	}
	tetris->keyCnt = 3;	// keyCnt 초기화 (3프레임마다 입력을 받는다)
	 
	// 어떤 키를 입력 했는지 체크
	if ((tetris->owner == PLAYER1 && GetAsyncKeyState('A')) || ((tetris->owner == PLAYER2) && GetAsyncKeyState(VK_LEFT)))
	{ 
		// 블럭이 왼쪽으로 한 칸 이동 가능하면
		if (tetris->checkCrush(tetris, block->x - 1, block->y, block->rotation) == true)
			tetris->move_block(tetris, block, LEFT);	// 블럭을 왼쪽으로 한 칸 이동
	}
	
	// 어떤 키를 입력 했는지 체크
	if ((tetris->owner == PLAYER1 && GetAsyncKeyState('D')) || ((tetris->owner == PLAYER2) && GetAsyncKeyState(VK_RIGHT)))
	{
		// 블럭이 오른쪽으로 한 칸 이동 가능하면
		if (tetris->checkCrush(tetris, block->x + 1, block->y, block->rotation) == true)
			tetris->move_block(tetris, block, RIGHT);	// 블럭을 오른쪽으로 한 칸 이동
	}

	// 어떤 키를 입력 했는지 체크
	if ((tetris->owner == PLAYER1 && GetAsyncKeyState('S')) || ((tetris->owner == PLAYER2) && GetAsyncKeyState(VK_DOWN)))
	{
		// 블럭이 아래쪽으로 한 칸 이동 가능하면
		if (tetris->checkCrush(tetris, block->x, block->y + 1, block->rotation) == true)
			tetris->move_block(tetris, block, DOWN);	// 블럭을 한 칸 soft drop
	}

	// 어떤 키를 입력 했는지 체크
	if ((tetris->owner == PLAYER1 && GetAsyncKeyState('W')) || ((tetris->owner == PLAYER2) && GetAsyncKeyState(VK_UP)))
	{
		// 블럭을 회전 했을 때 충돌이 나지 않으면
		if (tetris->checkCrush(tetris, block->x, block->y, (block->rotation + 1) % 4) == true)
			tetris->move_block(tetris, block, UP);	// 블럭을 한 번 회전

		// 제자리에서 회전을 하면 충돌하는데 위로 한 칸 올라가면 충돌하면
		else if (tetris->checkCrush(tetris, block->x, block->y - 1, (block->rotation + 1) % 4) == true)	
			tetris->move_block(tetris, block, 100);	// 블럭을 한 칸 위로 올려 한 번 회전(아래에 있는 블럭과 충돌 중인 경우의 특수 동작)
	} 

	// 어떤 키를 입력 했는지 체크
	if ((tetris->owner == PLAYER1 && GetAsyncKeyState('G')) || (tetris->owner == PLAYER2 && GetAsyncKeyState('L')))
	{
		while (tetris->crush_on == false)	// 충돌이 날때까지 반복 (Hard drop)
		{
			tetris->dropBlock(tetris);	// 블럭을 한 칸 드랍
			tetris->score += tetris->level;	// Hard Drop을 한 경우 블럭을 떨어트린 칸 수 만큼 level을 곱해 점수 업
		}
		tetris->fCnt = 0;	// fCnt를 0으로 만들어 바로 ACTIVE 블록을 INACTIVE 블록으로 변경 후 처리하도록 함 (Tetris_dropBlock 함수 참고)
	}
}

void Tetris_checkLine(Tetris* tetris, Blocks* block)
{
	int block_amount;	// 한 줄에 있는 INACTIVE_BLOCK의 개수
	int combo = 0;	// 한 번에 지운 줄 수 저장

	for (int i = HEIGHT - 2; i > 3;)	// 게임을 플레이하는 공간부터 천장까지
	{
		block_amount = 0;	// 갯수 초기화
		for (int j = 1; j < WIDTH - 1; j++)
		{
			if (tetris->gameOrg[i][j] > 0)	// INACTIVE가 존재할 경우
				block_amount++;	// INACTIVE_BLOCK의 개수 +1
		}


		if (block_amount == WIDTH - 2)	// 한 줄이 완성된 경우
		{
			tetris->score += 100 * tetris->level;	// level * 100 만큼 점수 업
			tetris->lineCnt++;	// 지운 줄 수 +1
			combo++;	// combo 증가

			if (tetris->pushAttackRegP > 0)	// 공격 포인트가 있는 경우
			{
				for (int j = 1; j < WIDTH - 1; j++)
				{
					// 내가 마지막으로 놓아 라인을 지운 블럭을 제외한 블록들을 공격으로 보냄
					if (j > block->x && j < block->x + 4 && block->shape[block->type][block->rotation][i - block->y][j - block->x] == 1)
						tetris->pushAttackReg[tetris->pushAttackRegP][j] = 0;
					else
						tetris->pushAttackReg[tetris->pushAttackRegP][j] = 16;
				}
				// 보낼 공격을 한 줄 쌓았으므로 다음 윗 줄로 높이 변경
				tetris->pushAttackRegP--;
			}

			for (int k = i; k > 1; k--)	// 지워진 라인부터 천장까지
			{
				for (int l = 1; l < WIDTH - 1; l++)
				{
					if (tetris->gameOrg[k - 1][l] != CEILLING)	// 라인이 지워졌으므로 한 칸씩 모두 내림
						tetris->gameOrg[k][l] = tetris->gameOrg[k - 1][l];
					if (tetris->gameOrg[k - 1][l] == CEILLING)	// 윗 줄이 천장이면 내리면 안되므로 EMPTY
						tetris->gameOrg[k][l] = EMPTY;
				}
			}
		}
		else
			i--;	// 라인이 지워지지 않으면 i를 1 감소 (여러 줄이 한 번에 지워지는 처리를 위함)
	}

	if (combo)	// 콤보가 있을 경우
	{
		if (combo > 1)	// 콤보가 1 보다 크면
		{
			tetris->drawGame(tetris);	// 게임을 그린 후
			tetris->set_gameMsg(tetris, (WIDTH / 2) - 1, block->y - 2, 0, combo);	// 콤보 메시지 세팅
			tetris->score += (combo * tetris->level * 100);	// 콤보한 만큼 점수 업
		}
		else // 1일 때 (콤보가 아닌 경우) 
		{
			if (tetris->pushAttackRegP > 0)	// 보낼 공격이 존재하면
			{
				tetris->pushAttackRegP++;
				for (int m = 1; m < WIDTH - 1; m++)
				{
					// 보낼 공격을 한 줄 감소
					tetris->pushAttackReg[tetris->pushAttackRegP][m] = -1;
				}
			}
		}

		// 지운 줄 수가 5줄이 넘고 레벨이 9 이하면 (level 9가 최고레벨)
		if (tetris->lineCnt > 5 && tetris->level < 9)
		{
			tetris->level++;	// 레벨 업
			tetris->lineCnt = 0;	// 지운 줄 수 초기화

			if (tetris->speed[tetris->level] < tetris->speed[tetris->level - 1])	// 증가된 레벨의 속도를 비교
				tetris->set_gameMsg(tetris, (WIDTH / 2) - 1, 5, 1, 0);	// 증가된 레벨의 속도가 더 빠르면 SPEED UP 출력
			else
				tetris->set_gameMsg(tetris, (WIDTH / 2) - 1, 5, 2, 0);	// 증가된 레벨의 속도가 더 느리면 SPEED DOWN 출력
		
			gotoxy(tetris->status_x, tetris->status_y + 6);
			printf("Speed:%2d", tetris->speed[tetris->level]);	// 상태창에 게임 스피드 갱신
		}
	}

	for (int j = 1; j < WIDTH - 2; j++)
		if (tetris->gameOrg[3][j] > 0)	// 천장에 INACTIVE 블럭이 닿으면
			tetris->gameOver_on = true;	// 게임 오버 On
}

void Tetris_getAttack(Tetris* tetris)
{
	// getAttackRegP의 기본값이 HEIGHT - 1 이므로 
	if (tetris->getAttackRegP < HEIGHT - 1)	// 공격 받을 줄이 있다면
	{
		// 공격 받을 줄 수를 저장하는 변수
		int line = (HEIGHT - 1) - (tetris->getAttackRegP);

		for (int i = 4; i < HEIGHT - 1; i++)	// 천장부터 바닥 전까지
		{
			for (int j = 1; j < WIDTH - 1; j++)
			{
				if(i - line > 0)	// 지우는 라인이 배열을 넘어가면 안되므로
					tetris->gameOrg[i - line][j] = tetris->gameOrg[i][j];	// 공격 받을 줄 수 만큼 블럭을 올림
					tetris->gameOrg[i][j] = EMPTY;	// 올린 곳은 빈 곳으로 만듬
			}
		}


		for (int i = tetris->getAttackRegP + 1; i < HEIGHT; i++) // 내가 공격 받을 줄이 저장된 배열의 높이 만큼
		{
			for (int j = 1; j < WIDTH - 1; j++)
			{
				tetris->gameOrg[i - 1][j] = tetris->getAttackReg[i][j];	// 올려서 빈 곳에 공격 받는 줄 저장
				tetris->getAttackReg[i][j] = -1;	// 공격 받았으므로 배열에 저장 된 공격 받을 줄을 지움
			}
		}

		tetris->getAttackRegP += line;	// 공격 받은 만큼 공격 받을 배열의 높이를 저장하는 변수값 수정
		
		for (int i = 1; i < 10; i++)
		{
			// 화면에 표기 된 공격 예정 표시를 지움
			gotoxy(tetris->attackQueue_x, HEIGHT + tetris->game_y - i);
			printf("  ");
		}
	}
}



// 함수 원형 선언 (*아래의 함수의 주소를 저장하는 구조체(BattleTetrisManager)의 멤버(함수 포인터)를 위함)
// 함수 포인터를 사용한 이유는 C언어의 구조체를 C++의 클래스처럼 사용하기 위함
void BTM_init(BattleTetrisManager* btm);
void BTM_resetManager(BattleTetrisManager* btm);
void BTM_gamePlay(Tetris* A);
void BTM_getKey(BattleTetrisManager* btm);
void BTM_pushAttack(Tetris* A, Tetris* B);
void BTM_checkWinner(BattleTetrisManager* btm, Tetris* A, Tetris* B);

typedef struct BTM {

	// 구조체(BattleTetrisManager)를 초기화하는 함수 (C++의 생성자)
	void (*init)(BattleTetrisManager* btm) = BTM_init;
	Tetris *p1;	// tetris 구조체를 저장하는 포인터
	Tetris *p2;

	// 게임을 초기화하는 함수 (tetris.init()함수를 실행해 Tetris 구조체를 초기화하고 게임을 시작)
	void (*resetManager)(BattleTetrisManager* btm) = BTM_resetManager;


	// 게임을 진행하는 함수
	void (*gamePlay)(Tetris* A) = BTM_gamePlay;

	// ESC를 입력 받으면 게임을 종료시키고 감사 메시지를 출력하는 함수
	void (*getKey)(BattleTetrisManager* btm) = BTM_getKey;

	// A가 B로 공격을 push하는 함수
	void (*pushAttack)(Tetris* A, Tetris* B) = BTM_pushAttack;

	// 승리자가 있는지 확인하는 함수
	void (*checkWinner)(BattleTetrisManager* btm, Tetris* A, Tetris* B) = BTM_checkWinner;
		bool winner_on;	// 한 쪽이 게임 오버를 해 승리자가 나오면 On(true) 아니면 false

}BattleTetrisManager;

void BTM_init(BattleTetrisManager* btm)
{
	// 플레이어가 저장될 포인터 초기화
	btm->p1 = NULL;
	btm->p2 = NULL;
}

void BTM_resetManager(BattleTetrisManager* btm)
{
	// 콘솔 화면을 지우는 함수
	system("cls");
	// 게임을 재시작 했을 때는 true이므로 게임을 시작하기 전에 false로 변경
	btm->winner_on = false;

	// player1의 블럭 구조체 초기화
	btm->p1->block->init(btm->p1->block);
	// player1의 Tetris 구조체 초기화 및 좌표 설정
	btm->p1->init(btm->p1, 2, 1, 14, 2, PLAYER1);


	// player2의 블럭 구조체 초기화
	btm->p2->block->init(btm->p2->block);
	// player2의 Tetris 구조체 초기화 및 좌표 설정
	btm->p2->init(btm->p2, 27, 1, 22, 2, PLAYER2);

	// 게임 종료를 할 수 있는 안내 메시지 출력
	gotoxy(31, 26);
	printf("Game Exit: ESC");

	// 로비에서 입력된 키값 제거
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
	if (A->gameMsgCnt >= 0)	// 게임 메시지가 켜져있을 경우
		A->cnt_gameMsg(A);	// 게임 메시지 Cnt를 감소시키는 함수 호출

	if (A->gameDelay_on == true)	// 게임 딜레이가 켜져있을 경우
	{
		A->cnt_gameDelay(A);	// 게임 딜레이 Cnt를 감소시키는 함수 호출
	}
	else    // 게임 딜레이가 꺼져있을 경우
	{
		if (A->gameOver_on == false)	// 게임 오버가 아닌 경우
		{
			// 사용자로부터 키를 입력받는 함수 호출
			A->getKey(A, A->block);
		}
		else    // 게임 오버인 경우
		{
			// 게임 오버 애니메이션을 진행하는 함수 호출
			A->gameOver(A);
		}

		// gameOrg와 gameCpy를 비교해 게임을 그리는 함수 호출
		A->drawGame(A);
	}
}

void BTM_getKey(BattleTetrisManager* btm)
{
	if (GetAsyncKeyState(VK_ESCAPE))	// ESC를 입력하면
	{
		gotoxy(0, 24);
		printf("Thanks for playing :)");	// 감사 문구 출력
		exit(0);	// 프로그램 종료
	}
}

void BTM_pushAttack(Tetris* A, Tetris* B)
{
	if (A->pushAttackRegP < HEIGHT - 1)	// 받을 공격이 있는 경우
	{
		int line = (HEIGHT - 1) - (A->pushAttackRegP);	// 공격 받을 줄 수

		for (int i = B->getAttackRegP + 1; i < HEIGHT; i++)
		{
			for (int j = 0; j < WIDTH - 1; j++)
			{
				B->getAttackReg[i - line][j] = B->getAttackReg[i][j];	// 공격이 추가로 들어오면 배열에 있는 줄 수를 추가하기 위해 위로 올린다.
																		// (밑에서 부터 쌓이므로)
			}
		}

		for (int i = A->pushAttackRegP + 1; i < HEIGHT; i++)
		{
			for (int j = 0; j < WIDTH - 1; j++)
			{
				if(i - line > 0) // 추가되는 줄 수가 배열을 넘어가면 안되므로
					B->getAttackReg[i][j] = A->pushAttackReg[i][j];	// B의 공격 받을 줄을 저장하는 배열로 A가 공격할 줄을 저장하는 배열의 값을 옮긴다
					A->pushAttackReg[i][j] = -1;	// A가 B로 공격한 만큼 배열을 비운다.
			}
		}
		B->getAttackRegP -= line;	// 공격 받을 블럭이 쌓였으므로 높이를 높인다.
		A->pushAttackRegP += line;	// 공격 할 블럭을 push 했으므로 높이를 낮춘다.

		// 공격 예고 아이콘 갯수
		int queue = HEIGHT - 1 - B->getAttackRegP;
		int queueP = 0;	// 공격 예고 아이콘의 높이

		while (queue >= 5) // 공격받을 줄 수가 5개 이상이면 "★"로 표시
		{
			gotoxy(B->attackQueue_x, B->game_y - 2 - queueP + HEIGHT);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x000C);	// 별 색을 빨간색으로 지정
			printf("★");
			queue -= 5;
			queueP++;
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);
		
		while (queue >= 1) { // 공격받을 줄 수가 1개 이상이면 "○"로 표시
			gotoxy(B->attackQueue_x, B->game_y - 2 - queueP + HEIGHT);
			printf("○");
			queue--;
			queueP++;
		}
		
		while (queueP < 10) { //최대 10칸까지 표시하고 나머지 칸은 빈 칸으로 표시
			gotoxy(B->attackQueue_x, B->game_y - 2 - queueP + HEIGHT);
			printf("  ");
			queueP++;
		}
	}
}

void BTM_checkWinner(BattleTetrisManager* btm, Tetris* A, Tetris* B)
{
	if (btm->winner_on == true)	// 게임의 승자가 나온 경우
	{
		if(GetAsyncKeyState(VK_RETURN))	// ENTER를 입력하면 게임 재시작
			btm->resetManager(btm);
	}

	int whoWin;	// 누가 이겼는지 저장
	
	// 승자가 없는 상황에서 A가 게임 오버인 경우
	if (btm->winner_on == false && A->gameOver_on == true)
	{
		btm->winner_on = true;	// 승자가 나왔으므로 true 저장
		whoWin = B->owner;	// 승자는 B
	}

	// 승자가 없는 상황에서 B가 게임 오버인 경우
	else if (btm->winner_on == false && B->gameOver_on == true)
	{
		btm->winner_on = true;	// 승자가 나왔으므로 true 저장
		whoWin = A->owner;	// 승자는 A
	}
	else return;	// 아무 것도 아니면 함수 반환

	gotoxy(15, 10);
	switch (whoWin)	// 이긴 사람에 따라 출력 메시지 변경
	{
	case 1:
		printf(" << PLAYER 1 WIN >>");
		break;
	case 2:
		printf(" << PLAYER 2 WIN >>");
		break;
	}
	gotoxy(14, 12);
	printf("Press <ENTER> to restart");	// 엔터를 누르면 재시작한다는 안내 문구 출력
}


void titleMenu(void)
{
	// "BATTLE"을 출력할 커서 좌표
	int text_battle_x = 3;
	int text_battle_y = 2;
	
	// "TETRIS"를 출력할 커서 좌표
	int text_tetris_x = 16;
	int text_tetris_y = 9;

	// "< Press Enter Key to Start"를 출력할 커서 좌표
	int text_pressKey_x = 20;
	int text_pressKey_y = 15;
	
	// "BATTLE" 문구의 흔들림을 구현하기 위한 배열
	int text_battle_order[9] = { 0,1,2,3,4,2,1,4,2 };
	
	// Text "BATTLE"을 저장한 배열
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

	// Text "TETRIS"을 저장한 배열
	int text_tetris[5][21] = {
		1,1,1,0,1,1,1,0,1,1,1,0,1,1,0,0,1,0,1,1,1,
		0,1,0,0,1,0,0,0,0,1,0,0,1,0,1,0,1,0,1,0,0,
		0,1,0,0,1,1,1,0,0,1,0,0,1,1,0,0,1,0,1,1,1,
		0,1,0,0,1,0,0,0,0,1,0,0,1,0,1,0,1,0,0,0,1,
		0,1,0,0,1,1,1,0,0,1,0,0,1,0,1,0,1,0,1,1,1
	};

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0003);
	// Text "BATTLE" 출력
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

	// 입력 키 안내
	gotoxy(2, 16); printf("┌─── <  Key Instructions  >───┐ ");
	gotoxy(2, 17); printf("│ PLAYER 1            PLAYER 2│ ");
	gotoxy(2, 18); printf("│    W         UP        ↑   │ ");
	gotoxy(2, 19); printf("│    D        RIGHT      →   │ ");
	gotoxy(2, 20); printf("│    A        LEFT       ←   │ ");
	gotoxy(2, 21); printf("│    S      SOFT DROP    ↓   │ ");
	gotoxy(2, 22); printf("│    G      HARD DROP    L    │ ");
	gotoxy(2, 23); printf("└─────────────────────────────┘ ");


	for (int cnt = 0;; cnt++) 
	{
		Sleep(10);

		if ((cnt + 0) % 60 == 0) 
		{
			// 게임 스타트 키 안내
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
			// 글자 색 변경
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cnt % 7 + 2);
			
			// Text "TETRIS" 출력 
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

		// 엔터를 입력하면 반복문 탈출 (탈출 후 titleMenu 함수가 종료되면 게임 시작)
		if (GetAsyncKeyState(VK_RETURN)) 
			break;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);	// 커서 색 변경
}


int main(void)
{
	srand((unsigned)time(NULL));	// 난수 생성
	setcursortype(NOCURSOR);	// 커서 제거
	titleMenu();	// 게임 타이틀 출력


	// GM 생성 및 멤버 초기화
	BattleTetrisManager GM;
	GM.init(&GM);

	// Player 생성 및 GM에 등록
	Tetris player1;
	Tetris player2;
	GM.p1 = &player1;
	GM.p2 = &player2;

	// 각 Player의 Block 생성 후 각 플레이어에 등록
	Blocks p1_block, p2_block;
	GM.p1->block = &p1_block;
	GM.p2->block = &p2_block;

	// GM 리셋
	GM.resetManager(&GM);

	while (1)	// 게임 진행
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