#include <Windows.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <atlImage.h>
#include <Vfw.h>
#include <fmod.h>
#include "resource.h"
#include "2015182007_김선중.h"

#pragma comment (lib, "fmodex_vc.lib")

typedef struct Enemy_Info
{
	int poke;
	char name[15];
	char Skill1[15];
	char Skill2[15];
}Enemy_info;
typedef struct My_info
{
	int What_Poke;
	int poke;
	char name[15];
	int Level, MaxHp;
	int Stack_Damage;
	char Skill1[15], Skill2[15], Skill3[15], Skill4[15];
	int Limit1, Limit2, Limit3, Limit4;
}My_info;
My_info my_info[6];//0번째 주소는 안씀
int Potion = 50, Ball = 50, HiperBall = 50;//아이템들의갯수
int Item_Judge;
int Bag[4];
int state=5;
int EnemyHpBarPos = 421, MyHpBarPos = 933;//hp bar위치입니다.
int What_Poke = 1;//첫스타트 포켓몬은 1이다.
int TalkBox_Judge = 0, Show_Ball_Throw;//TalkBox_Judge=대화상태따라가기,Stack_Damage1=주인공데미지스택
int Field_Level = 1;//필드레벨
BOOL EnemyTurn;
BOOL BOSSKILL;
CImage Skill[50];
CImage pokemon[500];
CImage backpoke[500];
CImage battle[5];//battle3==포켓몬 교환창
CImage Ball_Throw[20];
CImage Exchange_Screen;
CImage bSelect;
CImage mbag[3];
CImage bwmap[9];
CImage bPro;
BOOL Catch;
BOOL Skill_Using;
bool wildm = true;
int cmap;
HWND hWndAVI = 0;
char AVfile[] = "movies.avi";

HWND hWndAVI2 = 0;
char AVfile2[] = "movies2.avi";

#define MAP_SIZEX 7
#define MAP_SIZEY 7

#define LIMIT_BGM 13
#define LIMIT_EFFECT 4

HINSTANCE g_hinst;
LPCTSTR lpszClass = "Window Class Name";
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam);
LRESULT CALLBACK ChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//인버서스
LRESULT CALLBACK BattleProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//배틀

FMOD_SYSTEM *g_System;
FMOD_SOUND *g_Sound[LIMIT_BGM];
FMOD_SOUND *e_Sound[LIMIT_EFFECT];
FMOD_CHANNEL *g_Channel[2];

void Init()
{
	char btr[128];
	FMOD_System_Create(&g_System);
	FMOD_System_Init(g_System, 10, FMOD_INIT_NORMAL, NULL);

	for (int i = 0; i < LIMIT_EFFECT; i++)
	{
		wsprintf(btr, "sound\\effect\\effect%d.mp3", i);
		FMOD_System_CreateSound(g_System, btr, FMOD_DEFAULT, 0, &e_Sound[i]);
	}
	for (int i = 0; i < LIMIT_BGM; i++)
	{
		wsprintf(btr, "sound\\bgm\\bgm%d.mp3", i);
		FMOD_System_CreateSound(g_System, btr, FMOD_LOOP_NORMAL, 0, &g_Sound[i]);
	}
}

void PlayBgm(int sound)
{
	FMOD_BOOL IsPlaying;
	FMOD_Channel_IsPlaying(g_Channel[0], &IsPlaying);
	if (IsPlaying == TRUE)
		FMOD_Channel_Stop(g_Channel[0]);
	FMOD_System_Update(g_System);
	FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[sound], 0, &g_Channel[0]);
}

void PlayEffect(int sound)
{
	FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, e_Sound[sound], 0, &g_Channel[1]);
}

void Release()
{
	for (int i = 0; i < LIMIT_EFFECT; i++)
	{
		FMOD_Sound_Release(e_Sound[i]);
	}

	for (int i = 0; i < LIMIT_BGM; i++)
	{
		FMOD_Sound_Release(g_Sound[i]);
	}
	FMOD_System_Close(g_System);
	FMOD_System_Release(g_System);
}

void Draw_MsgBox(HDC memDC, int MsgJudge)
{
	if (MsgJudge == 0)
		bSelect.TransparentBlt(memDC, 20, 545, 50, 50, RGB(255, 255, 255));
	else if (MsgJudge == 1)
		bSelect.TransparentBlt(memDC, 520, 545, 50, 50, RGB(255, 255, 255));
	else if (MsgJudge == 2)
		bSelect.TransparentBlt(memDC, 20, 617, 50, 50, RGB(255, 255, 255));
	else if (MsgJudge == 3)
		bSelect.TransparentBlt(memDC, 520, 617, 50, 50, RGB(255, 255, 255));
}
void My_Skill_Selection(char str[], int Damage2, int Skill_Selection,int *Skill_progress,HWND hwnd)
{
	if (Skill_Selection == 1)
		wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", my_info[What_Poke].Skill1, Damage2);
	else if (Skill_Selection == 2)
	{
		*Skill_progress = 2;
		KillTimer(hwnd, 1);
		SetTimer(hwnd, 8, 60, NULL);
		wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", my_info[What_Poke].Skill2, Damage2);
	}
	else if (Skill_Selection == 3)
	{
		if (my_info[What_Poke].poke != 382 && my_info[What_Poke].poke != 383 && my_info[What_Poke].poke != 384)
			*Skill_progress = 7;
		else
			*Skill_progress = 15;
		KillTimer(hwnd, 1);
		SetTimer(hwnd, 9, 100, NULL);
		wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", my_info[What_Poke].Skill3, Damage2);
	}
	else if (Skill_Selection == 4)
	{
		if (my_info[What_Poke].poke != 382 && my_info[What_Poke].poke != 383 && my_info[What_Poke].poke != 384)
			*Skill_progress = 11;
		else if (my_info[What_Poke].poke == 382)
			*Skill_progress = 27;
		else if (my_info[What_Poke].poke == 383)
			*Skill_progress = 19;
		else if (my_info[What_Poke].poke == 384)
			*Skill_progress = 23;
		KillTimer(hwnd, 1);
		SetTimer(hwnd, 10, 100, NULL);
		wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", my_info[What_Poke].Skill4, Damage2);
	}

	Skill_Using = TRUE;
}
void Enemy_Attack(int Skill_Selection, HDC memDC, char str[], Enemy_Info info[], int Respawn, RECT fect, RECT rectView, int Damage1, int MMovePos)
{
	if (state == 1)
	{
		if (Skill_Selection == 0)
		{
			Skill[1].TransparentBlt(memDC, rectView.right / 10 + MMovePos + 200, rectView.bottom * 6 / 15, 150, 150, RGB(255, 255, 255));
			wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", info[Respawn].Skill1, Damage1);
		}
		else if (Skill_Selection == 1)
		{
			Skill[6].TransparentBlt(memDC, rectView.right / 10 + MMovePos + 200, rectView.bottom * 6 / 15, 150, 150, RGB(255, 255, 255));
			wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", info[Respawn].Skill2, Damage1);
		}
		else if (Skill_Selection == 1)
			wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", info[Respawn].Skill2, Damage1);
	}
	else if (state == 2)
	{
		if (Skill_Selection == 0)
		{
			Skill[18].TransparentBlt(memDC, rectView.right / 10 + MMovePos + 200, rectView.bottom * 6 / 15, 150, 150, RGB(255, 255, 255));
			wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", info[Respawn].Skill1, Damage1);
		}
		else if (Skill_Selection == 1)
		{
			Skill[28].TransparentBlt(memDC, rectView.right / 10 + MMovePos + 150, rectView.bottom * 6 / 15, 300, 200, RGB(255, 255, 255));
			wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", info[Respawn].Skill2, Damage1);
		}
		else if (Skill_Selection == 1)
			wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", info[Respawn].Skill2, Damage1);
	}
	else if (state == 3)
	{
		if (Skill_Selection == 0)
		{
			Skill[18].TransparentBlt(memDC, rectView.right / 10 + MMovePos + 200, rectView.bottom * 6 / 15, 150, 150, RGB(255, 255, 255));
			wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", info[Respawn].Skill1, Damage1);
		}
		else if (Skill_Selection == 1)
		{
			Skill[22].TransparentBlt(memDC, rectView.right / 10 + MMovePos + 200, rectView.bottom * 6 / 15, 300, 200, RGB(255, 255, 255));
			wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", info[Respawn].Skill2, Damage1);
		}
		else if (Skill_Selection == 1)
			wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", info[Respawn].Skill2, Damage1);
	}
	else if (state == 4)
	{
		if (Skill_Selection == 0)
		{
			Skill[18].TransparentBlt(memDC, rectView.right / 10 + MMovePos + 200, rectView.bottom * 6 / 15, 150, 150, RGB(255, 255, 255));
			wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", info[Respawn].Skill1, Damage1);
		}
		else if (Skill_Selection == 1)
		{
			Skill[26].TransparentBlt(memDC, rectView.right / 10 + MMovePos + 200, rectView.bottom * 6 / 15, 300, 200, RGB(255, 255, 255));
			wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", info[Respawn].Skill2, Damage1);
		}
		else if (Skill_Selection == 1)
			wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", info[Respawn].Skill2, Damage1);
	}
	else if (state == 5)
	{
		if (Skill_Selection == 0)
		{
			Skill[29].TransparentBlt(memDC, rectView.right / 10 + MMovePos + 200, rectView.bottom * 6 / 15, 150, 150, RGB(255, 255, 255));
			wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", info[Respawn].Skill1, Damage1);
		}
		else if (Skill_Selection == 1)
		{
			Skill[33].TransparentBlt(memDC, rectView.right / 10 + MMovePos + 200, rectView.bottom * 6 / 15, 300, 200, RGB(255, 255, 255));
			wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", info[Respawn].Skill2, Damage1);
		}
		else if (Skill_Selection == 1)
			wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", info[Respawn].Skill2, Damage1);
	}
	DrawText(memDC, str, -1, &fect, DT_LEFT);
}
void Enemy_Play(int Skill_Selection, int MMovePos, int count, HWND hWnd, int *Damage1, int EnemyLv, int *Skill_Progress)
{
	KillTimer(hWnd, 1);
	SetTimer(hWnd, 3, 10, NULL);
	if (state == 1)
	{
		if (Skill_Selection == 0)
			*Damage1 = 2 * EnemyLv;
		else if (Skill_Selection == 1)
		{
			TalkBox_Judge = 12;
			KillTimer(hWnd, 1);
			SetTimer(hWnd, 7, 60, NULL);
			*Skill_Progress = 2;
			*Damage1 = 3 * EnemyLv;
		}
	}
	else if (state == 2)
	{
		if (Skill_Selection == 0)
		{
			TalkBox_Judge = 12;
			KillTimer(hWnd, 1);
			SetTimer(hWnd, 7, 100, NULL);
			*Skill_Progress = 15;
			*Damage1 = 3 * EnemyLv;
		}
		else if (Skill_Selection == 1)
		{
			TalkBox_Judge = 12;
			KillTimer(hWnd, 1);
			SetTimer(hWnd, 7, 100, NULL);
			*Skill_Progress = 27;
			*Damage1 = 4 * EnemyLv;
		}
	}
	else if (state == 3)
	{
		if (Skill_Selection == 0)
		{
			TalkBox_Judge = 12;
			KillTimer(hWnd, 1);
			SetTimer(hWnd, 7, 100, NULL);
			*Skill_Progress = 15;
			*Damage1 = 3 * EnemyLv;
		}
		else if (Skill_Selection == 1)
		{
			TalkBox_Judge = 12;
			KillTimer(hWnd, 1);
			SetTimer(hWnd, 7, 100, NULL);
			*Skill_Progress = 19;
			*Damage1 = 4 * EnemyLv;
		}
	}
	else if (state == 4)
	{
		if (Skill_Selection == 0)
		{
			TalkBox_Judge = 12;
			KillTimer(hWnd, 1);
			SetTimer(hWnd, 7, 100, NULL);
			*Skill_Progress = 15;
			*Damage1 = 3 * EnemyLv;
		}
		else if (Skill_Selection == 1)
		{
			TalkBox_Judge = 12;
			KillTimer(hWnd, 1);
			SetTimer(hWnd, 7, 100, NULL);
			*Skill_Progress = 23;
			*Damage1 = 4 * EnemyLv;
		}
	}
	else if (state == 5)
	{
		if (Skill_Selection == 0)
		{
			
			*Damage1 = 200;
		}
		else if (Skill_Selection == 1)
		{
			TalkBox_Judge = 12;
			KillTimer(hWnd, 1);
			SetTimer(hWnd, 7, 100, NULL);
			*Skill_Progress = 30;
			*Damage1 = 300;
		}
	}
	my_info[What_Poke].Stack_Damage += *Damage1;
}
void Draw_Pokemon_BackPack(int Bag[], int count, HDC memDC, RECT rectView, char str[])
{
	HBRUSH hBrush, oldBrush;
	double MyRatio;
	for (int i = 0; i < 4; i++)//포켓몬 아이템창 보여주기
	{
		if (Bag[i] == 0)continue;
		if (count == 0)
		{
			pokemon[my_info[Bag[i]].poke].Draw(memDC, rectView.right * 3 / 16 - 70, rectView.bottom * 11 / 16, 70, 70);
			RECT textect1 = { rectView.right * 3 / 16, rectView.bottom * 11 / 16, rectView.right * 15 / 16, rectView.bottom };
			wsprintf(str, "%s", my_info[Bag[i]].name);//E name
			DrawText(memDC, str, -1, &textect1, DT_LEFT);

			RECT textect2 = { rectView.right * 2 / 16 - 30, rectView.bottom * 12 / 16 + 15, rectView.right * 15 / 16, rectView.bottom };
			wsprintf(str, "%d", my_info[Bag[i]].Level);//E name
			DrawText(memDC, str, -1, &textect2, DT_LEFT);

			RECT textect3 = { rectView.right * 5 / 16 - 30, rectView.bottom * 12 / 16 + 15, rectView.right * 15 / 16, rectView.bottom };
			wsprintf(str, "%d", (my_info[Bag[i]].MaxHp - my_info[Bag[i]].Stack_Damage));//E name
			DrawText(memDC, str, -1, &textect3, DT_LEFT);

			RECT textect4 = { rectView.right * 6 / 16 + 30 , rectView.bottom * 12 / 16 + 15, rectView.right * 15 / 16, rectView.bottom };
			wsprintf(str, "%d", my_info[Bag[i]].MaxHp);//E name
			DrawText(memDC, str, -1, &textect4, DT_LEFT);

			hBrush = CreateSolidBrush(RGB(0, 0, 0));
			oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
			MyRatio = 190 / (double)my_info[Bag[0]].MaxHp;
			Rectangle(memDC, 441 - (my_info[Bag[0]].Stack_Damage*MyRatio), 552, 441, 561);
			SelectObject(memDC, oldBrush);
			DeleteObject(hBrush);//HP의 손실을 보여주기

			count++;
		}
		else if (count == 1)
		{
			pokemon[my_info[Bag[i]].poke].Draw(memDC, rectView.right * 11 / 16 - 70, rectView.bottom * 11 / 16 + 20, 70, 70);
			RECT textect1 = { rectView.right * 11 / 16, rectView.bottom * 11 / 16 + 20, rectView.right * 15 / 16, rectView.bottom };
			wsprintf(str, "%s", my_info[Bag[i]].name);//E name
			DrawText(memDC, str, -1, &textect1, DT_LEFT);

			RECT textect2 = { rectView.right * 10 / 16 - 30, rectView.bottom * 12 / 16 + 35, rectView.right * 15 / 16, rectView.bottom };
			wsprintf(str, "%d", my_info[Bag[i]].Level);//E name
			DrawText(memDC, str, -1, &textect2, DT_LEFT);

			RECT textect3 = { rectView.right * 13 / 16 - 60, rectView.bottom * 12 / 16 + 35, rectView.right * 15 / 16, rectView.bottom };
			wsprintf(str, "%d", my_info[Bag[i]].MaxHp - my_info[Bag[i]].Stack_Damage);//E name
			DrawText(memDC, str, -1, &textect3, DT_LEFT);

			RECT textect4 = { rectView.right * 14 / 16 , rectView.bottom * 12 / 16 + 35, rectView.right * 15 / 16, rectView.bottom };
			wsprintf(str, "%d", my_info[Bag[i]].MaxHp);//E name
			DrawText(memDC, str, -1, &textect4, DT_LEFT);
			count++;

			hBrush = CreateSolidBrush(RGB(0, 0, 0));
			oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
			MyRatio = 190 / (double)my_info[Bag[1]].MaxHp;
			Rectangle(memDC, 946 - (my_info[Bag[1]].Stack_Damage*MyRatio), 571, 946, 580);
			SelectObject(memDC, oldBrush);
			DeleteObject(hBrush);//HP의 손실을 보여주기
		}
		else if (count == 2)
		{
			pokemon[my_info[Bag[i]].poke].Draw(memDC, rectView.right * 3 / 16 - 70, rectView.bottom * 13 / 16 + 20, 70, 70);
			RECT textect1 = { rectView.right * 3 / 16, rectView.bottom * 13 / 16 + 20, rectView.right * 15 / 16, rectView.bottom };
			wsprintf(str, "%s", my_info[Bag[i]].name);//E name
			DrawText(memDC, str, -1, &textect1, DT_LEFT);

			RECT textect2 = { rectView.right * 2 / 16 - 30, rectView.bottom * 14 / 16 + 30, rectView.right * 15 / 16, rectView.bottom };
			wsprintf(str, "%d", my_info[Bag[i]].Level);//E name
			DrawText(memDC, str, -1, &textect2, DT_LEFT);

			RECT textect3 = { rectView.right * 5 / 16 - 30, rectView.bottom * 14 / 16 + 30, rectView.right * 15 / 16, rectView.bottom };
			wsprintf(str, "%d", my_info[Bag[i]].MaxHp - my_info[Bag[i]].Stack_Damage);//E name
			DrawText(memDC, str, -1, &textect3, DT_LEFT);

			RECT textect4 = { rectView.right * 6 / 16 + 30 , rectView.bottom * 14 / 16 + 30, rectView.right * 15 / 16, rectView.bottom };
			wsprintf(str, "%d", my_info[Bag[i]].MaxHp);//E name
			DrawText(memDC, str, -1, &textect4, DT_LEFT);
			count++;

			hBrush = CreateSolidBrush(RGB(0, 0, 0));
			oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
			MyRatio = 190 / (double)my_info[Bag[2]].MaxHp;
			Rectangle(memDC, 441 - (my_info[Bag[2]].Stack_Damage*MyRatio), 661, 441, 670);
			SelectObject(memDC, oldBrush);
			DeleteObject(hBrush);//HP의 손실을 보여주기
		}
		else if (count == 3)
		{
			pokemon[my_info[Bag[i]].poke].Draw(memDC, rectView.right * 11 / 16 - 70, rectView.bottom * 13 / 16 + 40, 70, 70);
			RECT textect1 = { rectView.right * 11 / 16, rectView.bottom * 13 / 16 + 40, rectView.right * 15 / 16, rectView.bottom };
			wsprintf(str, "%s", my_info[Bag[i]].name);//E name
			DrawText(memDC, str, -1, &textect1, DT_LEFT);

			RECT textect2 = { rectView.right * 10 / 16 - 30, rectView.bottom * 14 / 16 + 55, rectView.right * 15 / 16, rectView.bottom };
			wsprintf(str, "%d", my_info[Bag[i]].Level);//E name
			DrawText(memDC, str, -1, &textect2, DT_LEFT);

			RECT textect3 = { rectView.right * 13 / 16 - 60, rectView.bottom * 14 / 16 + 55, rectView.right * 15 / 16, rectView.bottom };
			wsprintf(str, "%d", my_info[Bag[i]].MaxHp - my_info[i].Stack_Damage);//E name
			DrawText(memDC, str, -1, &textect3, DT_LEFT);

			RECT textect4 = { rectView.right * 14 / 16 , rectView.bottom * 14 / 16 + 55, rectView.right * 15 / 16, rectView.bottom };
			wsprintf(str, "%d", my_info[Bag[i]].MaxHp);//E name
			DrawText(memDC, str, -1, &textect4, DT_LEFT);
			count++;

			hBrush = CreateSolidBrush(RGB(0, 0, 0));
			oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
			MyRatio = 190 / (double)my_info[Bag[2]].MaxHp;
			Rectangle(memDC, 945 - (my_info[Bag[2]].Stack_Damage*MyRatio), 678, 945, 687);
			SelectObject(memDC, oldBrush);
			DeleteObject(hBrush);//HP의 손실을 보여주기
		}
	}
}
void Hp_State_Draw(HDC memDC, int EnemyHp, int Stack_Damage2)
{
	HBRUSH hBrush, oldBrush;
	double MyRatio, EnemyRatio;
	hBrush = CreateSolidBrush(RGB(0, 0, 0));
	oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
	MyRatio = 200 / (double)my_info[What_Poke].MaxHp;//HP_Bar 크기/포켓몬MaxHp
	EnemyRatio = 206 / (double)EnemyHp;
	if (my_info[What_Poke].Stack_Damage >= my_info[What_Poke].MaxHp)my_info[What_Poke].Stack_Damage = my_info[What_Poke].MaxHp;//이거랑 그 밑에꺼는 MAX HP만큼 피가 깎인것을 보여주려고한것입니다.
	if (Stack_Damage2 >= EnemyHp)Stack_Damage2 = EnemyHp;
	Rectangle(memDC, EnemyHpBarPos - (Stack_Damage2*EnemyRatio), 152, EnemyHpBarPos, 163);
	Rectangle(memDC, MyHpBarPos - (my_info[What_Poke].Stack_Damage*MyRatio), 410, MyHpBarPos, 421);
	SelectObject(memDC, oldBrush);
	DeleteObject(hBrush);//HP의 손실을 보여주기
	if (Stack_Damage2 > (double)EnemyHp / 1.5)Catch = TRUE;//포획조건
}
void What_Skill_Selection(int MsgJudge, int *Skill_Selection, int *Damage2, int *Stack_Damage2, int *EMovePos, int *count, HWND hWnd)
{
	for (int i = 0; i < 4; i++)
	{
		if (i == MsgJudge)
		{
			if (i == 0)my_info[What_Poke].Limit1--;
			else if (i == 1)my_info[What_Poke].Limit2--;
			else if (i == 2)my_info[What_Poke].Limit3--;
			else if (i == 3)my_info[What_Poke].Limit4--;
			*Skill_Selection = i + 1;
			if (my_info[What_Poke].poke != 382 && my_info[What_Poke].poke != 383 && my_info[What_Poke].poke != 384)
				*Damage2 = (i + 2) * my_info[What_Poke].Level;
			else
				*Damage2 = (i + 5)*my_info[What_Poke].Level;
			*Stack_Damage2 += *Damage2;
			*EMovePos = 0, *count = 0;
			KillTimer(hWnd, 1);
			SetTimer(hWnd, 2, 10, NULL);
		}
	}
}
void Battle_Begin(LOGFONT lf, RECT rectView, HDC memDC, char str[], int EnemyLv, Enemy_info info[], int Respawn,int state)
{
	HFONT Font, OldFont;
	lf.lfHeight = rectView.bottom / 20;
	Font = CreateFontIndirect(&lf);
	OldFont = (HFONT)SelectObject(memDC, Font);
	
		RECT textect = { rectView.right * 1 / 15, rectView.bottom * 2 / 15, rectView.right * 14 / 15, rectView.bottom };
		wsprintf(str, "%s", info[Respawn].name);//E name
		DrawText(memDC, str, -1, &textect, DT_LEFT);

		RECT textect2 = { rectView.right * 9 / 15, rectView.bottom * 8 / 16, rectView.right * 14 / 15, rectView.bottom };
		wsprintf(str, "%s", my_info[What_Poke].name);//M name
		DrawText(memDC, str, -1, &textect2, DT_LEFT);

		RECT textect3 = { rectView.right * 6 / 16, rectView.bottom * 2 / 15, rectView.right * 14 / 15, rectView.bottom };//적 레벨
		wsprintf(str, "%d", EnemyLv);//Enemy Lv
		DrawText(memDC, str, -1, &textect3, DT_LEFT);

		RECT textect4 = { rectView.right * 14 / 16, rectView.bottom * 8 / 16, rectView.right, rectView.bottom };//내 레벨
		wsprintf(str, "%d", my_info[What_Poke].Level);//My Lv
		DrawText(memDC, str, -1, &textect4, DT_LEFT);

		RECT textect5 = { rectView.right * 14 / 16, rectView.bottom * 10 / 17, rectView.right, rectView.bottom };//내 최대 hp
		wsprintf(str, "%d", my_info[What_Poke].MaxHp);//MaxHp==MyHp
		DrawText(memDC, str, -1, &textect5, DT_LEFT);

		RECT textect6 = { rectView.right * 13 / 17, rectView.bottom * 10 / 17, rectView.right, rectView.bottom };//내 현재 hp
		wsprintf(str, "%d", my_info[What_Poke].MaxHp - my_info[What_Poke].Stack_Damage);//MaxHp-Damage
		DrawText(memDC, str, -1, &textect6, DT_LEFT);
	
}
void Pokemon_change(int MsgJudge, int Bag[])
{
	/*for (int i = 0; i < 4; i++)
	{
		if (MsgJudge == 0 && Bag[0] != 0)
		{
			int temp;
			temp = What_Poke;
			What_Poke = Bag[0];
			Bag[0] = temp;
			break;
		}
		else if (MsgJudge == 1 && Bag[1] != 0)
		{
			int temp;
			temp = What_Poke;
			What_Poke = Bag[1];
			Bag[1] = temp;
			break;
		}
		else if (MsgJudge == 2 && Bag[2] != 0)
		{
			int temp;
			temp = What_Poke;
			What_Poke = Bag[2];
			Bag[2] = temp;
			break;
		}
		else if (MsgJudge == 3 && Bag[3] != 0)
		{
			int temp;
			temp = What_Poke;
			What_Poke = Bag[3];
			Bag[3] = temp;
			break;
		}
	}*/
	What_Poke = Bag[MsgJudge];

}
void Catch_Pokemon(int count,int Respawn,Enemy_Info info[],FILE *f,int EnemyLv,int EnemyHp)
{
	if (count < 4)
	{
		f = fopen("My.txt", "w");
		if (count == 1)
		{
			fprintf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d\n", 1, my_info[1].poke, my_info[1].name, my_info[1].Level, my_info[1].Level * 10, my_info[1].Stack_Damage, my_info[1].Skill1, my_info[1].Skill2, my_info[1].Skill3, my_info[1].Skill4,my_info[1].Limit1, my_info[1].Limit2, my_info[1].Limit3, my_info[1].Limit4);
			if (state == 1)
				fprintf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d\n", 2, info[Respawn].poke, info[Respawn].name, EnemyLv, EnemyHp, 0, "몸통박치기", "할퀴기", "연속뺨치기", "째려보기", 15, 10, 5, 3);
			else if (state == 2)
				fprintf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d\n", 2, info[Respawn].poke, info[Respawn].name, EnemyLv, EnemyHp, 0, "몸통박치기", "할퀴기", "위압감주기", "파도타기", 15, 10, 5, 3);
			else if (state == 3)
				fprintf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d\n", 2, info[Respawn].poke, info[Respawn].name, EnemyLv, EnemyHp, 0, "몸통박치기", "할퀴기", "위압감주기", "땅흔들기", 15, 10, 5, 3);
			else if (state == 4)
				fprintf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d\n", 2, info[Respawn].poke, info[Respawn].name, EnemyLv, EnemyHp, 0, "몸통박치기", "할퀴기", "위압감주기", "백만볼트", 15, 10, 5, 3);
		}
		else if (count == 2)
		{
			for (int i = 1; i < 3; i++)
			{
				fprintf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d\n", i, my_info[i].poke, my_info[i].name, my_info[i].Level, my_info[i].Level * 10, my_info[i].Stack_Damage, my_info[i].Skill1, my_info[i].Skill2, my_info[i].Skill3, my_info[i].Skill4,my_info[i].Limit1, my_info[i].Limit2, my_info[i].Limit3, my_info[i].Limit4);
			}
			if (state == 1)
				fprintf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d\n", 3, info[Respawn].poke, info[Respawn].name, EnemyLv, EnemyHp, 0, "몸통박치기", "할퀴기", "연속뺨치기", "째려보기", 15, 10, 5, 3);
			else if (state == 2)
				fprintf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d\n", 3, info[Respawn].poke, info[Respawn].name, EnemyLv, EnemyHp, 0, "몸통박치기", "할퀴기", "위압감주기", "파도타기", 15, 10, 5, 3);
			else if (state == 3)
				fprintf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d\n", 3, info[Respawn].poke, info[Respawn].name, EnemyLv, EnemyHp, 0, "몸통박치기", "할퀴기", "위압감주기", "땅흔들기", 15, 10, 5, 3);
			else if (state == 4)
				fprintf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d\n", 3, info[Respawn].poke, info[Respawn].name, EnemyLv, EnemyHp, 0, "몸통박치기", "할퀴기", "위압감주기", "백만볼트", 15, 10, 5, 3);

		}
		else if (count == 3)
		{
			for (int i = 1; i < 4; i++)
			{
				fprintf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d\n", i, my_info[i].poke, my_info[i].name, my_info[i].Level, my_info[i].Level * 10, my_info[i].Stack_Damage, my_info[i].Skill1, my_info[i].Skill2, my_info[i].Skill3, my_info[i].Skill4, my_info[i].Limit1, my_info[i].Limit2, my_info[i].Limit3, my_info[i].Limit4);
			}
			if (state == 1)
				fprintf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d\n", 4, info[Respawn].poke, info[Respawn].name, EnemyLv, EnemyHp, 0, "몸통박치기", "할퀴기", "연속뺨치기", "째려보기", 15, 10, 5, 3);
			else if (state == 2)
				fprintf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d\n", 4, info[Respawn].poke, info[Respawn].name, EnemyLv, EnemyHp, 0, "몸통박치기", "할퀴기", "위압감주기", "파도타기", 15, 10, 5, 3);
			else if (state == 3)
				fprintf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d\n", 4, info[Respawn].poke, info[Respawn].name, EnemyLv, EnemyHp, 0, "몸통박치기", "할퀴기", "위압감주기", "땅흔들기", 15, 10, 5, 3);
			else if (state == 4)
				fprintf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d\n", 4, info[Respawn].poke, info[Respawn].name, EnemyLv, EnemyHp, 0, "몸통박치기", "할퀴기", "위압감주기", "백만볼트", 15, 10, 5, 3);

		}

		fclose(f);
		f = fopen("My.txt", "r");
		for (int i = 1; i < 6; i++)
		{
			fscanf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d", &my_info[i].What_Poke, &my_info[i].poke, my_info[i].name,
				&my_info[i].Level, &my_info[i].MaxHp, &my_info[i].Stack_Damage, my_info[i].Skill1, my_info[i].Skill2, my_info[i].Skill3,
				my_info[i].Skill4, &my_info[i].Limit1, &my_info[i].Limit2, &my_info[i].Limit3, &my_info[i].Limit4);
		}
		fclose(f);
	}
}
void Cure_Pokemon(void)
{
	int count = 0;
	for (int i = 0; i < 4; i++)
	{
		if (Bag[i] != 0)
		{
			count++;
		}
	}
	for (int i = 1; i <= count; i++)
	{
		my_info[i].Stack_Damage = 0;
		my_info[i].Limit1 = 15;
		my_info[i].Limit2 = 10;
		my_info[i].Limit3 = 5;
		my_info[i].Limit4 = 3;
	}
}
void createmap(int block[][50], int npc[][50], int map, int *maptype, int *mapx, int *mapy, int *mapsx, int *mapsy, double *dx, double *dy, RECT rectView)
{
	for (int y = 0; y < 50; y++)
	{
		for (int x = 0; x < 50; x++)
		{
			npc[y][x] = 0;
		}
	}
	if (map == 0)
	{
		PlayBgm(1);
		*mapx = 30;
		*mapy = 30;
		*maptype = 1;
		*mapsx = 7;
		*mapsy = 7;
		*dx = rectView.right / ((*mapsx) * 2);
		*dy = rectView.bottom / ((*mapsy) * 2);
		npc[11][2] = 1;
		npc[9][12] = 2;
		npc[16][18] = 3;
		npc[13][27] = 4;
		npc[20][7] = 5;
		for (int y = 0; y < *mapy; y++)
		{
			for (int x = 0; x < *mapx; x++)
			{
				if (y >= 0 && y <= 5)
				{
					block[y][x] = 0;
				}
				else if (y >= 6 && y <= 8 && (x<4 || x>9))
				{
					block[y][x] = 0;
				}
				else if (y == 9 && (x<4 || (x>8 && x<13) || (x>17)))
				{
					block[y][x] = 0;
				}
				else if (y == 10 && (x<3 || (x == 17) || (x>23)))
				{
					block[y][x] = 0;
				}
				else if (y == 11 && (x<3 || (x>23)))
				{
					block[y][x] = 0;
				}
				else if (y == 12 && x>23)
				{
					block[y][x] = 0;
				}
				else if (y == 13 && ((x>18 && x <23) || (x>26)))
				{
					block[y][x] = 0;
				}
				else if (y == 14 && (x <9 || (x>18 && x <23) || (x>27)))
				{
					block[y][x] = 0;
				}
				else if (y == 15 && (x <9 || (x>17 && x <23) || (x>27)))
				{
					block[y][x] = 0;
				}
				else if (y == 16 && (x <13 || (x>16 && x <23)))
				{
					block[y][x] = 0;
				}
				else if (y >= 17 && y <= 19 && (x <13))
				{
					block[y][x] = 0;
				}
				else if (y == 20 && (x <8 || x>17))
				{
					block[y][x] = 0;
				}
				else if (y >= 21 && y <= 23 && (x <7 || x>17))
				{
					block[y][x] = 0;
				}
				else if (y == 24 && (x <14 || x>18))
				{
					block[y][x] = 0;
				}
				else if (y >= 25 && y <= 27 && (x <14 || x>24))
				{
					block[y][x] = 0;
				}
				else if (y >= 28)
				{
					block[y][x] = 0;
				}
				else
				{
					block[y][x] = 1;
				}
			}
		}
	}
	else if (map == 1)
	{
		for (int y = 0; y < 30; y++)
		{
			for (int x = 0; x < 30; x++)
			{
				block[y][x] = 0;
			}
		}
		*mapx = 11;
		*mapy = 9;
		*maptype = 0;
		*dx = rectView.right / ((*mapx));
		*dy = rectView.bottom / ((*mapy));
		npc[6][2] = 10;
		for (int y = 0; y < *mapy; y++)
		{
			for (int x = 0; x < *mapx; x++)
			{
				if (y == 0 || y == 1)
				{
					block[y][x] = 0;
				}
				else if (y == 2 && (x != 5 && x != 6))
				{
					block[y][x] = 0;
				}
				else if (y == 4 && (x >= 2 && x <= 4))
				{
					block[y][x] = 0;
				}
				else if (y == 6 && (x >= 2 && x <= 4))
				{
					block[y][x] = 0;
				}
				else if (y == 7 && (x >= 3 && x <= 4))
				{
					block[y][x] = 0;
				}
				else
				{
					block[y][x] = 1;
				}
			}
		}
	}
	else if (map == 14)
	{
		for (int y = 0; y < 30; y++)
		{
			for (int x = 0; x < 30; x++)
			{
				block[y][x] = 0;
			}
		}
		*mapx = 9;
		*mapy = 8;
		*maptype = 0;
		*dx = rectView.right / ((*mapx));
		*dy = rectView.bottom / ((*mapy));
		npc[1][3] = 11;
		for (int y = 0; y < *mapy; y++)
		{
			for (int x = 0; x < *mapx; x++)
			{
				if (y == 0 || y == 1)
				{
					block[y][x] = 0;
				}
				else if (y == 5 && x == 1)
				{
					block[y][x] = 0;
				}
				else
				{
					block[y][x] = 1;
				}
			}
		}
	}
	else if (map == 2)
	{
		PlayBgm(2);
		for (int y = 0; y < 30; y++)
		{
			for (int x = 0; x < 30; x++)
			{
				block[y][x] = 0;
			}
		}
		*mapx = 14;
		*mapy = 9;
		*maptype = 0;
		*dx = rectView.right / ((*mapx));
		*dy = rectView.bottom / ((*mapy));
		npc[3][7] = 7;
		for (int y = 0; y < *mapy; y++)
		{
			for (int x = 0; x < *mapx; x++)
			{
				if (y == 0 || y == 1)
				{
					block[y][x] = 0;
				}
				else if ((y == 2 || y == 3) && x >= 4 && x <= 9)
				{
					block[y][x] = 0;
				}
				else if ((y == 6 || y == 7) && x >= 11 && x <= 12)
				{
					block[y][x] = 0;
				}
				else if (y == 8 && (x == 0 || x == 13))
				{
					block[y][x] = 0;
				}
				else
				{
					block[y][x] = 1;
				}
			}
		}
	}
	else if (map == 3)
	{
		PlayBgm(3);
		*mapx = 11;
		*mapy = 8;
		*maptype = 0;
		*dx = rectView.right / ((*mapx));
		*dy = rectView.bottom / ((*mapy));
		npc[4][1] = 8;
		for (int y = 0; y < 30; y++)
		{
			for (int x = 0; x < 30; x++)
			{
				block[y][x] = 0;
			}
		}
		for (int y = 0; y < *mapy; y++)
		{
			for (int x = 0; x < *mapx; x++)
			{
				if (y >= 0 && y <= 1)
				{
					block[y][x] = 0;
				}
				else if (y == 2 && (x<3 || x>5))
				{
					block[y][x] = 0;
				}
				else if (y == 3 && (x<3 || x>9))
				{
					block[y][x] = 0;
				}
				else if (y == 4 && (x<3 || (x>5 && x< 8) || x>9))
				{
					block[y][x] = 0;
				}
				else if (y >= 5 && y <= 6 && ((x>5 && x< 8) || x>9))
				{
					block[y][x] = 0;
				}
				else
				{
					block[y][x] = 1;
				}
			}
		}
	}
	else if (map == 4)
	{
		*mapx = 13;
		*mapy = 13;
		*maptype = 0;
		*dx = rectView.right / ((*mapx));
		*dy = rectView.bottom / ((*mapy));
		npc[4][8] = 6;
		for (int y = 0; y < 30; y++)
		{
			for (int x = 0; x < 30; x++)
			{
				block[y][x] = 0;
			}
		}
		for (int y = 0; y < *mapy; y++)
		{
			for (int x = 0; x < *mapx; x++)
			{
				if (y >= 0 && y <= 2)
				{
					block[y][x] = 0;
				}
				else if (y == 3 && (x == 0 || (x> 7 && x<11) || x == 12))
				{
					block[y][x] = 0;
				}
				else if (y == 4 && (x> 7 && x<11))
				{
					block[y][x] = 0;
				}
				else if (y == 5 && (x< 4))
				{
					block[y][x] = 0;
				}
				else if (y >= 6 && y <= 7 && (x< 4 || x>9))
				{
					block[y][x] = 0;
				}
				else if (y == 9 && (x == 0 || x>10))
				{
					block[y][x] = 0;
				}
				else if (y >= 10 && y <= 11 && (x < 2 || x>10))
				{
					block[y][x] = 0;
				}
				else if (y >= 12 && (x == 3))
				{
					block[y][x] = 0;
				}
				else
				{
					block[y][x] = 1;
				}
			}
		}
	}
	else if (map == 5)
	{
		*mapx = 20;
		*mapy = 10;
		*maptype = 0;
		*dx = rectView.right / ((*mapx));
		*dy = rectView.bottom / ((*mapy));
		npc[6][11] = 9;
		PlayBgm(12);
		for (int y = 0; y < 30; y++)
		{
			for (int x = 0; x < 30; x++)
			{
				block[y][x] = 0;
			}
		}
		for (int y = 0; y < *mapy; y++)
		{
			for (int x = 0; x < *mapx; x++)
			{
				if (y >= 0 && y <= 3)
				{
					block[y][x] = 0;
				}
				else if (y >= 4 && y <= 5 && (x < 6 || x > 11))
				{
					block[y][x] = 0;
				}
				else if (y == 6 && (x < 3 || x >= 11))
				{
					block[y][x] = 0;
				}
				else if (y == 7 && (x < 4 || (x > 4 && x<6) || x > 11))
				{
					block[y][x] = 0;
				}
				else if (y >= 8 && y <= 9 && (x != 4))
				{
					block[y][x] = 0;
				}
				else
				{
					block[y][x] = 1;
				}
			}
		}
	}
	else if (map == 6)
	{
		PlayBgm(4);
		*mapx = 48;
		*mapy = 44;
		*maptype = 1;
		*mapsx = 7;
		*mapsy = 7;
		*dx = rectView.right / ((*mapsx) * 2);
		*dy = rectView.bottom / ((*mapsy) * 2);
		for (int y = 0; y < *mapy; y++)
		{
			for (int x = 0; x < *mapx; x++)
			{
				if (y >= 0 && y <= 3 && (x < 14 || x>15))
				{
					block[y][x] = 0;
				}
				else if (y >= 4 && y <= 5 && (x < 14 || (x > 15 && x < 32) || (x > 43)))
				{
					block[y][x] = 0;
				}
				else if (y >= 6 && y <= 7 && ((x < 12) || (x > 17 && x < 26) || (x > 26 && x < 31) || (x > 36 && x < 41) || x > 45))
				{
					block[y][x] = 0;
				}
				else if (y == 8 && ((x < 4) || (x > 17 && x < 24) || (x > 36)))
				{
					block[y][x] = 0;
				}
				else if (y == 9 && ((x < 4) || (x > 17 && x < 24) || (x > 36 && x < 41) || (x>45)))
				{
					block[y][x] = 0;
				}
				else if (y >= 10 && y <= 11 && ((x < 4) || (x > 27 && x < 34)))
				{
					block[y][x] = 0;
				}
				else if (y >= 12 && y <= 13 && ((x < 4) || (x > 7 && x < 14) || (x > 17)))
				{
					block[y][x] = 0;
				}
				else if (y >= 14 && y <= 15)
				{
					block[y][x] = 0;
				}
				else
				{
					block[y][x] = 1;
				}
			}
		}
	}
	else if (map == 7)
	{
		PlayBgm(5);
		*mapx = 27;
		*mapy = 30;
		*maptype = 1;
		*mapsx = 7;
		*mapsy = 7;
		*dx = rectView.right / ((*mapsx) * 2);
		*dy = rectView.bottom / ((*mapsy) * 2);
		npc[25][9] = 12;
		npc[25][10] = 12;
		npc[25][11] = 12;
		for (int y = 0; y < *mapy; y++)
		{
			for (int x = 0; x < *mapx; x++)
			{
				if (y >= 0 && y <= 1)
				{
					block[y][x] = 0;
				}
				else if (y >= 2 && y <= 3 && (x < 10 || (x > 22)))
				{
					block[y][x] = 0;
				}
				else if (y == 4 && ((x < 9) || (x > 22)))
				{
					block[y][x] = 0;
				}
				else if (y >= 5 && y <= 8 && ((x < 9) || (x > 15)))
				{
					block[y][x] = 0;
				}
				else if (y == 9 && x != 14)
				{
					block[y][x] = 0;
				}
				else if (y == 10 && ((x < 9) || (x > 14)))
				{
					block[y][x] = 0;
				}
				else if (y == 11 && ((x < 8) || (x > 14)))
				{
					block[y][x] = 0;
				}
				else if (y == 12 && ((x < 7) || (x > 17)))
				{
					block[y][x] = 0;
				}
				else if (y == 13 && ((x < 12) || (x > 17)))
				{
					block[y][x] = 0;
				}
				else if (y == 14 && ((x < 14) || (x > 18)))
				{
					block[y][x] = 0;
				}
				else if (y == 15 && ((x < 16) || (x > 18)))
				{
					block[y][x] = 0;
				}
				else if (y == 16 && ((x < 17) || (x > 20)))
				{
					block[y][x] = 0;
				}
				else if (y >= 17 && y <= 18 && ((x < 18) || (x > 20)))
				{
					block[y][x] = 0;
				}
				else if (y >= 19 && y <= 24 && ((x < 18) || (x > 22)))
				{
					block[y][x] = 0;
				}
				else if (y == 25 && ((x < 13) || (x > 22)))
				{
					block[y][x] = 0;
				}
				else if (y == 26 && ((x < 6) || (x > 20)))
				{
					block[y][x] = 0;
				}
				else if (y == 27 && ((x < 8) || (x > 20)))
				{
					block[y][x] = 0;
				}
				else if (y == 28)
				{
					block[y][x] = 0;
				}
				else
				{
					block[y][x] = 1;
				}
			}
		}
	}
	else if (map == 8)
	{
		PlayBgm(7);
		*mapy = 50;
		*mapx = 37;
		*maptype = 1;
		*mapsx = 7;
		*mapsy = 5;
		*dx = rectView.right / ((*mapsx) * 2);
		*dy = rectView.bottom / ((*mapsy) * 2);

		for (int y = 0; y < (*mapy); y++)
		{
			for (int x = 0; x < (*mapx); x++)
			{
				if (y >= 0 && y <= 18 && (x < 19 || x>20))
				{
					block[y][x] = 0;
				}
				else if (y >= 19 && y <= 20 && (x < 19 || x>26))
				{
					block[y][x] = 0;
				}
				else if (y >= 21 && y <= 23 && (x < 24 || x>26))
				{
					block[y][x] = 0;
				}
				else if (y >= 24 && y <= 25 && (x < 23 || x>27))
				{
					block[y][x] = 0;
				}
				else if (y == 26 && (x < 22 || x>27))
				{
					block[y][x] = 0;
				}
				else if (y == 27 && (x < 8 || x>27))
				{
					block[y][x] = 0;
				}
				else if (y == 28 && (x < 8 || x>24))
				{
					block[y][x] = 0;
				}
				else if (y >= 29 && y <= 33 && (x < 9 || x>10))
				{
					block[y][x] = 0;
				}
				else if (y == 34 && (x < 8 || x>11))
				{
					block[y][x] = 0;
				}
				else if (y == 35 && (x < 8 || x>21))
				{
					block[y][x] = 0;
				}
				else if (y == 36 && (x < 10 || x>30))
				{
					block[y][x] = 0;
				}
				else if (y == 37 && (x < 14 || x>30))
				{
					block[y][x] = 0;
				}
				else if (y >= 38 && y <= 50 && (x < 27 || x>29))
				{
					block[y][x] = 0;
				}
				else
				{
					block[y][x] = 1;
				}
			}
		}
	}
	else if (map == 9)
	{
		PlayBgm(7);
		*mapy = 37;
		*mapx = 50;
		*maptype = 1;
		*mapsx = 7;
		*mapsy = 5;
		*dx = rectView.right / ((*mapsx) * 2);
		*dy = rectView.bottom / ((*mapsy) * 2);
		npc[4][22] = 14;
		npc[4][23] = 14;
		npc[4][24] = 14;
		for (int y = 0; y < (*mapy); y++)
		{
			for (int x = 0; x < (*mapx); x++)
			{
				if (y >= 0 && y <= 4)
				{
					block[y][x] = 0;
				}
				else if (y >= 5 && y <= 6 && (x < 19 || x>27))
				{
					block[y][x] = 0;
				}
				else if (y == 7 && (x < 21 || x>25))
				{
					block[y][x] = 0;
				}
				else if (y >= 8 && y <= 14 && (x < 22 || x>24))
				{
					block[y][x] = 0;
				}
				else if (y >= 15 && y <= 16 && (x < 21 || x>25))
				{
					block[y][x] = 0;
				}
				else if (y == 17 && (x < 22 || x>24))
				{
					block[y][x] = 0;
				}
				else if (y >= 18 && y <= 30 && (x < 21 || x>25))
				{
					block[y][x] = 0;
				}
				else if (y >= 31 && (x < 22 || x>24))
				{
					block[y][x] = 0;
				}
				else
				{
					block[y][x] = 1;
				}
			}
		}
	}
	else if (map == 10)
	{
		PlayBgm(4);
		*mapy = 40;
		*mapx = 40;
		*maptype = 1;
		*mapsx = 10;
		*mapsy = 6;
		*dx = rectView.right / ((*mapsx) * 2);
		*dy = rectView.bottom / ((*mapsy) * 2);

		for (int y = 0; y < (*mapy); y++)
		{
			for (int x = 0; x < (*mapx); x++)
			{
				if (y >= 0 && y <= 4)
				{
					block[y][x] = 0;
				}
				else if (y == 5 && (x < 20 || x > 21))
				{
					block[y][x] = 0;
				}
				else if (y >= 6 && y <= 7 && (x < 2 || (x>7 && x<20) || (x>21)))
				{
					block[y][x] = 0;
				}
				else if (y >= 8 && y <= 9 && (x < 2 || (x>10 && x<20) || (x>21)))
				{
					block[y][x] = 0;
				}
				else if (y == 10 && (x < 2 || (x>14 && x<20) || (x>21)))
				{
					block[y][x] = 0;
				}
				else if (y == 11 && (x < 2 || (x>14 && x<18) || (x>21)))
				{
					block[y][x] = 0;
				}
				else if (y == 12 && (x == 17 || (x>21)))
				{
					block[y][x] = 0;
				}
				else if (y == 13 && (x == 17 || (x>23)))
				{
					block[y][x] = 0;
				}
				else if (y == 14 && (x < 6 || (x>10 && x<13) || (x>14 && x<18) || (x>37)))
				{
					block[y][x] = 0;
				}
				else if (y == 15 && (x < 6 || (x>10 && x<12) || (x>37)))
				{
					block[y][x] = 0;
				}
				else if (y >= 16 && y <= 19 && (x < 12 || (x>37)))
				{
					block[y][x] = 0;
				}
				else if (y >= 20 && y <= 21 && (x < 17 || (x>35)))
				{
					block[y][x] = 0;
				}
				else if (y >= 22 && y <= 24 && (x < 24 || (x>35)))
				{
					block[y][x] = 0;
				}
				else if (y == 25)
				{
					block[y][x] = 0;
				}
				else
				{
					block[y][x] = 1;
				}
			}
		}
	}
	else if (map == 11)
	{
		PlayBgm(6);
		*mapy = 38;
		*mapx = 35;
		*maptype = 1;
		*mapsx = 7;
		*mapsy = 7;
		*dx = rectView.right / ((*mapsx) * 2);
		*dy = rectView.bottom / ((*mapsy) * 2);

		for (int y = 0; y < (*mapy); y++)
		{
			for (int x = 0; x < (*mapx); x++)
			{
				if (y >= 0 && y <= 1)
				{
					block[y][x] = 0;
				}
				else if (y == 2 && (x<19 || x>26))
				{
					block[y][x] = 0;
				}
				else if (y >= 3 && y <= 4 && (x<19 || x>27))
				{
					block[y][x] = 0;
				}
				else if (y == 5 && (x<18 || x>21))
				{
					block[y][x] = 0;
				}
				else if (y >= 6 && y <= 17 && (x<18 || x>20))
				{
					block[y][x] = 0;
				}
				else if (y >= 18 && y <= 19 && (x<18 || x>21))
				{
					block[y][x] = 0;
				}
				else if (y == 20 && (x<19 || x>26))
				{
					block[y][x] = 0;
				}
				else if (y >= 21 && y <= 22 && (x<19 || x>27))
				{
					block[y][x] = 0;
				}
				else if (y >= 23 && y <= 25 && (x<24 || x>27))
				{
					block[y][x] = 0;
				}
				else if (y >= 26 && y <= 36 && (x<25 || x>27))
				{
					block[y][x] = 0;
				}
				else if (y == 37)
				{
					block[y][x] = 0;
				}
				else
				{
					block[y][x] = 1;
				}
			}
		}
	}
	else if (map == 12)
	{
		PlayBgm(6);
		*mapy = 45;
		*mapx = 30;
		*maptype = 1;
		*mapsx = 7;
		*mapsy = 5;
		*dx = rectView.right / ((*mapsx) * 2);
		*dy = rectView.bottom / ((*mapsy) * 2);

		for (int y = 0; y < (*mapy); y++)
		{
			for (int x = 0; x < (*mapx); x++)
			{
				if (y >= 0 && y <= 3 && (x < 12 || x>15))
				{
					block[y][x] = 0;
				}
				else if (y >= 4 && y <= 18 && (x < 13 || x>14))
				{
					block[y][x] = 0;
				}
				else if (y >= 19 && y <= 20 && (x < 10 || x>22))
				{
					block[y][x] = 0;
				}
				else if (y == 21 && (x < 10 || x>19))
				{
					block[y][x] = 0;
				}
				else if (y == 22 && (x < 11 || x>19))
				{
					block[y][x] = 0;
				}
				else if (y == 23)
				{
					block[y][x] = 0;
				}
				else
				{
					block[y][x] = 1;
				}
			}
		}
	}
	else if (map == 13)
	{
		PlayBgm(6);
		*mapy = 47;
		*mapx = 40;
		*maptype = 1;
		*mapsx = 7;
		*mapsy = 5;
		*dx = rectView.right / ((*mapsx) * 2);
		*dy = rectView.bottom / ((*mapsy) * 2);
		npc[15][19] = 13;
		npc[15][20] = 13;
		npc[15][21] = 13;
		for (int y = 0; y < (*mapy); y++)
		{
			for (int x = 0; x < (*mapx); x++)
			{
				if (y >= 0 && y <= 14)
				{
					block[y][x] = 0;
				}
				else if (y == 15 && (x < 17 || x>22))
				{
					block[y][x] = 0;
				}
				else if (y >= 16 && y <= 21 && (x < 17 || x>25))
				{
					block[y][x] = 0;
				}
				else if (y == 22 && (x != 25))
				{
					block[y][x] = 0;
				}
				else if (y == 23 && (x < 19 || x > 25))
				{
					block[y][x] = 0;
				}
				else if (y == 24 && (x != 20))
				{
					block[y][x] = 0;
				}
				else if (y == 25 && (x < 20 || x > 26))
				{
					block[y][x] = 0;
				}
				else if (y == 26 && (x < 22 || x > 26))
				{
					block[y][x] = 0;
				}
				else if (y >= 27 && y <= 32 && (x != 26))
				{
					block[y][x] = 0;
				}
				else if (y >= 32 && y <= 38 && (x < 19 || x>27))
				{
					block[y][x] = 0;
				}
				else if (y >= 39 && y <= 40 && (x < 19 || x>22))
				{
					block[y][x] = 0;
				}
				else if (y >= 41 && y <= 42 && (x < 20 || x>21))
				{
					block[y][x] = 0;
				}
				else if (y == 43)
				{
					block[y][x] = 0;
				}
				else
				{
					block[y][x] = 1;
				}
			}
		}
	}
}

void movemap(int *cx, int *cy, int block[][50], int npc[][50], int *map, int *maptype, int *mapx, int *mapy, int *mapsx, int *mapsy, double *dx, double *dy, RECT rectView)
{
	if (*map == 0)
	{
		if (*cx == 20 && *cy == 17)
		{
			*map = 2;
			*cx = 6;
			*cy = 8;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
		else if (*cx == 20 && *cy == 25)
		{
			*map = 1;
			*cx = 8;
			*cy = 8;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
		else if (*cx == 25 && *cy == 13)
		{
			PlayBgm(3);
			*map = 3;
			*cx = 3;
			*cy = 7;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
		else if (*cx == 7 && *cy == 6)
		{
			*map = 4;
			*cx = 6;
			*cy = 12;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
		else if (*cx == 15 && *cy == 9)
		{
			*map = 5;
			*cx = 4;
			*cy = 9;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
		else if (*cx == 0 && (*cy >= 12 || *cy <= 13))
		{
			*map = 6;
			*cx = 47;
			*cy = 10;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
		else if (*cx == 29 && (*cy >= 16 || *cy <= 19))
		{
			*map = 10;
			*cx = 0;
			*cy = 12;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
	}
	else if (*map == 14)
	{
		if (*cx == 7 && *cy == 2)
		{
			*map = 1;
			*cx = 8;
			*cy = 3;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
	}
	else if (*map == 1)
	{
		if (*cx == 8 && *cy == 8)
		{
			*map = 0;
			*cx = 20;
			*cy = 25;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
		else if (*cx == 8 && *cy == 3)
		{
			*map = 14;
			*cx = 7;
			*cy = 2;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
	}
	else if (*map == 2)
	{
		if (*cx >= 6 && *cx <= 7 && *cy == 8)
		{
			*map = 0;
			*cx = 20;
			*cy = 17;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
	}
	else if (*map == 3)
	{
		if (*cx >= 3 && *cx <= 4 && *cy == 7)
		{
			*map = 0;
			*cx = 25;
			*cy = 13;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
	}
	else if (*map == 4)
	{
		if (*cx >= 6 && *cx <= 7 && *cy == 12)
		{
			*map = 0;
			*cx = 7;
			*cy = 6;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
	}
	else if (*map == 5)
	{
		if (*cx == 4 && *cy == 9)
		{
			*map = 0;
			*cx = 15;
			*cy = 9;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
	}
	else if (*map == 6)
	{
		if (*cx == 47 && (*cy >= 10 || *cy <= 11))
		{
			*map = 0;
			*cx = 0;
			*cy = 12;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
		else if (*cy == 0 && (*cx >= 14 || *cx <= 15))
		{
			*map = 8;
			*cx = 27;
			*cy = 49;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
		else if (*cy == 8 && *cx == 6)
		{
			*map = 7;
			*cx = 20;
			*cy = 4;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
	}
	else if (*map == 7)
	{
		if (*cx == 20 && *cy == 4)
		{
			*map = 6;
			*cx = 6;
			*cy = 8;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
	}
	else if (*map == 8)
	{
		if ((*cx >= 27 || *cx <= 29) && *cy == 49)
		{
			*map = 6;
			*cx = 14;
			*cy = 0;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
		else if ((*cx >= 19 || *cx <= 20) && *cy == 0)
		{
			*map = 9;
			*cx = 22;
			*cy = 36;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
	}
	else if (*map == 9)
	{
		if ((*cx >= 22 || *cx <= 24) && *cy == 36)
		{
			*map = 8;
			*cx = 19;
			*cy = 0;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
	}
	else if (*map == 10)
	{
		if ((*cy >= 12 || *cy <= 13) && *cx == 0)
		{
			*map = 0;
			*cx = 29;
			*cy = 16;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
		else if (*cx == 20 && *cy == 5)
		{
			*map = 11;
			*cx = 26;
			*cy = 36;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
	}
	else if (*map == 11)
	{
		if (*cy == 36 && *cx == 26)
		{
			*map = 10;
			*cx = 20;
			*cy = 5;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
		else if (*cx == 26 && *cy == 4)
		{
			*map = 12;
			*cx = 16;
			*cy = 19;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
	}
	else if (*map == 12)
	{
		if (*cy == 19 && *cx == 16)
		{
			*map = 11;
			*cx = 26;
			*cy = 4;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
		else if ((*cx >= 12 || *cx <= 15) && *cy == 0)
		{
			*map = 13;
			*cx = 20;
			*cy = 42;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
	}
	else if (*map == 13)
	{
		if (*cy == 42 && (*cx >= 20 || *cx <= 21))
		{
			*map = 12;
			*cx = 13;
			*cy = 0;
			createmap(block, npc, *map, maptype, mapx, mapy, mapsx, mapsy, dx, dy, rectView);
		}
	}
}

void Save_Pokemon_info(int count,BOOL Dead)
{
	FILE *f;
	f = fopen("My.txt", "w");
	for (int i = 1; i <= count; i++)
	{
		if (Dead == FALSE)
			my_info[i].Level++;
		
		
		fprintf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d\n", i, my_info[i].poke, my_info[i].name, my_info[i].Level, my_info[i].Level * 10, my_info[i].Stack_Damage, my_info[i].Skill1, my_info[i].Skill2, my_info[i].Skill3, my_info[i].Skill4, my_info[i].Limit1, my_info[i].Limit2, my_info[i].Limit3, my_info[i].Limit4);
	}
	fclose(f);
}
bool childe = false;
HWND child_hWnd;

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR IpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hinst = hinstance;
	//윈도우 클래스 구조체 값 설정
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hinstance;
	WndClass.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON1));
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	WndClass.lpszMenuName = NULL;
	//WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON1));

	//윈도우 클래스 등록
	RegisterClassEx(&WndClass);

	WndClass.hCursor = LoadCursor(NULL, IDC_HELP);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	WndClass.lpszClassName = "Child";   // 차일드 윈도우 클래스 이름 
	WndClass.lpfnWndProc = ChildProc;   // 차일드 윈도우 프로시저 지정 
	RegisterClassEx(&WndClass);
	
	WndClass.hCursor = LoadCursor(NULL, IDC_HELP);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	WndClass.lpszClassName = "Battle";   // 차일드 윈도우 클래스 이름 
	WndClass.lpfnWndProc = BattleProc;   // 차일드 윈도우 프로시저 지정 
	RegisterClassEx(&WndClass);
	
	//윈도우 생성
	hWnd = CreateWindow(lpszClass, "Pokemon", WS_OVERLAPPEDWINDOW, 0, 0, 1024, 768, NULL, (HMENU)NULL, hinstance, NULL);
	//윈도우 출력
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	//이벤트 루프 처리
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}


BOOL CALLBACK Dlg6_1Proc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg) {
	case WM_INITDIALOG:
		char str[12];
		wsprintf(str, "%d", Potion);
		SetDlgItemText(hDlg, IDC_EDIT1, str);
		wsprintf(str, "%d", Ball);
		SetDlgItemText(hDlg, IDC_EDIT2, str);
		wsprintf(str, "%d", HiperBall);
		SetDlgItemText(hDlg, IDC_EDIT3, str);
		return true;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			TalkBox_Judge = 8;
			EnemyTurn = TRUE;
			Potion--;
			my_info[What_Poke].Stack_Damage -= 300;
			if (my_info[What_Poke].Stack_Damage < 0)my_info[What_Poke].Stack_Damage = 0;
			EndDialog(hDlg, 1);
			break;
		case IDC_BUTTON2:
			if (state != 5)
			{
				Show_Ball_Throw = 1;
				TalkBox_Judge = 11;
				EndDialog(hDlg, 1);
			}
			break;
		case IDC_BUTTON3:
			if (state != 5)
			{
				Catch = TRUE;
				Show_Ball_Throw = 1;
				TalkBox_Judge = 11;
				HiperBall--;
				EndDialog(hDlg, 1);
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			break;
		}
		break;
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)
{
	FILE *f;
	PAINTSTRUCT ps;
	static CImage icon[5];
	static CImage effect[6];
	static CImage hero[13];
	static CImage background[15];
	static CImage bDialog;
	static CImage bmsel[5];
	HFONT Font, OldFont;
	static LOGFONT lf;
	static RECT rectView;
	static HDC hDC, memDC;
	static double dx, dy;//한칸 폭
	static bool moving = false;
	static bool fade = false;
	static bool poke = false;
	static int dial = 0;
	static int msel = -1;
	static int sbag = -1;
	static int wmap = -1;
	static int starting = 0;
	static char osstr[11][120] = {
		{ "" },
		{ "포켓몬스터의 세계에\n잘왔단다!" },//1
		{ "나의 이름은 오박사" },//2
		{ "모두로부터는 포켓몬박사라고\n존경받고 있단다." },//3
		{ "포켓몬스터... 포켓몬" },//4
		{ "이 세계에는\n포켓몬스터라고 불려지는" },//5
		{ "생명체들이\n도처에 살고있다!" },//6
		{ "하지만 우리는 포켓몬 전부를\n알고 있지는 못하다." },//7
		{ "포켓몬의 비밀은\n아직도 잔뜩 있다!" },//8
		{ "그것을 밝혀내기 위하여\n너에게 포켓몬을 주마!" },//9
		{ "드디어 이제부터\n너의 이야기가 시작되어진다." }//10
	};
	static char dstr[15][120] = {
		{ "" },
		{ "포켓몬은 풀숲에서만\n나오는게 아니야!" },//1
		{ "관장을 이기고 싶으면\n전설 포켓몬3마리정도는 있어야지!" },//2
		{ "포켓몬 센터에서 포켓몬을\n회복시킬 수 있어" },//3
		{ "그란돈이\n용암호수에 나타났다구나" },//4
		{ "호수 있는 동굴에\n가이오가가 살고있대!" },//5
		{ "포켓몬 하고\n많이 친해졌니?" },//6
		{ "포켓몬을 회복시키겠습니다." },//7
		{ "어서오세요." },//8
		{ "오호 나한테 도전하는 거냐?" },//9
		{ "과제는 다 하고 노는거니?" },//10
		{ "Inversus 게임을 켰다." }, //11
		{ "쿠오아왕!!!!!!!!" },//12
		{ "...!!!!!!!!" },//13
		{ "...!!!!!!!!" }//14
	};
	static int cx = 1, cy = 4;
	static int hx, hy;
	static int direct = 3;
	static int mcount = 0;
	static int map = 14;
	static int maptype = 0;
	static int mapx;
	static int mapy;
	static int mapsx;
	static int mapsy;
	static int block[50][50] = { 0, };//보드
	static int npc[50][50] = { 0, };
	switch (iMessage) {
	case WM_CREATE:
	{
		srand((unsigned)time(NULL));
		ZeroMemory(&lf, sizeof(lf));
		GetClientRect(hWnd, &rectView);
		Init();
		char btr[128];
		for (int i = 0; i < 12; i++)
		{
			wsprintf(btr, "img\\hero%d.png", i);
			hero[i].Load(TEXT(btr));
		}
		createmap(block, npc, map, &maptype, &mapx, &mapy, &mapsx, &mapsy, &dx, &dy, rectView);

		background[0].Load("img\\town2.dib");
		background[1].Load("img\\home1.dib");
		background[2].Load("img\\center.dib");
		background[3].Load("img\\shop.dib");
		background[4].Load("img\\lab.dib");
		background[5].Load("img\\gym.dib");
		background[6].Load("img\\left2.dib");
		background[7].Load("img\\gio.dib");
		background[8].Load("img\\grab.dib");
		background[9].Load("img\\recu.dib");
		background[10].Load("img\\right2.dib");
		background[11].Load("img\\cave.dib");
		background[12].Load("img\\caveout.dib");
		background[13].Load("img\\gra.dib");
		background[14].Load("img\\home0.jpg");

		for (int i = 0; i < 3; i++)
		{
			wsprintf(btr, "battle%d.jpg", i);
			battle[i].Load(TEXT(btr));
		}
		for (int i = 1; i < 50; i++)
		{
			wsprintf(btr, "Skill\\%d.png", i);
			Skill[i].Load(TEXT(btr));
		}

		for (int i = 1; i < 400; i++)
		{
			wsprintf(btr, "pokemon\\%d.png", i);
			pokemon[i].Load(TEXT(btr));
			wsprintf(btr, "back\\%d.png", i);
			backpoke[i].Load(TEXT(btr));
		}
		for (int i = 1; i < 20; i++)
		{
			wsprintf(btr, "Throw\\%d.png", i);
			Ball_Throw[i].Load(TEXT(btr));
		}
		for (int i = 0; i < 6; i++)
		{
			wsprintf(btr, "img\\menu%d.png", i);
			bmsel[i].Load(TEXT(btr));
		}

		for (int i = 0; i < 3; i++)
		{
			wsprintf(btr, "img\\bag%d.png", i);
			mbag[i].Load(TEXT(btr));
		}
		for (int i = 0; i < 9; i++) {
			wsprintf(btr, "img\\worldmap%d.png", i);
			bwmap[i].Load(TEXT(btr));
		}
		bPro.Load("img\\Professor.png");
		Exchange_Screen.Load(TEXT("Screen.png"));
		bSelect.Load(TEXT("Select.png"));
		bDialog.Load("img\\Dialogue.png");

		hWndAVI = MCIWndCreate(hWnd, g_hinst, MCIWNDF_NOTIFYANSI | MCIWNDF_NOMENU | MCIWNDF_NOTIFYALL | MCIWNDF_NOPLAYBAR, AVfile);
		if (hWndAVI)
		{
			SetWindowPos(hWndAVI, NULL, 0, 0, rectView.right, rectView.bottom, SWP_NOZORDER | SWP_NOMOVE);
			MCIWndPlay(hWndAVI);
			childe = true;
		}
		f = fopen("My.txt", "r");
		for (int i = 1; i < 6; i++)
		{
			fscanf(f, "%d %d %s %d %d %d %s %s %s %s %d %d %d %d", &my_info[i].What_Poke, &my_info[i].poke, my_info[i].name,
				&my_info[i].Level, &my_info[i].MaxHp, &my_info[i].Stack_Damage, my_info[i].Skill1, my_info[i].Skill2, my_info[i].Skill3,
				my_info[i].Skill4, &my_info[i].Limit1, &my_info[i].Limit2, &my_info[i].Limit3, &my_info[i].Limit4);
		}
		fclose(f);
		
		//SetTimer(hWnd, 1, 100, NULL);
		return 0;
	}
	case WM_PAINT:
	{
		for (int i = 0; i < 4; i++)
		{
			Bag[i] = my_info[i + 1].What_Poke;
		}
		CImage img;
		img.Create(rectView.right, rectView.bottom, 24);
		hDC = BeginPaint(hWnd, &ps);
		memDC = img.GetDC();
		{
			if (fade == false)
			{
				if (maptype == 0)
					background[map].Draw(memDC, 0, 0, rectView.right, rectView.bottom);
				else if (maptype == 1)
				{

					if (moving == true)
					{
						if (direct == 2)
							background[map].StretchBlt(memDC, 0, 0, rectView.right, rectView.bottom, (cx + mcount / 3 - mapsx)*(background[map].GetWidth() / mapx), (cy - mapsy)*(background[map].GetHeight() / mapy), 2 * mapsx * (background[map].GetWidth() / mapx), 2 * mapsy * (background[map].GetHeight() / mapy));
						else if (direct == 1)
							background[map].StretchBlt(memDC, 0, 0, rectView.right, rectView.bottom, (cx - mcount / 3 - mapsx)*(background[map].GetWidth() / mapx), (cy - mapsy)*(background[map].GetHeight() / mapy), 2 * mapsx * (background[map].GetWidth() / mapx), 2 * mapsy * (background[map].GetHeight() / mapy));
						else if (direct == 0)
							background[map].StretchBlt(memDC, 0, 0, rectView.right, rectView.bottom, (cx - mapsx)*(background[map].GetWidth() / mapx), (cy + mcount / 3 - mapsy)*(background[map].GetHeight() / mapy), 2 * mapsx * (background[map].GetWidth() / mapx), 2 * mapsy * (background[map].GetHeight() / mapy));
						else if (direct == 3)
							background[map].StretchBlt(memDC, 0, 0, rectView.right, rectView.bottom, (cx - mapsx)*(background[map].GetWidth() / mapx), (cy - mcount / 3 - mapsy)*(background[map].GetHeight() / mapy), 2 * mapsx * (background[map].GetWidth() / mapx), 2 * mapsy * (background[map].GetHeight() / mapy));
					}
					else
					{

						background[map].StretchBlt(memDC, 0, 0, rectView.right, rectView.bottom, (cx - mapsx)*(background[map].GetWidth() / mapx), (cy - mapsy)*(background[map].GetHeight() / mapy), 2 * mapsx * (background[map].GetWidth() / mapx), 2 * mapsy * (background[map].GetHeight() / mapy));
					}
				}
				if (maptype == 0)
				{
					if (moving == false)
						hero[direct * 3].Draw(memDC, cx*dx, cy*dy, dx, dy);
					else
					{
						if (direct == 2)
							hero[mcount + direct * 3].Draw(memDC, cx*dx + mcount * dx / 3, cy*dy, dx, dy);
						else if (direct == 1)
							hero[mcount + direct * 3].Draw(memDC, cx*dx - mcount * dx / 3, cy*dy, dx, dy);
						else if (direct == 0)
							hero[mcount + direct * 3].Draw(memDC, cx*dx, cy*dy + mcount * dy / 3, dx, dy);
						else if (direct == 3)
							hero[mcount + direct * 3].Draw(memDC, cx*dx, cy*dy - mcount * dy / 3, dx, dy);
					}
				}
				else if (maptype == 1)
				{
					if (moving == false)
						hero[direct * 3].Draw(memDC, mapsx * dx, mapsy * dy, dx, dy);
					else
					{
						if (direct == 2)
							hero[mcount + direct * 3].Draw(memDC, mapsx*dx + mcount * dx / 3, mapsy * dy, dx, dy);
						else if (direct == 1)
							hero[mcount + direct * 3].Draw(memDC, mapsx * dx - mcount * dx / 3, mapsy * dy, dx, dy);
						else if (direct == 0)
							hero[mcount + direct * 3].Draw(memDC, mapsx * dx, mapsy *dy + mcount * dy / 3, dx, dy);
						else if (direct == 3)
							hero[mcount + direct * 3].Draw(memDC, mapsx * dx, mapsy *dy - mcount * dy / 3, dx, dy);
					}
				}
				if (dial > 0)
				{
					bDialog.Draw(memDC, 0, 0, rectView.right, rectView.bottom);
					SetBkMode(memDC, TRANSPARENT);//글자 배경색 제거
					RECT fect = { rectView.right * 1 / 15, rectView.bottom * 8 / 11, rectView.right * 14 / 15, rectView.bottom };
					lf.lfHeight = rectView.bottom / 10;
					//lf.lfEscapement = 0;
					Font = CreateFontIndirect(&lf);
					OldFont = (HFONT)SelectObject(memDC, Font);
					DrawText(memDC, dstr[dial], -1, &fect, DT_LEFT);
					SelectObject(memDC, OldFont);
					DeleteObject(Font);
				}
				if (wmap >= 0)
				{
					if (map == 0 || maptype == 0)
					{
						bwmap[0].Draw(memDC, 0, 0, rectView.right, rectView.bottom);
					}
					else if(map == 6)
					{
						bwmap[1].Draw(memDC, 0, 0, rectView.right, rectView.bottom);
					}
					else if (map == 7)
					{
						bwmap[2].Draw(memDC, 0, 0, rectView.right, rectView.bottom);
					}
					else if (map == 8)
					{
						bwmap[3].Draw(memDC, 0, 0, rectView.right, rectView.bottom);
					}
					else if (map == 9)
					{
						bwmap[4].Draw(memDC, 0, 0, rectView.right, rectView.bottom);
					}
					else if (map == 10)
					{
						bwmap[5].Draw(memDC, 0, 0, rectView.right, rectView.bottom);
					}
					else if (map == 11)
					{
						bwmap[6].Draw(memDC, 0, 0, rectView.right, rectView.bottom);
					}
					else if (map == 12)
					{
						bwmap[7].Draw(memDC, 0, 0, rectView.right, rectView.bottom);
					}
					else if (map == 13)
					{
						bwmap[8].Draw(memDC, 0, 0, rectView.right, rectView.bottom);
					}
				}
				if (sbag >= 0)
				{
					mbag[sbag].Draw(memDC, rectView.right / 3, rectView.bottom / 3, rectView.right/2, rectView.bottom/2);
				}
				if (poke == true)
				{
					Exchange_Screen.Draw(memDC, 0, 506, rectView.right, 240);
					lf.lfHeight = rectView.bottom / 20;
					Font = CreateFontIndirect(&lf);
					OldFont = (HFONT)SelectObject(memDC, Font);
					int count = 0;
					char str[120];
					Draw_Pokemon_BackPack(Bag, count, memDC, rectView, str);
				}
				if (msel >= 0)
				{
					bmsel[msel].Draw(memDC, rectView.right / 4, rectView.bottom / 4, rectView.right / 2, rectView.bottom / 2);
				}
				
				if (starting < 11)
				{
					Rectangle(memDC, 0, 0, rectView.right, rectView.bottom);
					bPro.Draw(memDC, rectView.right * 1 / 3, rectView.bottom * 1 / 11, rectView.right * 1 / 3, rectView.bottom * 7 / 11);
					bDialog.Draw(memDC, 0, 0, rectView.right, rectView.bottom);
					SetBkMode(memDC, TRANSPARENT);//글자 배경색 제거
					RECT fect = { rectView.right * 1 / 15, rectView.bottom * 8 / 11, rectView.right * 14 / 15, rectView.bottom };
					lf.lfHeight = rectView.bottom / 10;
					//lf.lfEscapement = 0;
					Font = CreateFontIndirect(&lf);
					OldFont = (HFONT)SelectObject(memDC, Font);
					DrawText(memDC, osstr[starting], -1, &fect, DT_LEFT);
					SelectObject(memDC, OldFont);
					DeleteObject(Font);
				}
			}
			img.Draw(hDC, 0, 0);
			img.ReleaseDC();
		}
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_KEYDOWN:
		if (starting < 11)
		{
			if (wParam == VK_SPACE)
			{
				if (starting == 0)
				{
					MCIWndClose(hWndAVI);
					MCIWndDestroy(hWndAVI);
					hWndAVI = 0;
					PlayBgm(0);
				}
				InvalidateRect(hWnd, NULL, FALSE);
				starting++;
				if (starting == 11)
				{
					PlayBgm(1);
					childe = false;
				}
				PlayEffect(0);
			}
		}
		if (!childe)
		{
			if (wmap >= 0)
			{
				if (wParam == VK_ESCAPE || wParam == VK_SPACE)
				{
					wmap = -1;
				}
			}

			if (poke == true)
			{
				if (wParam == VK_ESCAPE || wParam == VK_SPACE)
				{
					poke = false;
				}
			}
			if (msel < 0 && sbag < 0)
			{
				if (dial == 0)
				{
					if (wParam == VK_UP)
					{
						if (moving == false)
						{
							direct = 3;
							if (block[cy - 1][cx] != 0 && cy - 1 > -1)
							{
								moving = true;
								mcount = 0;
								SetTimer(hWnd, 2, 50, NULL);
							}
						}
					}
					else if (wParam == VK_DOWN)
					{
						if (moving == false)
						{
							direct = 0;
							if (maptype == 0)
							{
								if (block[cy + 1][cx] != 0 && (cy + 1)*dy < rectView.bottom)
								{
									moving = true;
									mcount = 0;
									SetTimer(hWnd, 2, 50, NULL);
								}
							}
							else if (maptype == 1 || maptype == 2)
							{
								if (block[cy + 1][cx] != 0 && cy < mapy - 1)
								{
									moving = true;
									mcount = 0;
									SetTimer(hWnd, 2, 50, NULL);
								}
							}
						}
					}
					else if (wParam == VK_LEFT)
					{
						if (moving == false)
						{
							direct = 1;
							if (block[cy][cx - 1] != 0 && cx - 1 > -1)
							{
								moving = true;
								mcount = 0;
								SetTimer(hWnd, 2, 50, NULL);
							}
						}
					}
					else if (wParam == VK_RIGHT)
					{
						if (moving == false)
						{
							direct = 2;
							if (maptype == 0)
							{
								if (block[cy][cx + 1] != 0 && (cx + 1)*dx < rectView.right)
								{
									moving = true;
									mcount = 0;
									SetTimer(hWnd, 2, 50, NULL);
								}
							}
							else if (maptype == 1 || maptype == 2)
							{
								if (block[cy][cx + 1] != 0 && cx < mapx - 1)
								{
									moving = true;
									mcount = 0;
									SetTimer(hWnd, 2, 50, NULL);
								}
							}
						}
					}
				}
				if (wParam == VK_ESCAPE)
				{
					msel = 0;
				}
			}
			else if(msel >= 0)
			{
				if (wParam == VK_ESCAPE)
				{
					msel = -1;
				}
				else if (wParam == VK_UP)
				{
					if (msel == 0)
					{
						msel = 4;
					}
					else if (msel == 1)
					{
						msel = 5;
					}
					else
					{
						msel -= 2;
					}
				}
				else if (wParam == VK_DOWN)
				{
					if (msel == 4)
					{
						msel = 0;
					}
					else if (msel == 5)
					{
						msel = 1;
					}
					else
					{
						msel += 2;
					}
				}
				else if (wParam == VK_LEFT)
				{
					if (msel % 2 == 0)
					{
						msel++;
					}
					else
					{
						msel--;
					}
				}
				else if (wParam == VK_RIGHT)
				{
					if (msel % 2 == 0)
					{
						msel++;
					}
					else
					{
						msel--;
					}
				}
			}
			else if (sbag >= 0)
			{
				if (wParam == VK_ESCAPE || wParam == VK_SPACE)
				{
					sbag = -1;
				}
				else if (wParam == VK_UP)
				{
					if (sbag == 0)
					{
						sbag = 2;
					}
					else
					{
						sbag--;
					}
				}
				else if (wParam == VK_DOWN)
				{
					if (sbag == 2)
					{
						sbag = 0;
					}
					else
					{
						sbag++;
					}
				}
			}

			if (wParam == VK_SPACE)
			{
				if (msel < 0 && sbag < 0)
				{
					if (dial == 0)
					{
						movemap(&cx, &cy, block, npc, &map, &maptype, &mapx, &mapy, &mapsx, &mapsy, &dx, &dy, rectView);
						if (direct == 0)
						{
							if (npc[cy + 1][cx] > 0)
							{
								dial = npc[cy + 1][cx];
							}
						}
						else if (direct == 1)
						{
							if (npc[cy][cx - 1] > 0)
							{
								dial = npc[cy][cx - 1];
							}
						}
						else if (direct == 2)
						{
							if (npc[cy][cx + 1] > 0)
							{
								dial = npc[cy][cx + 1];
							}
						}
						else if (direct == 3)
						{
							if (npc[cy - 1][cx] > 0)
							{
								dial = npc[cy - 1][cx];
							}
						}
					}
					else
					{
						if (dial == 7)//포켓몬 센터 회복
						{
							PlayEffect(2);
							Cure_Pokemon();
							fade = true;
							SetTimer(hWnd, 3, 2500, NULL);
						}
						else if (dial == 8)//포켓몬 상점 구입
						{

						}
						else if (dial == 9)//포켓몬 관장 배틀
						{
							PlayBgm(9);
							state = 5;
							cmap = map;
							child_hWnd = CreateWindow("Battle", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
								0, 0, rectView.right, rectView.bottom, hWnd, NULL, g_hinst, NULL);
							childe = true;
							SetFocus(child_hWnd);
						}
						else if (dial == 11)//인버서스 게임 시작
						{
							childe = true;
							child_hWnd = CreateWindow("Child", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
								0, 0, rectView.right, rectView.bottom, hWnd, NULL, g_hinst, NULL);
							SetFocus(child_hWnd);
						}
						else if (dial == 12)//가이오가 배틀
						{
							PlayBgm(10);
							state = 2;
							cmap = map;
							child_hWnd = CreateWindow("Battle", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
								0, 0, rectView.right, rectView.bottom, hWnd, NULL, g_hinst, NULL);
							childe = true;
							SetFocus(child_hWnd);
						}
						else if (dial == 13)//그란돈 배틀
						{
							PlayBgm(10);
							state = 3;
							cmap = map;
							child_hWnd = CreateWindow("Battle", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
								0, 0, rectView.right, rectView.bottom, hWnd, NULL, g_hinst, NULL);
							childe = true;
							SetFocus(child_hWnd);
						}
						else if (dial == 14)//레쿠자 배틀
						{
							PlayBgm(11);
							state = 4;
							cmap = map;
							child_hWnd = CreateWindow("Battle", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
								0, 0, rectView.right, rectView.bottom, hWnd, NULL, g_hinst, NULL);
							childe = true;
							SetFocus(child_hWnd);
						}
						dial = 0;
					}
				}
				else
				{
					if (msel == 0)//포켓몬
					{
						poke = true;
					}
					else if (msel == 1)//맵
					{
						wmap = 0;
					}
					else if (msel == 2)//가방
					{
						sbag = 0;
					}
					else if (msel == 3)//치트키
					{
						if (wildm == true)
						{
							wildm = false;
							MessageBox(NULL, "야생 포켓몬 off", "야생", MB_OK);
						}
						else
						{
							MessageBox(NULL, "야생 포켓몬 on", "야생", MB_OK);
							wildm = true;
						}
					}
					else if (msel == 4)//헬프
					{
						MessageBox(NULL, "Credit 쓰려다 잘못 썻어요\n2015182007 김선중\n2016182044 하재오", "제작자", MB_OK);
					}
					else if (msel == 5)//나가기
					{
						PostQuitMessage(0);
					}
					msel = -1;
				}
			}
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	case WM_KEYUP:
		if (!childe)
			InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_CHAR:
		if (!childe)
		{
			if (msel < 0 && sbag < 0)
			{
				if (dial == 0)
				{
					if (wParam == '1')//마을
					{
						map = 0;
						cx = 20;
						cy = 25;
						createmap(block, npc, map, &maptype, &mapx, &mapy, &mapsx, &mapsy, &dx, &dy, rectView);
					}
					else if (wParam == '2')//가이오가
					{
						map = 7;
						cx = 20;
						cy = 4;
						createmap(block, npc, map, &maptype, &mapx, &mapy, &mapsx, &mapsy, &dx, &dy, rectView);
					}
					else if (wParam == '3')//그란돈
					{
						map = 13;
						cx = 20;
						cy = 42;
						createmap(block, npc, map, &maptype, &mapx, &mapy, &mapsx, &mapsy, &dx, &dy, rectView);
					}
					else if (wParam == '4')//레쿠자
					{
						map = 9;
						cx = 22;
						cy = 36;
						createmap(block, npc, map, &maptype, &mapx, &mapy, &mapsx, &mapsy, &dx, &dy, rectView);
					}
				}
			}
		}
		if (!childe)
			InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_LBUTTONDOWN: // 버튼을 누르면 드래그 동작 시작

		break;
	case WM_LBUTTONUP: // 버튼을 놓으면 드래그 종료
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_MOUSEMOVE:
		break;

	case WM_TIMER: // 시간이 경과하면 메시지 자동 생성
		if (!childe)
		{
			switch (wParam) {
			case 1:
				break;
			case 2:
				if (moving == true)
				{
					if (mcount < 2)
					{
						mcount++;
					}
					else if (mcount == 2)
					{
						moving = false;
						if (direct == 2)
							cx++;
						else if (direct == 1)
							cx--;
						else if (direct == 0)
							cy++;
						else if (direct == 3)
							cy--;
						KillTimer(hWnd, 2);

						if (map != 0 && maptype == 1 && wildm == true)
						{
							int bs = rand() % 5;
							if (bs == 0)
							{
								if (map == 10 || map == 6)//level 1
								{
									state = 1;
									Field_Level = 1;
									cmap = map;
									PlayBgm(8);
									child_hWnd = CreateWindow("Battle", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
										0, 0, rectView.right, rectView.bottom, hWnd, NULL, g_hinst, NULL);
									childe = true;
									SetFocus(child_hWnd);
								}
								else if (map == 8 || map == 11 || map == 12)//level 2
								{
									state = 1;
									Field_Level = 2;
									cmap = map;
									PlayBgm(8);
									child_hWnd = CreateWindow("Battle", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
										0, 0, rectView.right, rectView.bottom, hWnd, NULL, g_hinst, NULL);
									childe = true;
									SetFocus(child_hWnd);
								}
								else if (map == 7 || map == 9 || map == 13)//level 3
								{
									state = 1;
									Field_Level = 3;
									cmap = map;
									PlayBgm(8);
									child_hWnd = CreateWindow("Battle", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
										0, 0, rectView.right, rectView.bottom, hWnd, NULL, g_hinst, NULL);
									childe = true;
									SetFocus(child_hWnd);
								}
							}
						}
					}
				}
				break;
			case 3:
				fade = false;
				KillTimer(hWnd, 3);
				break;
			}
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	case WM_DESTROY:
		KillTimer(hWnd, 2);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProcA(hWnd, iMessage, wParam, IParam));//위의 세 메세지 외의 나머지 메세지는 OS로
}

LRESULT CALLBACK ChildProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)
{
	PAINTSTRUCT ps;
	static HDC hDC, hMemDC;
	HBRUSH hBrush, oldBrush, eBrush, ehBrush, unBrush;
	HBRUSH hBrush2, hBrush3, oldBrush2;
	HPEN MyPen, ePen, OldPen, cPen, unPen, iPen;
	HFONT Font, OldFont;
	RECT regg[6];// = { 400, 300, 500, 310 };//총알 날라가는거
	RECT segg[18];//특수총알
	static RECT rectView;
	static RECT tect;
	static LOGFONT lf;
	static double dx, dy; //사각형 크기
	static double rx[7], ry[7];//총알 회전위치
	static double cx = 400, cy = 300;//주인공 첫 위치
	static double bullet[6][4] = { 0 };//총알
	static double seta = 0;//총알 돌리기 위한 세타
	static int reload = 0;//총알 장전

	static player parray[2];

	static double effect[LIMIT_ENEMY + 1][17];//사망 이펙트
	static double reffect[LIMIT_ENEMY][4] = { 0 };//리스폰 이펙트

	static double enemy[LIMIT_ENEMY][5] = { 0 };//적
	static int ecolor[3];//적 색깔
	static int ecount = 0; //적 카운트
	static int etime = 30; // 적 생성시간

	static BOOL death = false;//플레이어 죽음
	static BOOL infi = false;
	static int dcount = 0;//부활 카운트
	static int life = 3;

	static double sgun[LIMIT_SGUN][3] = { 0 };//특수총알
	static int scount = 0;

	static int score = 0;
	static int combo = 0;

	static int ten = 0;
	static int gametime = 0;
	static char str[120];

	static int level = 1;

	static BOOL multi = false;

	static int block[20][20] = { 0 };//보드
									 //메세지 처리하기
	switch (iMessage) {
	case WM_CREATE:
		srand((unsigned)time(NULL));
		SetGame(hWnd, &rectView, &tect, &dx, &dy, block, bullet, ecolor, &cx, &cy, &seta, &reload
			, effect, reffect, enemy, &ecount, &etime, &death, &dcount, &life, &score, &combo
			, &ten, &gametime, level, sgun, &scount, multi);
		ZeroMemory(&lf, sizeof(lf));
		multireset(parray, rectView, dx, dy);
		return 0;
	case WM_PAINT:
	{
		CImage img;
		img.Create(rectView.right, rectView.bottom, 24);
		hDC = BeginPaint(hWnd, &ps);
		MyPen = CreatePen(PS_SOLID, 3, RGB(125, 125, 125));//보드
		ePen = CreatePen(PS_SOLID, 3, RGB(ecolor[0], ecolor[1], ecolor[2]));//적
		cPen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));//주인공
		iPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));//무적 주인공
		unPen = (HPEN)GetStockObject(NULL_BRUSH);//투명

		unBrush = (HBRUSH)GetStockObject(NULL_BRUSH);//투명
		hBrush = CreateSolidBrush(RGB(255, 255, 255));//흰색
		hBrush2 = CreateSolidBrush(RGB(0, 0, 0));//검정
		hBrush3 = CreateSolidBrush(RGB(125, 125, 125));//검정
		eBrush = CreateSolidBrush(RGB(ecolor[0], ecolor[1], ecolor[2]));//적
		ehBrush = CreateHatchBrush(HS_BDIAGONAL, RGB(ecolor[0], ecolor[1], ecolor[2]));//적 빗금

		hMemDC = img.GetDC();
		{
			//-------------------------------------------------------
			//보드판
			Font = CreateFontIndirect(&lf);
			OldFont = (HFONT)SelectObject(hMemDC, Font);
			oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
			Hscorebord(oldBrush, hBrush, hBrush2, eBrush, hMemDC, rectView, dx, dy, life, Font, OldFont, str, score, combo, tect, &lf, multi);

			OldPen = (HPEN)SelectObject(hMemDC, MyPen);
			Hcreateboad(block, dx, dy, hBrush, hBrush2, hBrush3, oldBrush, hMemDC);
			if (multi == false)
			{
				//적
				Henemy(enemy, block, sgun, dx, dy, cx, cy, &infi, &death, effect, &life, &dcount, ePen, OldPen, ehBrush, oldBrush, hMemDC);
				//부활이펙트
				Hrespwan(OldPen, oldBrush, unBrush, hBrush2, eBrush, cPen, ePen, hMemDC, dx, dy, reffect);

				for (int i = 0; i < LIMIT_ENEMY; i++)
				{
					if (enemy[i][0] == 1)
					{
						oldBrush = (HBRUSH)SelectObject(hMemDC, eBrush);
						Rectangle(hMemDC, enemy[i][1] - dx / 2, enemy[i][2] - dy / 2, enemy[i][1] + dx / 2, enemy[i][2] + dy / 2);
					}
				}

				OldPen = (HPEN)SelectObject(hMemDC, cPen);
				//주인공
				oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush2);
				if (death == false)
				{
					if (infi == false)
						OldPen = (HPEN)SelectObject(hMemDC, cPen);
					else
						OldPen = (HPEN)SelectObject(hMemDC, iPen);
					Rectangle(hMemDC, cx - dx / 2, cy - dy / 2, cx + dx / 2, cy + dy / 2);
					Hplayersgun(sgun, dx, dy, bullet, cx, cy, &score);
				}
				OldPen = (HPEN)SelectObject(hMemDC, cPen);
				oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);

				//총알
				for (int i = 0; i < 6; i++)
				{
					//총알 도는거
					if (bullet[i][0] == 1 || bullet[i][0] == 2)
					{
						Hrotategun(rx, ry, cx, cy, dx, dy, seta, i, death, bullet, oldBrush, hBrush, eBrush, hMemDC);
					}
					else if (bullet[i][0] == 3)
					{
						//총알 쏜거
						Hshotbullet(bullet, regg, hMemDC, i, 0, ecolor, 0);
						//총알-블록 충돌
						Hcolblock(dx, dy, regg, block, &score, &combo, bullet, i, 0, 0);
						//총알-적 충돌
						Hcolenemy(dx, dy, regg, block, &score, &combo, bullet, i, effect, enemy, sgun, &scount, 0);
					}
					else if (bullet[i][0] == 4)
					{
						//총알 쏜거
						Hshotbullet(bullet, segg, hMemDC, i, 1, ecolor, 0);
						//총알-블록 충돌
						Hcolblock(dx, dy, segg, block, &score, &combo, bullet, i, 1, 0);
						//총알-적 충돌
						Hcolenemy(dx, dy, segg, block, &score, &combo, bullet, i, effect, enemy, sgun, &scount, 1);
					}
				}

				SelectObject(hMemDC, OldPen);
				oldBrush = (HBRUSH)SelectObject(hMemDC, eBrush);
				Hsgun(sgun, seta, dx, dy, hMemDC);
			}
			//사망이펙트
			Hdeatheffect(effect, oldBrush, hBrush2, eBrush, hMemDC);

			if (life == 0)
			{
				Hgameover(hMemDC, rectView, score, str, hWnd);
			}
			SelectObject(hMemDC, OldFont);
			DeleteObject(Font);

			SelectObject(hMemDC, oldBrush);
			DeleteObject(hBrush);
			DeleteObject(eBrush);
			DeleteObject(MyPen);
			DeleteObject(cPen);
			DeleteObject(ePen);
			DeleteObject(iPen);
			DeleteObject(hBrush2);
			DeleteObject(hBrush3);
			img.Draw(hDC, 0, 0);
			img.ReleaseDC();
		}
		//-----------------------------------------------------------------------------------

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_TIMER: // 시간이 경과하면 메시지 자동 생성
		switch (wParam) {
		case 1:
			if (seta < 2 * 3.14)
			{
				seta += 0.3;
			}
			else
			{
				seta = 0;
			}
			if (multi == false)
			{
				ReloadBullet(&reload, bullet, 20);
				MoveBullet(rectView, bullet, 50, &combo, multi, regg, regg);
				Tdetaheffect(effect);
				if (dcount > 0)
				{
					dcount--;
					if (dcount == 1)
					{
						Trespawnclear(level, block, enemy, effect, dx, dy);
						etime = 20;
						cx = rectView.right / 2;
						cy = rectView.bottom / 2;
						RespawnEffect(reffect, cx, cy, -1);
					}
				}
				MoveEnemy(enemy, cx, cy);
				Trespawneffect(reffect, gametime, &death, enemy);

				if (etime > 0)
				{
					etime--;
				}
				else
				{
					Tcreateenemy(cy, rectView, enemy, ecount);
					enemy[ecount][3] = rand() % 3 + 5;
					RespawnEffect(reffect, enemy[ecount][1], enemy[ecount][2], ecount);
					Tenemyspwantime(&ecount, gametime, &etime);
				}
				if (death == false)
				{
					MovePlayer(hWnd,&cx, &cy, dx, dy, block, rectView, 10, 0);
				}
			}
			if (ten < 10)//1초
			{
				ten++;
			}
			else
			{
				ten = 0;
				gametime++;
				score++;
			}
			break;
		case 2:

			break;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_CHAR:
		//if ( wParam == 'x')
			//MessageBox(NULL, "char", "char", MB_OK);
		break;

	case WM_KEYDOWN:
		//MessageBox(NULL, "test", "test", MB_OK);
		if (death == false)
		{
			
			if (wParam == VK_UP)
			{
				Kshotbullet(bullet, cx, cy, 2);
			}
			else if (wParam == VK_DOWN)
			{
				Kshotbullet(bullet, cx, cy, 3);
			}
			else if (wParam == VK_RIGHT)
			{
				Kshotbullet(bullet, cx, cy, 0);
			}
			else if (wParam == VK_LEFT)
			{
				Kshotbullet(bullet, cx, cy, 1);
			}
			else if (wParam == VK_SPACE)
			{
				if (infi == true)
					infi = false;
				else
					infi = true;
			}
		}

		if (life == 0)
		{
			if (wParam == VK_RETURN)
			{
				SetGame(hWnd, &rectView, &tect, &dx, &dy, block, bullet, ecolor, &cx, &cy, &seta, &reload
					, effect, reffect, enemy, &ecount, &etime, &death, &dcount, &life, &score, &combo
					, &ten, &gametime, level, sgun, &scount, multi);
			}
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_KEYUP:

		InvalidateRect(hWnd, NULL, FALSE);
		break;
	//case WM_CHAR:
	//	InvalidateRect(hWnd, NULL, FALSE);
	//	break;

	case WM_DESTROY:
		KillTimer(hWnd, 1);
		childe = false;
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, IParam));//위의 세 메세지 외의 나머지 메세지는 OS로
}

LRESULT CALLBACK BattleProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)
{
	int mx, my;
	PAINTSTRUCT ps;
	FILE *f;
	static Enemy_info info[15];
	HFONT Font, OldFont;
	static LOGFONT lf;
	static int Respawn;//몬스터 생성
	static int Skill_Selection;//AI가 사용할 스킬을 결정함.
	static RECT rectView;
	static HDC hDC, memDC;
	HBRUSH hBrush, hBrush2, oldBrush;
	HPEN MyPen, oldPen;
	static SIZE size;
	static double MyRatio, EnemyRatio;
	static int EnemyHp, Stack_Damage2 = 0, Damage1, Damage2, EMovePos, MMovePos;//MyHp-Damage
	static int EnemyLv;
	static int Skill_Progress;//스킬 모션 진행
	static int poke;//BackPoke
	static int count, BallYpos;//BallYpos=>몬스터볼 던질때 공팅기는거 표현.
	static int MsgJudge = 0;//선택지
	static BOOL Attack = FALSE;
	static BOOL Dead;//Dead가 True==내가죽음,else==적이죽음
					 //적을 조우할시 토크박스 저지는0으로 초기화,hp들은 파일입출력으로 받아와서 초기화,데미지는0으로초기화.
					 //아이템 사용,포켓몬 교체 ->자식윈도우 or 다이얼로그 박스를 통해 구현 예정
	switch (iMessage) {
	case WM_CREATE:
	{
		srand((unsigned)time(NULL));
		ZeroMemory(&lf, sizeof(lf));
		GetClientRect(hWnd, &rectView);
		if (state == 1)//야생
		{
			if (Field_Level == 1)
				f = fopen("Lv1zone.txt", "r"), EnemyLv = 1 + rand() % 20;
			else if (Field_Level == 2)
				f = fopen("Lv2zone.txt", "r"), EnemyLv = 20 + rand() % 21;//a+rand()%b-a+1
			else if (Field_Level == 3)
				f = fopen("Lv3zone.txt", "r"), EnemyLv = 41 + rand() % 21;
			for (int i = 0; i < 15; i++)
			{
				fscanf(f, "%d %s %s %s", &info[i].poke, &info[i].name, &info[i].Skill1, &info[i].Skill2);
			}
			Respawn = rand() % 15;
			EnemyHp = EnemyLv * 10;
			fclose(f);
		}
		else if (state == 2)//가이오가
		{
			EnemyLv = 80;
			info[0].poke = 382;
			strcpy(info[0].name, "가이오가");
			strcpy(info[0].Skill1, "위압감주기");
			strcpy(info[0].Skill2, "파도타기");
			Respawn = 0;
			EnemyHp = EnemyLv * 10;
		}
		else if (state == 3)//그란돈
		{
			EnemyLv = 100;
			info[0].poke = 383;
			strcpy(info[0].name, "그란돈");
			strcpy(info[0].Skill1, "위압감주기");
			strcpy(info[0].Skill2, "땅흔들기");
			Respawn = 0;
			EnemyHp = EnemyLv * 10;
		}
		else if (state == 4)//레쿠쟈
		{
			EnemyLv = 90;
			info[0].poke = 384;
			strcpy(info[0].name, "레쿠쟈");
			strcpy(info[0].Skill1, "위압감주기");
			strcpy(info[0].Skill2, "백만볼트");
			Respawn = 0;
			EnemyHp = EnemyLv * 10;
		}
		else if (state == 5)
		{
			EnemyLv = 777;
			info[0].poke = 150;
			strcpy(info[0].name, "뮤츠");
			strcpy(info[0].Skill1, "정권지르기");
			strcpy(info[0].Skill2, "염동력");
			Respawn = 0;
			EnemyHp = EnemyLv * 10;
		}
		TalkBox_Judge = 0;
		MsgJudge = 0;
		Catch = FALSE;
		poke = my_info[What_Poke].poke;
		Stack_Damage2 = 0;
		SetTimer(hWnd, 1, 100, NULL);
		Skill_Using = FALSE;
		
		return 0;
	}
	case WM_PAINT:
	{
		CImage img;
		img.Create(rectView.right, rectView.bottom, 24);
		hDC = BeginPaint(hWnd, &ps);
		hBrush2 = (HBRUSH)GetStockObject(NULL_BRUSH);
		memDC = img.GetDC();
		{
			battle[1].Draw(memDC, 0, 0, rectView.right, rectView.bottom);
			char str[128];
			SetBkMode(memDC, TRANSPARENT);//글자 배경색 제거
			
			if (state == 5 && TalkBox_Judge == 0);
			else
			Battle_Begin(lf, rectView, memDC, str, EnemyLv, info, Respawn, state);

			Hp_State_Draw(memDC, EnemyHp, Stack_Damage2);
			
			RECT fect = { rectView.right * 1 / 15, rectView.bottom * 8 / 11, rectView.right * 14 / 15, rectView.bottom };
			lf.lfHeight = rectView.bottom / 10;
			Font = CreateFontIndirect(&lf);
			OldFont = (HFONT)SelectObject(memDC, Font);
			if (TalkBox_Judge != 11)
				backpoke[my_info[What_Poke].poke].Draw(memDC, rectView.right / 10 + MMovePos, rectView.bottom * 6 / 15, rectView.right / 3, rectView.bottom / 3);
			else
			{
				if (Show_Ball_Throw < 16)SetTimer(hWnd, 4, 40, NULL);
				if (Show_Ball_Throw < 5)
					Ball_Throw[Show_Ball_Throw].TransparentBlt(memDC, rectView.right / 10 + MMovePos, rectView.bottom * 6 / 15, rectView.right / 3, rectView.bottom / 3, RGB(0, 0, 0));
				else
				{
					if (Show_Ball_Throw < 16)
					{
						Ball_Throw[4].TransparentBlt(memDC, rectView.right / 10 + MMovePos, rectView.bottom * 6 / 15, rectView.right / 3, rectView.bottom / 3, RGB(0, 0, 0));
						Ball_Throw[Show_Ball_Throw].TransparentBlt(memDC, 300 + rectView.right / 10 + MMovePos + 40 * (Show_Ball_Throw - 5), rectView.bottom * 6 / 15 - 12 * (Show_Ball_Throw - 5), 30, 30, RGB(0, 0, 0));
					}
				}
			}
			if (TalkBox_Judge == 11 && Show_Ball_Throw >= 15)
			{
				if (Show_Ball_Throw == 15)
				{
					Ball_Throw[14].TransparentBlt(memDC, 300 + rectView.right / 10 + MMovePos + 40 * (Show_Ball_Throw - 7), rectView.bottom * 6 / 15 - 12 * (Show_Ball_Throw - 7), 30, 30, RGB(0, 0, 0)), Ball_Throw[15].TransparentBlt(memDC, 300 + rectView.right / 10 + MMovePos + 40 * (Show_Ball_Throw - 6), rectView.bottom * 6 / 15 - 12 * (Show_Ball_Throw - 6), 100, 100, RGB(0, 0, 0));
					KillTimer(hWnd, 1);
					count = 0, BallYpos = 0;
					SetTimer(hWnd, 5, 60, NULL);
				}
				else if (Show_Ball_Throw == 16)
				{
					Ball_Throw[16].TransparentBlt(memDC, 300 + rectView.right / 10 + MMovePos + 40 * (Show_Ball_Throw - 7), rectView.bottom * 6 / 15 - 12 * (Show_Ball_Throw - 6) + BallYpos, 30, 30, RGB(0, 0, 0));
				}
				else if (Show_Ball_Throw >= 17 && Show_Ball_Throw <= 20)
				{
					Ball_Throw[Show_Ball_Throw].TransparentBlt(memDC, 300 + rectView.right / 10 + MMovePos + 40 * (9), rectView.bottom * 6 / 15 - 12 * (10) + BallYpos, 30, 30, RGB(0, 0, 0));
				}
				if (Show_Ball_Throw == 21)
				{
					if (Catch)
					{
						TalkBox_Judge = 9;
						Ball--;
					}
					else
					{
						EnemyTurn = TRUE;
						TalkBox_Judge = 10;
						Ball--;
					}
				}

			}
			else
			{
				if (state == 5 && TalkBox_Judge == 0)
					pokemon[399].Draw(memDC, rectView.right * 6 / 10 + EMovePos, rectView.bottom * 2 / 15, rectView.right / 3, rectView.bottom / 3);//포켓몬 그리기!
				else
					pokemon[info[Respawn].poke].Draw(memDC, rectView.right * 6 / 10 + EMovePos, rectView.bottom * 2 / 15, rectView.right / 3, rectView.bottom / 3);//포켓몬 그리기!
			}
			if (TalkBox_Judge == 0)//야생몬스터출현
			{
				if (state == 5)
				{
					wsprintf(str, "관장이 배틀을 신청했다.\n가라 %s!", my_info[What_Poke].name);
				}
				else
					wsprintf(str, "%s가 나타났다.\n가라 %s!", info[Respawn].name, my_info[What_Poke].name);
				DrawText(memDC, str, -1, &fect, DT_LEFT);
			}
			else if (TalkBox_Judge == 1)//행동선택지
			{
				wsprintf(str, "공격하기                아이템사용\n도망가기                교체");
				DrawText(memDC, str, -1, &fect, DT_LEFT);
				hBrush = CreateSolidBrush(RGB(0, 0, 255));
				oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
				Draw_MsgBox(memDC, MsgJudge);
				SelectObject(memDC, oldBrush);
				DeleteObject(hBrush);
			}
			else if (TalkBox_Judge == 2)//공격선택지
			{
				wsprintf(str, "%s %d/15    %s %d/10\n%s %d/5       %s %d/3", my_info[What_Poke].Skill1,my_info[What_Poke].Limit1, my_info[What_Poke].Skill2,my_info[What_Poke].Limit2 ,my_info[What_Poke].Skill3, my_info[What_Poke].Limit3,my_info[What_Poke].Skill4, my_info[What_Poke].Limit4);
				DrawText(memDC, str, -1, &fect, DT_LEFT);
				hBrush = CreateSolidBrush(RGB(0, 0, 255));
				oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
				Draw_MsgBox(memDC, MsgJudge);
				SelectObject(memDC, oldBrush);
				DeleteObject(hBrush);
			}
			else if (TalkBox_Judge == 3)//주인공 공격수행
			{
				if (Skill_Using==FALSE)
				My_Skill_Selection(str, Damage2, Skill_Selection,&Skill_Progress,hWnd);
				if (MsgJudge == 0)
				{
					wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", my_info[What_Poke].Skill1, Damage2);
					Skill[1].TransparentBlt(memDC, 700, 150, 50, 50, RGB(255, 255, 255));
				}
				else if (MsgJudge == 1)
				{
					wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", my_info[What_Poke].Skill2, Damage2);
					Skill[Skill_Progress].TransparentBlt(memDC, 700, 150, 50, 50, RGB(255, 255, 255));
				}
				else if (MsgJudge == 2)
				{
					wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", my_info[What_Poke].Skill3, Damage2);
					Skill[Skill_Progress].TransparentBlt(memDC, 700, 150, 50, 50, RGB(255, 255, 255));
				}
				else if (MsgJudge == 3)
				{
					wsprintf(str, "%s 스킬은 %d의 \n데미지를 주었다.", my_info[What_Poke].Skill4, Damage2);
					Skill[Skill_Progress].TransparentBlt(memDC, 700, 150, 150, 150, RGB(255, 255, 255));
				}
				DrawText(memDC, str, -1, &fect, DT_LEFT);

			}
			else if (TalkBox_Judge == 4)
			{
				if (EnemyTurn == FALSE)
				{
					Enemy_Attack(Skill_Selection, memDC, str, info, Respawn, fect, rectView, Damage1, MMovePos);
					MsgJudge = 0;
				}
				else
				{
					wsprintf(str, "%s의 차례", info[Respawn].name);
					DrawText(memDC, str, -1, &fect, DT_LEFT);
				}
			}
			else if (TalkBox_Judge == 5)//도망성공
			{
				wsprintf(str, "도망치는데 성공했다!!!");
				DrawText(memDC, str, -1, &fect, DT_LEFT);
			}
			else if (TalkBox_Judge == 6)//도망실패
			{
				wsprintf(str, "도망치는데 실패했다!!!");
				DrawText(memDC, str, -1, &fect, DT_LEFT);
			}
			else if (TalkBox_Judge == 7)
			{
				if (Dead)
				{
					wsprintf(str, "나는 죽었다.");
					DrawText(memDC, str, -1, &fect, DT_LEFT);
				}
				else
				{
					wsprintf(str, "적이 죽었다.");
					DrawText(memDC, str, -1, &fect, DT_LEFT);
					if (state == 5)
					{
						BOSSKILL = TRUE;
						KillTimer(hWnd, 1);
						FMOD_Channel_Stop(g_Channel[0]);
						hWndAVI2 = MCIWndCreate(hWnd, g_hinst, MCIWNDF_NOTIFYANSI | MCIWNDF_NOMENU | MCIWNDF_NOTIFYALL | MCIWNDF_NOPLAYBAR, AVfile2);
						if (hWndAVI2)
						{
							SetWindowPos(hWndAVI2, NULL, 0, 0, rectView.right, rectView.bottom, SWP_NOZORDER | SWP_NOMOVE);
							MCIWndPlay(hWndAVI2);
							childe = true;
						}
					}
				}
			}
			else if (TalkBox_Judge == 8)
			{
				if (EnemyTurn == FALSE)
				{
					Enemy_Attack(Skill_Selection, memDC, str, info, Respawn, fect, rectView, Damage1, MMovePos);
					MsgJudge = 0;
				}
				else
				{
					wsprintf(str, "Hp200을 회복하였다!");
					DrawText(memDC, str, -1, &fect, DT_LEFT);
				}
			}
			else if (TalkBox_Judge == 9)
			{
				wsprintf(str, "포켓몬을 포획했다!!!");
				DrawText(memDC, str, -1, &fect, DT_LEFT);
			}
			else if (TalkBox_Judge == 10)
			{
				if (EnemyTurn == FALSE)
				{
					Enemy_Attack(Skill_Selection, memDC, str, info, Respawn, fect, rectView, Damage1, MMovePos);
					MsgJudge = 0;
				}
				else
				{
					wsprintf(str, "이런! 포획에 실패했다!");
					DrawText(memDC, str, -1, &fect, DT_LEFT);
				}
			}
			else if (TalkBox_Judge == 11)
			{
				wsprintf(str, "");
				DrawText(memDC, str, -1, &fect, DT_LEFT);
			}
			else if (TalkBox_Judge == 12)
			{
				wsprintf(str, "");
				DrawText(memDC, str, -1, &fect, DT_LEFT);
				if (state == 1)
				{
					if (Skill_Selection == 1)
					{
						Skill[Skill_Progress].TransparentBlt(memDC, rectView.right / 10 + MMovePos + 200, rectView.bottom * 6 / 15, 150, 150, RGB(255, 255, 255));
					}
				}
				else if (state == 2 || state == 3 || state == 4||state==5)
				{
					if(Skill_Selection==0)
						Skill[Skill_Progress].TransparentBlt(memDC, rectView.right / 10 + MMovePos + 200, rectView.bottom * 6 / 15, 150, 150, RGB(255, 255, 255));
					else if (Skill_Selection == 1)
						Skill[Skill_Progress].TransparentBlt(memDC, rectView.right / 10 + MMovePos + 200, rectView.bottom * 6 / 15, 300, 300, RGB(255, 255, 255));

				}
			}//할퀴기스킬
			else if (TalkBox_Judge == 13)
			{
				Exchange_Screen.Draw(memDC, 0, 506, rectView.right, 240);
				lf.lfHeight = rectView.bottom / 20;
				Font = CreateFontIndirect(&lf);
				OldFont = (HFONT)SelectObject(memDC, Font);
				count = 0;
				Draw_Pokemon_BackPack(Bag, count, memDC, rectView, str);
				
				hBrush = CreateSolidBrush(RGB(0, 0, 255));
				oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
				if (MsgJudge == 0)
					bSelect.TransparentBlt(memDC, 40, 505, 50, 50, RGB(255, 255, 255));
				else if (MsgJudge == 1)
					bSelect.TransparentBlt(memDC, 550, 525, 50, 50, RGB(255, 255, 255));
				else if (MsgJudge == 2)
					bSelect.TransparentBlt(memDC, 40, 617, 50, 50, RGB(255, 255, 255));
				else if (MsgJudge == 3)
					bSelect.TransparentBlt(memDC, 550, 617, 50, 50, RGB(255, 255, 255));
				SelectObject(memDC, oldBrush);
				DeleteObject(hBrush);
			}
			else if (TalkBox_Judge == 14)
			{
				wsprintf(str, "수고했다 %s!!", my_info[What_Poke].name);
				DrawText(memDC, str, -1, &fect, DT_LEFT);
			}
			else if (TalkBox_Judge == 15)
			{
				if (EnemyTurn == FALSE)
				{
					Enemy_Attack(Skill_Selection, memDC, str, info, Respawn, fect, rectView, Damage1, MMovePos);
					MsgJudge = 0;
				}
			}
			img.Draw(hDC, 0, 0);
			img.ReleaseDC();
		}
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_KEYDOWN:
		if (Attack == FALSE&&BOSSKILL==FALSE)
		{
			if (wParam == VK_SPACE)
			{
				if (TalkBox_Judge == 0)
				{
					TalkBox_Judge = 1;
				}
				else if (TalkBox_Judge == 1)
				{
					if (MsgJudge == 0)//공격
					{
						TalkBox_Judge = 2;
					}
					else if (MsgJudge == 1)//아이템
					{
						DialogBox(g_hinst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Dlg6_1Proc);
						SetFocus(hWnd);
					}
					else if (MsgJudge == 2)//도망가기
					{
						int flee;
						flee = rand() % 50;
						if (flee > 24) TalkBox_Judge = 5;
						else TalkBox_Judge = 6;

					}
					else if (MsgJudge == 3)//교체
					{
						MsgJudge = 0;
						TalkBox_Judge = 13;
					}
				}
				else if (TalkBox_Judge == 2)
				{
					if (MsgJudge == 0)if (my_info[What_Poke].Limit1 == 0)break;
					if (MsgJudge == 1)if (my_info[What_Poke].Limit2 == 0)break;
					if (MsgJudge == 2)if (my_info[What_Poke].Limit3 == 0)break;
					if (MsgJudge == 3)if (my_info[What_Poke].Limit4 == 0)break;
					What_Skill_Selection(MsgJudge, &Skill_Selection, &Damage2, &Stack_Damage2, &EMovePos, &count, hWnd);
					TalkBox_Judge = 3;
				}
				else if (TalkBox_Judge == 3)
				{
					EnemyTurn = TRUE;
					TalkBox_Judge = 4;
					Skill_Using = FALSE;
				}
				else if (TalkBox_Judge == 4)
				{
					if (EnemyTurn)
					{
						Skill_Selection = rand() % 2;
						MMovePos = 0, count = 0;
						Enemy_Play(Skill_Selection, MMovePos, count, hWnd, &Damage1, EnemyLv, &Skill_Progress);
						EnemyTurn = FALSE;
					}
					else TalkBox_Judge = 1;
				}
				else if (TalkBox_Judge == 5)
				{
					int count = 0;
					for (int i = 0; i < 4; i++)
					{
						if (Bag[i] != 0)count++;
					}
					Save_Pokemon_info(count,Dead);
					DestroyWindow(hWnd);
				}
				else if (TalkBox_Judge == 6)
				{
					EnemyTurn = TRUE;
					TalkBox_Judge = 4;
				}
				else if (TalkBox_Judge == 7)
				{
					int count = 0;
					for (int i = 0; i < 4; i++)
					{
						if (Bag[i] != 0)count++;
					}
					Save_Pokemon_info(count,Dead);
					if(BOSSKILL==FALSE)
					DestroyWindow(hWnd);
				}
				else if (TalkBox_Judge == 8)
				{
					if (EnemyTurn)
					{
						Skill_Selection = rand() % 2;
						MMovePos = 0, count = 0;
						Enemy_Play(Skill_Selection, MMovePos, count, hWnd, &Damage1, EnemyLv, &Skill_Progress);
						EnemyTurn = FALSE;
					}
					else TalkBox_Judge = 1;
				}
				else if (TalkBox_Judge == 9)
				{
					int count=0;
					for (int i = 0; i < 4; i++)
					{
						if (Bag[i] != 0) 
						{
							my_info[i+1].Level++;
							count++;
						}
					}
					f = fopen("My.txt", "w");
					Catch_Pokemon(count, Respawn, info, f, EnemyLv, EnemyHp);
					fclose(f);
					DestroyWindow(hWnd);
				}
				else if (TalkBox_Judge == 10)
				{
					if (EnemyTurn)
					{
						Skill_Selection = rand() % 2;
						MMovePos = 0, count = 0;
						Enemy_Play(Skill_Selection, MMovePos, count, hWnd, &Damage1, EnemyLv, &Skill_Progress);
						EnemyTurn = FALSE;
					}
					else 
						TalkBox_Judge = 1;
				}
				else if (TalkBox_Judge == 11)break;//포켓몬 볼 던졋을때
				else if (TalkBox_Judge == 13)
				{
					if (Bag[MsgJudge] == 0 || my_info[MsgJudge+1].Stack_Damage >= my_info[MsgJudge+1].MaxHp||my_info[What_Poke].poke==my_info[MsgJudge+1].poke)
						break;
					else
						TalkBox_Judge = 14;
				}//교체
				else if (TalkBox_Judge == 14)
				{
					Pokemon_change(MsgJudge, Bag);
					EnemyTurn = TRUE;
					TalkBox_Judge = 15;
					if (EnemyTurn)
					{
						Skill_Selection = rand() % 2;
						MMovePos = 0, count = 0;
						Enemy_Play(Skill_Selection, MMovePos, count, hWnd, &Damage1, EnemyLv, &Skill_Progress);
						EnemyTurn = FALSE;
					}
				}
				else if (TalkBox_Judge == 15)
				{
					TalkBox_Judge = 1;
				}
			}
			else if (wParam == VK_TAB)
			{
				if (TalkBox_Judge == 2)
				{
					TalkBox_Judge = 1;
				}
				else if (TalkBox_Judge == 13)
				{
					TalkBox_Judge = 1;
				}
			}
		}
		if (TalkBox_Judge == 1 || TalkBox_Judge == 2 || TalkBox_Judge == 13)
		{
			if (wParam == VK_LEFT)
			{
				if (MsgJudge == 1)MsgJudge = 0;
				else if (MsgJudge == 3)MsgJudge = 2;
			}
			else if (wParam == VK_RIGHT)
			{
				if (MsgJudge == 0)MsgJudge = 1;
				else if (MsgJudge == 2)MsgJudge = 3;
			}
			else if (wParam == VK_UP)
			{
				if (MsgJudge == 2)MsgJudge = 0;
				else if (MsgJudge == 3)MsgJudge = 1;
			}
			else if (wParam == VK_DOWN)
			{
				if (MsgJudge == 0)MsgJudge = 2;
				else if (MsgJudge == 1)MsgJudge = 3;
			}
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_TIMER:
		switch (wParam) {
		case 1://이 타이머는 계속 돌아가면서 나 또는 적의 죽음을 판단해줌
			if (Stack_Damage2 >= EnemyHp) TalkBox_Judge = 7, Dead = FALSE;
			if (my_info[What_Poke].Stack_Damage >= my_info[What_Poke].MaxHp)TalkBox_Judge = 7, Dead = TRUE;
			break;
		case 2:
			Attack = TRUE;
			if (count % 2 == 0)//공격당했을때 좌우로 움직이는것(적)
			{
				EMovePos += 5;
				if (EMovePos == 15)count++;
			}
			else if (count % 2 == 1)
			{
				EMovePos -= 5;
				if (EMovePos == -15)count++;
			}
			if (count == 6)
			{
				Attack = FALSE;
				KillTimer(hWnd, 2);
				SetTimer(hWnd, 1, 100, NULL);
				EMovePos = 0;
			}
			break;
		case 3://공격당했을때 좌우로 움직이는것(나)
			Attack = TRUE;
			if (count % 2 == 0)
			{
				MMovePos += 5;
				if (MMovePos == 15)count++;
			}
			else if (count % 2 == 1)
			{
				MMovePos -= 5;
				if (MMovePos == -15)count++;
			}
			if (count == 6)
			{
				Attack = FALSE;
				KillTimer(hWnd, 3);
				SetTimer(hWnd, 1, 100, NULL);
				MMovePos = 0;
			}
			break;
		case 4://포켓몬볼 던지는 모션
			Show_Ball_Throw++;
			if (Show_Ball_Throw > 15)
			{
				KillTimer(hWnd, 4);
			}
			break;
		case 5://포켓몬볼 위아래 튕기는모션
			if (count == 0)
			{
				BallYpos += 15;
				if (BallYpos > 65) count++;
			}
			else if (count == 1)
			{
				BallYpos -= 15;
				if (BallYpos < 30)count++;
			}
			else if (count == 2)
			{
				BallYpos += 15;
				if (BallYpos == 75)count++;
			}
			else if (count == 3)
			{
				count = 0;
				KillTimer(hWnd, 5);
				Show_Ball_Throw++;
				SetTimer(hWnd, 6, 100, NULL);
			}
			break;
		case 6:
			if (count % 2 == 0)
			{
				Show_Ball_Throw++;
				if (Show_Ball_Throw == 19)count++;
			}
			else if (count % 2 == 1)
			{
				Show_Ball_Throw--;
				if (Show_Ball_Throw == 17)count++, Sleep(200);
			}
			if (count == 8)
			{
				count = 0;
				KillTimer(hWnd, 6);
				Show_Ball_Throw = 21;//종료
				SetTimer(hWnd, 1, 100, NULL);
			}
			break;
		case 7:
			Skill_Progress++;
			if (Skill_Progress == 6||Skill_Progress==18||Skill_Progress==22||Skill_Progress==26||Skill_Progress==28||Skill_Progress==33)
			{
				TalkBox_Judge = 4, KillTimer(hWnd, 7), SetTimer(hWnd, 1, 100, NULL);
			}
			break;
		case 8:
			Skill_Progress++;
			if (Skill_Progress == 6)
			{
				TalkBox_Judge = 3, KillTimer(hWnd, 8), SetTimer(hWnd, 1, 100, NULL);
			}
			break;
		case 9:
			Skill_Progress++;
			if (Skill_Progress == 10||Skill_Progress==18)
			{
				TalkBox_Judge = 3, KillTimer(hWnd, 9), SetTimer(hWnd, 1, 100, NULL);
			}
			break;
		case 10:
			Skill_Progress++;
			if (Skill_Progress == 14|| Skill_Progress == 22|| Skill_Progress == 26|| Skill_Progress == 28)
			{
				TalkBox_Judge = 3, KillTimer(hWnd, 10), SetTimer(hWnd, 1, 100, NULL);
			}
			break;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_CHAR:
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_LBUTTONDOWN:
		mx = LOWORD(IParam);
		my = HIWORD(IParam);//340 336 455 165 665 195
		break;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		childe = false;
		if (cmap == 5)
		{
			PlayBgm(12);
		}
		else if (cmap == 6 || cmap == 10)
		{
			PlayBgm(4);
		}
		else if (cmap == 7)
		{
			PlayBgm(5);
		}
		else if (cmap >= 11 && cmap <= 13)
		{
			PlayBgm(6);
		}
		else if (cmap == 8 || cmap == 9)
		{
			PlayBgm(7);
		}
		return 0;
	}
	return(DefWindowProcA(hWnd, iMessage, wParam, IParam));//위의 세 메세지 외의 나머지 메세지는 OS로
}