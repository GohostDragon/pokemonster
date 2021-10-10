#pragma once
#include <Windows.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#pragma comment(lib, "msimg32.lib")

#define LIMIT_ENEMY 20
#define LIMIT_SGUN 100

#define LIMIT_BGM 5
#define LIMIT_EFFECT 2

typedef struct player {
	double rx[7], ry[7];
	double cx, cy;
	double bullet[6][4] = { 0 };
	RECT regg[6];
}player;

void ReloadBullet(int *reload, double bullet[][4], int time)//총알 장전
{
	if (*reload < time)
	{
		(*reload)++;
	}
	else
	{
		for (int i = 0; i < 6; i++)
		{
			if (bullet[i][0] == 0)
			{
				bullet[i][0] = 1;
				*reload = 0;
				return;
			}
		}
	}
}

void ColRect(RECT rec, RECT &rec2, double *cx, double *cy)//검은벽 움직이지 못하게
{
	RECT temp;
	if (IntersectRect(&temp, &rec, &rec2))
	{
		if ((temp.bottom - temp.top) <= (temp.right - temp.left))
		{
			if (temp.top == rec2.top)
			{
				*cy -= temp.bottom - temp.top;
			}
			else if (temp.bottom == rec2.bottom)
			{
				*cy += temp.bottom - temp.top;
			}
		}
		else
		{
			if (temp.left == rec2.left)
			{
				*cx -= temp.right - temp.left;
			}
			else if (temp.right == rec2.right)
			{
				*cx += temp.right - temp.left;
			}
		}
	}
}

void MovePlayer(HWND hWnd,double *cx, double *cy, double dx, double dy, int block[][20], RECT rectView, int speed, int check)//주인공 이동
{
	RECT rec = { *cx - dx / 2,*cy - dy / 2,*cx + dx / 2,*cy + dy / 2 };
	
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
	{
		DestroyWindow(hWnd);
	}

	if (check == 0)
	{
		if (GetAsyncKeyState('w') & 0x8000 || GetAsyncKeyState('W') & 0x8000)
		{
			*cy -= speed;
		}

		if (GetAsyncKeyState('s') & 0x8000 || GetAsyncKeyState('S') & 0x8000)
		{
			*cy += speed;
		}

		if (GetAsyncKeyState('a') & 0x8000 || GetAsyncKeyState('A') & 0x8000)
		{
			*cx -= speed;
		}

		if (GetAsyncKeyState('d') & 0x8000 || GetAsyncKeyState('D') & 0x8000)
		{
			*cx += speed;
		}
		for (int i = 0; i < 20; i++)
		{
			for (int j = 0; j < 20; j++)
			{
				if (block[i][j] == 1 || block[i][j] == 2)
				{
					RECT rec2 = { dx * j ,dy * i ,dx * (j + 1),dy * (i + 1) };
					ColRect(rec, rec2, cx, cy);
				}
			}
		}
	}
	else
	{
		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			*cy -= speed;
		}

		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			*cy += speed;
		}

		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			*cx -= speed;
		}

		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			*cx += speed;
		}
		for (int i = 0; i < 20; i++)
		{
			for (int j = 0; j < 20; j++)
			{
				if (block[i][j] == 0 || block[i][j] == 2)
				{
					RECT rec2 = { dx * j ,dy * i ,dx * (j + 1),dy * (i + 1) };
					ColRect(rec, rec2, cx, cy);
				}
			}
		}
	}
	if (rectView.top > *cy - dy / 2)
		*cy += speed;

	if (rectView.bottom < *cy + dy / 2)
		*cy -= speed;

	if (rectView.left > *cx - dx / 2)
		*cx += speed;

	if (rectView.right < *cx + dx / 2)
		*cx -= speed;
}

void MoveEnemy(double enemy[][5], int cx, int cy)
{
	for (int i = 0; i < LIMIT_ENEMY; i++)
	{
		if (enemy[i][0] == 1)
		{
			if (enemy[i][1] < cx)
				enemy[i][1] += enemy[i][3];
			else
				enemy[i][1] -= enemy[i][3];

			if (enemy[i][2] < cy)
				enemy[i][2] += enemy[i][3];
			else
				enemy[i][2] -= enemy[i][3];
		}
	}
}

void MoveBullet(RECT rectView, double bullet[][4], int speed, int *combo, BOOL multi, RECT *regg, RECT *eegg)//총알 이동
{
	for (int i = 0; i < 6; i++)
	{
		if (bullet[i][0] == 3 || bullet[i][0] == 4)
		{
			if (bullet[i][3] == 0) // ->
			{
				if (bullet[i][1] - 50 < rectView.right)
				{
					bullet[i][1] += speed;
				}
				else
				{
					bullet[i][0] = 0;
					*combo = 0;
				}
			}
			else if (bullet[i][3] == 1)// <-
			{
				if (bullet[i][1] + 50 > rectView.left)
				{
					bullet[i][1] -= speed;
				}
				else
				{
					bullet[i][0] = 0;
					*combo = 0;
				}
			}
			else if (bullet[i][3] == 2)// 위
			{
				if (bullet[i][2] - 50 > rectView.top)
				{
					bullet[i][2] -= speed;
				}
				else
				{
					bullet[i][0] = 0;
					*combo = 0;
				}
			}
			else if (bullet[i][3] == 3)// 아래
			{
				if (bullet[i][2] + 50 < rectView.bottom)
				{
					bullet[i][2] += speed;
				}
				else
				{
					bullet[i][0] = 0;
					*combo = 0;
				}
			}

			if (multi == true)
			{
				for (int j = 0; j < 6; j++)
				{
					if (bullet[j][0] == 3 || bullet[j][0] == 4)
					{
						RECT temp;
						if (IntersectRect(&temp, &eegg[j], &regg[i]))
						{
							bullet[i][0] = 0;
							bullet[j][0] = 0;
							*combo = 0;
						}
					}
				}
			}
		}
	}
}

void SetGame(HWND hWnd, RECT *rectView, RECT *tect, double *dx, double *dy, int block[][20], double bullet[][4], int *ecolor, double *cx, double *cy, double *seta, int *reload,
	double effect[][17], double reffect[][4], double enemy[][5], int *ecount, int *etime, BOOL *death, int *dcount, int *life, int *score, int *combo
	, int *ten, int *gametime, int level, double sgun[][3], int *scount, BOOL multi)//게임 세팅
{
	GetClientRect(hWnd, rectView);
	if (level == 1)
	{
		*dx = rectView->right / 20;
		*dy = rectView->bottom / 14;
	}
	else if (level == 0)
	{
		*dx = rectView->right / 15;
		*dy = rectView->bottom / 10;
	}
	else if (level == 2)
	{
		*dx = rectView->right / 20;
		*dy = rectView->bottom / 14;
	}
	rectView->top = 2 * (*dy);
	*cx = rectView->right / 2;
	*cy = rectView->bottom / 2;
	if (level == 2)
	{
		*cy = rectView->bottom / 2 + *dy;
	}
	*tect = { 0,0,rectView->right * 1 / 3,rectView->top };
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			if (multi == false)
			{
				if (level == 1)
				{
					if (i > 3 && i < 10 && j > 5 && j < 15)
					{
						block[i][j] = 0;
					}
					else
					{
						block[i][j] = 1;
					}
				}
				else if (level == 0)
				{
					if (i > 3 && i < 9 && j > 5 && j < 10)
					{
						block[i][j] = 0;
					}
					else
					{
						block[i][j] = 1;
					}
				}
				else if (level == 2)
				{
					if (i > 6 && i < 9 && j>8 && j < 11)
					{
						block[i][j] = 0;
					}
					else if (j == 0 || j == 3 || j == 4 || j == 7 || j == 8 || j == 11 || j == 12 || j == 15 || j == 16 || j == 19 || i == 2 || i == 5 || i == 7 || i == 8 || i == 10 || i == 13)
					{
						block[i][j] = 1;
					}
					else
					{
						block[i][j] = 2;
					}
				}
			}
			else
			{
				if (level == 1)
				{
					if (j < 10)
					{
						block[i][j] = 0;
					}
					else
					{
						block[i][j] = 1;
					}
				}
				else if (level == 0)
				{
					if (j<7)
					{
						block[i][j] = 0;
					}
					else
					{
						block[i][j] = 1;
					}
				}
				else if (level == 2)
				{
					if ((j<10) && (i == 2 || i == 5 || i == 7 || i == 8 || i == 10 || i == 13))
					{
						block[i][j] = 0;
					}
					else if (j == 0 || j == 3 || j == 4 || j == 7 || j == 8)
					{
						block[i][j] = 0;
					}
					else if ((j == 1 && (i == 3 || i == 4)) || (j == 2 && (i == 3 || i == 4)))
					{
						block[i][j] = 0;
					}
					else if ((j >= 10) && (i == 2 || i == 5 || i == 7 || i == 8 || i == 10 || i == 13))
					{
						block[i][j] = 1;
					}
					else if (j == 11 || j == 12 || j == 15 || j == 16 || j == 19)
					{
						block[i][j] = 1;
					}
					else if ((j == 17 && (i == 11 || i == 12)) || (j == 18 && (i == 11 || i == 12)))
					{
						block[i][j] = 1;
					}
					else
					{
						block[i][j] = 2;
					}
				}
			}
		}
	}
	for (int i = 0; i < 6; i++)
	{
		bullet[i][0] = 0;
	}

	for (int i = 0; i < LIMIT_ENEMY; i++)
	{
		reffect[i][0] = 0;
		enemy[i][0] = 0;
	}

	for (int i = 0; i < LIMIT_ENEMY + 1; i++)
	{
		effect[i][0] = 0;
	}

	for (int i = 0; i < LIMIT_SGUN; i++)
	{
		sgun[i][0] = 0;
	}
	*seta = 0;//총알 돌리기 위한 세타
	*reload = 0;//총알 장전

	*ecount = 0; //적 카운트
	*etime = 30; // 적 생성시간

	*death = false;//플레이어 죽음
	*dcount = 0;//부활 카운트
	*life = 3;

	*score = 0;
	*combo = 0;

	*ten = 0;
	*gametime = 0;
	*scount = 0;
	ecolor[0] = rand() % 240 + 10;
	ecolor[1] = rand() % 240 + 10;
	ecolor[2] = rand() % 240 + 10;
	SetTimer(hWnd, 1, 100, NULL);
}

void DrawGr(HDC pDC, COLORREF start, COLORREF finish, RECT prect, BOOL direct)//그라데이션
{
	int rs, gs, bs;
	rs = GetRValue(start);
	gs = GetGValue(start);
	bs = GetBValue(start);
	int rf, gf, bf;
	rf = GetRValue(finish);
	gf = GetGValue(finish);
	bf = GetBValue(finish);

	TRIVERTEX vert[2];
	GRADIENT_RECT gRect;
	vert[0].x = prect.left;
	vert[0].y = prect.top;
	vert[0].Red = rs * 16 * 16;
	vert[0].Green = gs * 16 * 16;
	vert[0].Blue = bs * 16 * 16;
	vert[0].Alpha = 0x0000;
	vert[1].x = prect.right;
	vert[1].y = prect.bottom;
	vert[1].Red = rf * 16 * 16;
	vert[1].Green = gf * 16 * 16;
	vert[1].Blue = bf * 16 * 16;
	vert[1].Alpha = 0x0000;
	gRect.UpperLeft = 1;
	gRect.LowerRight = 0;
	if (direct)
		GradientFill(pDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_H);
	else
		GradientFill(pDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
}

void DeathEffect(double effect[][17], double x, double y, int dx, int i)//죽을때 이펙트
{
	//0-시간 1-x1 2-y1 3-x2 4-y2 5-x3 6-y3 7-x4 8-y4
	//9- 1지름 10-2지름 11-3지름 12-4지름 13~16+/-
	for (int j = 1; j<8; j += 2)
		effect[i][j] = x + rand() % 50 * cos(rand() % 3);
	for (int j = 2; j<9; j += 2)
		effect[i][j] = y + rand() % 50 * sin(rand() % 3);
	for (int j = 9; j<13; j++)
		effect[i][j] = rand() % (dx / 2) + 10;
	for (int j = 13; j<17; j++)
		effect[i][j] = rand() % 2;
	effect[i][0] = 5;
}

void RespawnEffect(double reffect[][4], double x, double y, int nu)//리스폰 할때 이펙트
{
	reffect[0][0] = 30;
	reffect[0][1] = x;
	reffect[0][2] = y;
	reffect[0][3] = nu;
}

void DeathEnemy(double enemy[][5], double effect[][17], int block[][20], double dx, double dy, int k, RECT temp, RECT erec, int *score, int *combo, double sgun[][3], int *s)//적 죽음
{
	enemy[k][0] = 0;
	POINT eec = { enemy[k][1] , enemy[k][2] };
	for (int y = 0; y < 20; y++)
	{
		for (int x = 0; x < 20; x++)
		{
			RECT rec = { dx * x, dy * y, dx * (x + 1), dy * (y + 1) };
			if (IntersectRect(&temp, &erec, &rec))
			{
				if (block[y][x] == 1)
					block[y][x] = 0;

				if (block[y][x] == 0)
				{
					if (PtInRect(&rec, eec))
					{
						int r = rand() % 4;
						for (int a = 0; a < r; a++)
						{
							sgun[*s][0] = 1;
							sgun[*s][1] = x *dx + dx / 2;
							sgun[*s][2] = y *dy + dy / 2;
							if (*s < LIMIT_SGUN - 1)
								(*s)++;
							else
								(*s) = 0;
						}
					}
				}
			}
		}
	}
	DeathEffect(effect, enemy[k][1], enemy[k][2], dx, k + 1);

	(*score) += 10;
	(*combo)++;
}

void Hshotbullet(double bullet[][4], RECT *regg, HDC hMemDC, int i, int check, int *ecolor, int ch)
{//총알 나가는거 생성
	if (bullet[i][3] == 0 || bullet[i][3] == 1)
	{
		if (check == 0)
		{
			regg[i].left = bullet[i][1] - 50;
			regg[i].top = bullet[i][2] - 5;
			regg[i].right = bullet[i][1] + 50;
			regg[i].bottom = bullet[i][2] + 5;
			if (ch == 0)
			{
				if (bullet[i][3] == 0)
					DrawGr(hMemDC, RGB(250, 250, 250), RGB(0, 0, 0), regg[i], TRUE);
				else
					DrawGr(hMemDC, RGB(0, 0, 0), RGB(250, 250, 250), regg[i], TRUE);
			}
			else
			{
				if (bullet[i][3] == 0)
					DrawGr(hMemDC, RGB(20, 20, 20), RGB(255, 255, 255), regg[i], TRUE);
				else
					DrawGr(hMemDC, RGB(255, 255, 255), RGB(20, 20, 20), regg[i], TRUE);
			}
		}
		else
		{
			regg[3 * i].left = bullet[i][1] - 50;
			regg[3 * i].top = bullet[i][2] - 5;
			regg[3 * i].right = bullet[i][1] + 50;
			regg[3 * i].bottom = bullet[i][2] + 5;

			regg[3 * i + 1].left = bullet[i][1] - 50;
			regg[3 * i + 1].top = bullet[i][2] + 45;
			regg[3 * i + 1].right = bullet[i][1] + 50;
			regg[3 * i + 1].bottom = bullet[i][2] + 55;

			regg[3 * i + 2].left = bullet[i][1] - 50;
			regg[3 * i + 2].top = bullet[i][2] - 55;
			regg[3 * i + 2].right = bullet[i][1] + 50;
			regg[3 * i + 2].bottom = bullet[i][2] - 45;
			if (bullet[i][3] == 0)
			{
				DrawGr(hMemDC, RGB(255, 255, 255), RGB(ecolor[0], ecolor[1], ecolor[2]), regg[3 * i], TRUE);
				DrawGr(hMemDC, RGB(255, 255, 255), RGB(ecolor[0], ecolor[1], ecolor[2]), regg[3 * i + 1], TRUE);
				DrawGr(hMemDC, RGB(255, 255, 255), RGB(ecolor[0], ecolor[1], ecolor[2]), regg[3 * i + 2], TRUE);
			}
			else
			{
				DrawGr(hMemDC, RGB(ecolor[0], ecolor[1], ecolor[2]), RGB(255, 255, 255), regg[3 * i], TRUE);
				DrawGr(hMemDC, RGB(ecolor[0], ecolor[1], ecolor[2]), RGB(255, 255, 255), regg[3 * i + 1], TRUE);
				DrawGr(hMemDC, RGB(ecolor[0], ecolor[1], ecolor[2]), RGB(255, 255, 255), regg[3 * i + 2], TRUE);
			}
		}
	}
	else
	{
		if (check == 0)
		{
			regg[i].left = bullet[i][1] - 5;
			regg[i].top = bullet[i][2] - 50;
			regg[i].right = bullet[i][1] + 5;
			regg[i].bottom = bullet[i][2] + 50;
			if (ch == 0)
			{
				if (bullet[i][3] == 2)
					DrawGr(hMemDC, RGB(0, 0, 0), RGB(250, 250, 250), regg[i], FALSE);
				else
					DrawGr(hMemDC, RGB(250, 250, 250), RGB(0, 0, 0), regg[i], FALSE);
			}
			else
			{
				if (bullet[i][3] == 2)
					DrawGr(hMemDC, RGB(255, 255, 255), RGB(20, 20, 20), regg[i], FALSE);
				else
					DrawGr(hMemDC, RGB(20, 20, 20), RGB(255, 255, 255), regg[i], FALSE);
			}
		}
		else
		{
			regg[3 * i].left = bullet[i][1] - 5;
			regg[3 * i].top = bullet[i][2] - 50;
			regg[3 * i].right = bullet[i][1] + 5;
			regg[3 * i].bottom = bullet[i][2] + 50;

			regg[3 * i + 1].left = bullet[i][1] + 45;
			regg[3 * i + 1].top = bullet[i][2] - 50;
			regg[3 * i + 1].right = bullet[i][1] + 55;
			regg[3 * i + 1].bottom = bullet[i][2] + 50;

			regg[3 * i + 2].left = bullet[i][1] - 55;
			regg[3 * i + 2].top = bullet[i][2] - 50;
			regg[3 * i + 2].right = bullet[i][1] - 45;
			regg[3 * i + 2].bottom = bullet[i][2] + 50;
			if (bullet[i][3] == 2)
			{
				DrawGr(hMemDC, RGB(ecolor[0], ecolor[1], ecolor[2]), RGB(255, 255, 255), regg[3 * i], FALSE);
				DrawGr(hMemDC, RGB(ecolor[0], ecolor[1], ecolor[2]), RGB(255, 255, 255), regg[3 * i + 1], FALSE);
				DrawGr(hMemDC, RGB(ecolor[0], ecolor[1], ecolor[2]), RGB(255, 255, 255), regg[3 * i + 2], FALSE);
			}
			else
			{
				DrawGr(hMemDC, RGB(255, 255, 255), RGB(ecolor[0], ecolor[1], ecolor[2]), regg[3 * i], FALSE);
				DrawGr(hMemDC, RGB(255, 255, 255), RGB(ecolor[0], ecolor[1], ecolor[2]), regg[3 * i + 1], FALSE);
				DrawGr(hMemDC, RGB(255, 255, 255), RGB(ecolor[0], ecolor[1], ecolor[2]), regg[3 * i + 2], FALSE);
			}
		}
	}
}

void Hcolblock(double dx, double dy, RECT *regg, int block[][20], int *score, int *combo, double bullet[][4], int i, int check, int ch)
{
	//총알-블록 충돌
	for (int y = 0; y < 20; y++)
	{
		for (int x = 0; x < 20; x++)
		{
			RECT rec = { dx * x, dy * y, dx * (x + 1), dy * (y + 1) };
			RECT temp;
			if (check == 0)
			{
				if (IntersectRect(&temp, &regg[i], &rec))
				{
					if (ch == 0)
					{
						if (block[y][x] == 1)
						{
							block[y][x] = 0;
							*score += 5;
						}
						else if (block[y][x] == 2)
						{
							bullet[i][0] = 0;
							*combo = 0;
						}
					}
					else
					{
						if (block[y][x] == 0)
						{
							block[y][x] = 1;
							*score += 5;
						}
						else if (block[y][x] == 2)
						{
							bullet[i][0] = 0;
							*combo = 0;
						}
					}
				}
			}
			else
			{
				if (IntersectRect(&temp, &regg[i * 3], &rec) || IntersectRect(&temp, &regg[i * 3 + 1], &rec) || IntersectRect(&temp, &regg[i * 3 + 2], &rec))
				{
					if (block[y][x] == 1)
					{
						block[y][x] = 0;
						*score += 5;
					}
				}
			}
		}
	}
}

void Hcolenemy(double dx, double dy, RECT *regg, int block[][20], int *score, int *combo, double bullet[][4], int i, double effect[][17], double enemy[][5], double sgun[][3], int *scount, int check)
{
	//총알-적 충돌
	for (int k = 0; k < LIMIT_ENEMY; k++)
	{
		if (enemy[k][0] == 1)
		{
			RECT rec = { enemy[k][1] - dx / 2, enemy[k][2] - dy / 2, enemy[k][1] + dx / 2, enemy[k][2] + dy / 2 };
			RECT erec = { enemy[k][1] - 2 * dx / 2,enemy[k][2] - 2 * dy / 2 ,enemy[k][1] + 2 * dx / 2,enemy[k][2] + 2 * dy / 2 };
			RECT temp;
			if (check == 0)
			{
				if (IntersectRect(&temp, &regg[i], &rec))
				{
					DeathEnemy(enemy, effect, block, dx, dy, k, temp, erec, score, combo, sgun, scount);
				}
			}
			else
			{
				if (IntersectRect(&temp, &regg[i * 3], &rec) || IntersectRect(&temp, &regg[i * 3 + 1], &rec) || IntersectRect(&temp, &regg[i * 3 + 2], &rec))
				{

					DeathEnemy(enemy, effect, block, dx, dy, k, temp, erec, score, combo, sgun, scount);
				}
			}
		}
	}
}

void Hgamewin(HDC hMemDC, RECT rectView, int check, char *str, HWND hWnd)//일대일 승패 메세지
{
	SetBkColor(hMemDC, RGB(255, 255, 255));
	RECT aect = { rectView.left,rectView.bottom * 1 / 4,rectView.right,rectView.bottom * 3 / 4 };
	if (check == 0)
		wsprintf(str, "Player BLACK WIN\nPress Enter key to reset game");
	else
		wsprintf(str, "Player WHITE WIN\nPress Enter key to reset game");
	DrawText(hMemDC, str, -1, &aect, DT_CENTER);
	KillTimer(hWnd, 1);
}

void Hcolplayer(double cx, double cy, double dx, double dy, RECT *regg, double effect[][17], int i, HDC hMemDC, RECT rectView, int check, char *str, HWND hWnd, BOOL *death)//일대일 총알 맞음
{
	RECT rec = { cx - dx / 2, cy - dy / 2, cx + dx / 2, cy + dy / 2 };
	RECT temp;
	if (IntersectRect(&temp, &regg[i], &rec))
	{
		DeathEffect(effect, cx, cy, dx, i);
		Hgamewin(hMemDC, rectView, check, str, hWnd);
		*death = true;
	}
}

void Hcreateboad(int block[][20], double dx, double dy, HBRUSH hBrush, HBRUSH hBrush2, HBRUSH hBrush3, HBRUSH oldBrush, HDC hMemDC)
{//보드판
	for (int i = 2; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			if (block[i][j] == 0)//흰색
			{
				oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
				Rectangle(hMemDC, dx * j, dy * i, dx * (j + 1), dy * (i + 1));
			}
			else if (block[i][j] == 1)//검정
			{
				oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush2);
				Rectangle(hMemDC, dx * j, dy * i, dx * (j + 1), dy * (i + 1));
			}
			else if (block[i][j] == 2)//회색
			{
				oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush3);
				Rectangle(hMemDC, dx * j, dy * i, dx * (j + 1), dy * (i + 1));
			}
		}
	}
}

void Hchangeblock(int block[][20], int x, int y, RECT rec, POINT *point, double dx, double dy, double sgun[][3], RECT erec)
{
	//적에 의해 블록 변화
	if (PtInRect(&rec, point[4]))
	{
		if (block[y][x] == 0)
			block[y][x] = 1;
	}
	for (int s = 0; s < LIMIT_SGUN; s++)
	{
		if (sgun[s][0] == 1)
		{
			RECT sec = { sgun[s][1] - dx / 2 , sgun[s][2] - dy / 2, sgun[s][1] + dx / 2 , sgun[s][2] + dy / 2 };
			RECT temp;
			if (IntersectRect(&temp, &erec, &sec))
			{
				sgun[s][0] = 0;
			}
		}
	}
}

void Henemyhatch(RECT rec, POINT *point, int block[][20], int x, int y, double dx, double dy, HBRUSH ehBrush, HBRUSH oldBrush, HDC hMemDC)
{
	//적 주변의 빗금
	for (int k = 0; k < 9; k++)
	{
		if (PtInRect(&rec, point[k]))
		{
			if (block[y][x] == 0)
				SetBkColor(hMemDC, RGB(255, 255, 255));
			else if (block[y][x] == 1)
				SetBkColor(hMemDC, RGB(0, 0, 0));
			else if (block[y][x] == 2)
				SetBkColor(hMemDC, RGB(125, 125, 125));
			oldBrush = (HBRUSH)SelectObject(hMemDC, ehBrush);
			Rectangle(hMemDC, dx * x, dy * y, dx * (x + 1), dy * (y + 1));
		}
	}
}

void Henemycol(double enemy[][5], double dx, double dy, double cx, double cy, BOOL *infi, BOOL *death, double effect[][17], int *life, int *dcount, int i)
{
	//적-플레이어 충돌체크
	RECT temp;
	RECT rec = { enemy[i][1] - dx / 2, enemy[i][2] - dy / 2, enemy[i][1] + dx / 2, enemy[i][2] + dy / 2 };
	RECT rec2 = { cx - dx / 2, cy - dy / 2, cx + dx / 2, cy + dy / 2 };
	if (IntersectRect(&temp, &rec, &rec2))
	{
		if (*infi == false)
		{
			if (*death == FALSE)
			{
				*death = TRUE;
				DeathEffect(effect, cx, cy, dx, 0);
				*dcount = 20;
				if (*life > 0)
				{
					(*life)--;
				}
			}
		}
	}
}

void Henemy(double enemy[][5], int block[][20], double sgun[][3], double dx, double dy, double cx, double cy, BOOL *infi, BOOL *death, double effect[][17], int *life, int *dcount,
	HPEN ePen, HPEN OldPen, HBRUSH ehBrush, HBRUSH oldBrush, HDC hMemDC)
{
	//적
	for (int i = 0; i < LIMIT_ENEMY; i++)
	{
		OldPen = (HPEN)SelectObject(hMemDC, ePen);
		if (enemy[i][0] == 1)
		{
			POINT point[9] = { { enemy[i][1] - 2 * dx / 2,enemy[i][2] - 2 * dy / 2 },{ enemy[i][1] - 2 * dx / 2,enemy[i][2] },{ enemy[i][1] - 2 * dx / 2,enemy[i][2] + 2 * dy / 2 },
			{ enemy[i][1], enemy[i][2] - 2 * dy / 2 },{ enemy[i][1], enemy[i][2] },{ enemy[i][1], enemy[i][2] + 2 * dy / 2 },
			{ enemy[i][1] + 2 * dx / 2,enemy[i][2] + 2 * dy / 2 },{ enemy[i][1] + 2 * dx / 2,enemy[i][2] },{ enemy[i][1] + 2 * dx / 2,enemy[i][2] - 2 * dy / 2 } };
			RECT erec = { enemy[i][1] - dx / 2, enemy[i][2] - dy / 2,enemy[i][1] + dx / 2, enemy[i][2] + dy / 2 };
			for (int y = 2; y < 20; y++)
			{
				for (int x = 0; x < 20; x++)
				{
					RECT rec = { dx * x, dy * y, dx * (x + 1), dy * (y + 1) };
					//적에 의해 블록 변화
					Hchangeblock(block, x, y, rec, point, dx, dy, sgun, erec);
					//적 주변의 빗금
					Henemyhatch(rec, point, block, x, y, dx, dy, ehBrush, oldBrush, hMemDC);
				}
			}
			//적-플레이어 충돌체크
			Henemycol(enemy, dx, dy, cx, cy, infi, death, effect, life, dcount, i);
		}
	}
}

void Hrespwan(HPEN OldPen, HBRUSH oldBrush, HBRUSH unBrush, HBRUSH hBrush2, HBRUSH eBrush, HPEN cPen, HPEN ePen, HDC hMemDC, double dx, double dy, double reffect[][4])
{//부활 이펙트
	for (int i = 0; i < LIMIT_ENEMY; i++)
	{
		//부활 이펙트
		if (reffect[i][0] > 0)
		{
			if (reffect[i][3] == -1)
				OldPen = (HPEN)SelectObject(hMemDC, cPen);
			else
				OldPen = (HPEN)SelectObject(hMemDC, ePen);
			oldBrush = (HBRUSH)SelectObject(hMemDC, unBrush);
			Rectangle(hMemDC, reffect[i][1] - dx / 2 - reffect[i][0], reffect[i][2] - dy / 2 - reffect[i][0], reffect[i][1] + dx / 2 + reffect[i][0], reffect[i][2] + dy / 2 + reffect[i][0]);
			if (reffect[i][3] == -1)
				oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush2);
			else
				oldBrush = (HBRUSH)SelectObject(hMemDC, eBrush);
			Rectangle(hMemDC, reffect[i][1] - dx / 2 + reffect[i][0], reffect[i][2] - dy / 2 + reffect[i][0], reffect[i][1] + dx / 2 - reffect[i][0], reffect[i][2] + dy / 2 - reffect[i][0]);
		}
	}
}

void Hsgun(double sgun[][3], double seta, double dx, double dy, HDC hMemDC)//보드판 위에 특수총알
{
	for (int i = 0; i < LIMIT_SGUN; i++)
	{
		if (sgun[i][0] == 1)
		{
			int gx, gy;
			gx = dx / 3 * cos(seta + i) + sgun[i][1];
			gy = dy / 3 * sin(seta + i) + sgun[i][2];
			Ellipse(hMemDC, gx - dx / 10, gy - dx / 10, gx + dx / 10, gy + dx / 10);
		}
	}
}

void Hdeatheffect(double effect[][17], HBRUSH oldBrush, HBRUSH hBrush2, HBRUSH eBrush, HDC hMemDC)//데스이펙트
{
	for (int i = 0; i < LIMIT_ENEMY + 1; i++)
	{
		if (i == 0)
			oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush2);
		else
			oldBrush = (HBRUSH)SelectObject(hMemDC, eBrush);
		if (effect[i][0] > 0)
		{
			Ellipse(hMemDC, effect[i][1] - effect[i][9], effect[i][2] - effect[i][9], effect[i][1] + effect[i][9], effect[i][2] + effect[i][9]);
			Ellipse(hMemDC, effect[i][3] - effect[i][10], effect[i][4] - effect[i][10], effect[i][3] + effect[i][10], effect[i][4] + effect[i][10]);
			Ellipse(hMemDC, effect[i][5] - effect[i][11], effect[i][6] - effect[i][11], effect[i][5] + effect[i][11], effect[i][6] + effect[i][11]);
			Ellipse(hMemDC, effect[i][7] - effect[i][12], effect[i][8] - effect[i][12], effect[i][7] + effect[i][12], effect[i][8] + effect[i][12]);
		}
	}
}

void Hgameover(HDC hMemDC, RECT rectView, int score, char *str, HWND hWnd)//게임오버메세지
{
	SetBkColor(hMemDC, RGB(255, 255, 255));
	RECT aect = { rectView.left,rectView.bottom * 1 / 4,rectView.right,rectView.bottom * 3 / 4 };
	wsprintf(str, "Game Over\n%d\nPress Enter key to reset game", score);
	DrawText(hMemDC, str, -1, &aect, DT_CENTER);
	KillTimer(hWnd, 1);
}

void Hscorebord(HBRUSH oldBrush, HBRUSH hBrush, HBRUSH hBrush2, HBRUSH eBrush, HDC hMemDC, RECT rectView, double dx, double dy, int life, HFONT Font, HFONT OldFont, char *str, int score, int combo, RECT tect, LOGFONT *lf, BOOL multi)
{//상단 스코어보드판
	oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
	Rectangle(hMemDC, 0, 0, rectView.right, rectView.top);
	lf->lfHeight = dy;
	OldFont = (HFONT)SelectObject(hMemDC, Font);
	if (multi == false)
	{
		wsprintf(str, "score\n%d", score);
		DrawText(hMemDC, str, -1, &tect, DT_LEFT);
		if (combo > 0)
		{
			RECT aect = { rectView.right * 1 / 3,0,rectView.right * 2 / 3,rectView.top };
			wsprintf(str, "combo x%d", combo);
			DrawText(hMemDC, str, -1, &aect, DT_LEFT);
		}

		oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush2);
		if (life > 0)
		{
			Rectangle(hMemDC, rectView.right * 2 / 3, dy / 2, rectView.right * 2 / 3 + dx, rectView.top - dy / 2);
			if (life > 1)
			{
				Rectangle(hMemDC, rectView.right * 2 / 3 + 2 * dx, dy / 2, rectView.right * 2 / 3 + 3 * dx, rectView.top - dy / 2);
				if (life > 2)
				{
					Rectangle(hMemDC, rectView.right * 2 / 3 + 4 * dx, dy / 2, rectView.right * 2 / 3 + 5 * dx, rectView.top - dy / 2);
				}
			}
		}
	}
	else
	{
		wsprintf(str, "versus mode");
		DrawText(hMemDC, str, -1, &tect, DT_LEFT);
	}
}

void Hplayersgun(double sgun[][3], double dx, double dy, double bullet[][4], double cx, double cy, int *score)//플레이어가 특수총알 먹는거
{
	RECT cec = { cx - dx / 2, cy - dy / 2, cx + dx / 2, cy + dy / 2 };
	for (int i = 0; i < LIMIT_SGUN; i++)
	{
		if (sgun[i][0] == 1)
		{
			RECT sec = { sgun[i][1] - dx / 2 , sgun[i][2] - dy / 2, sgun[i][1] + dx / 2 , sgun[i][2] + dy / 2 };
			RECT temp;
			if (IntersectRect(&temp, &sec, &cec))
			{
				for (int i = 0; i < 6; i++)
				{
					if (bullet[i][0] == 0)
					{
						bullet[i][0] = 2;
						break;
					}

					if (i == 5)
					{
						if (bullet[i][0] == 0)
						{
							bullet[i][0] = 2;
							break;
						}
						else
						{
							for (int i = 0; i < 6; i++)
							{
								if (bullet[i][0] == 1)
								{
									bullet[i][0] = 2;
									break;
								}
							}
						}
					}
				}
				sgun[i][0] = 0;
				*score += 5;
			}
		}
	}
}

void Hrotategun(double *rx, double *ry, double cx, double cy, double dx, double dy, double seta, int i, BOOL death, double bullet[][4], HBRUSH oldBrush, HBRUSH hBrush, HBRUSH eBrush, HDC hMemDC)
{//총알 돌아가는거
	rx[i] = dx / 3 * cos(seta + i) + cx;
	ry[i] = dy / 3 * sin(seta + i) + cy;
	if (death == false)
	{
		if (bullet[i][0] == 1)
			oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
		else
			oldBrush = (HBRUSH)SelectObject(hMemDC, eBrush);
		Ellipse(hMemDC, rx[i] - dx / 10, ry[i] - dx / 10, rx[i] + dx / 10, ry[i] + dx / 10);
	}
}

void Kshotbullet(double bullet[][4], double cx, double cy, int check)//방향키 총알 발사
{
	for (int i = 0; i < 6; i++)
	{
		if (bullet[i][0] == 1 || bullet[i][0] == 2)
		{
			if (bullet[i][0] == 1)
				bullet[i][0] = 3;
			else
				bullet[i][0] = 4;

			if (check == 0)
			{
				bullet[i][1] = cx + 50;
				bullet[i][2] = cy;
			}
			else if (check == 1)
			{
				bullet[i][1] = cx - 50;
				bullet[i][2] = cy;
			}
			else if (check == 2)
			{
				bullet[i][1] = cx;
				bullet[i][2] = cy - 50;
			}
			else if (check == 3)
			{
				bullet[i][1] = cx;
				bullet[i][2] = cy + 50;
			}
			bullet[i][3] = check;
			break;
		}
	}
}

void Tdetaheffect(double effect[][17])//데스이펙타임
{
	for (int i = 0; i < LIMIT_ENEMY + 1; i++)
	{
		if (effect[i][0] > 0)
		{
			for (int j = 13; j < 17; j++)
			{
				if (effect[i][j] == 0)
				{
					effect[i][j - 4] -= 5;
				}
				else
				{
					effect[i][j - 4] += 5;
				}
			}
			effect[i][0]--;
		}
	}
}

void Trespawneffect(double reffect[][4], int gametime, BOOL *death, double enemy[][5])//리스폰이펙타임 
{
	for (int i = 0; i < LIMIT_ENEMY; i++)
	{
		if (reffect[i][0] > 0)
		{
			if (gametime >= 60)
				reffect[i][0] -= 5;
			else
				reffect[i][0] -= 3;
			if (reffect[i][0] == 0)
			{
				if (reffect[i][3] == -1)//주인공
				{
					*death = false;
				}
				else//적
				{
					enemy[(int)reffect[i][3]][0] = 1;
				}
				reffect[i][0] = -1;
			}
		}
	}
}

void Tenemyspwantime(int *ecount, int gametime, int *etime)//적 스폰 시간 조절
{
	if (*ecount < LIMIT_ENEMY - 1)
		(*ecount)++;
	else
		*ecount = 0;
	if (gametime < 20)
	{
		*etime = 25;
	}
	else if (gametime >= 20 && gametime < 40)
	{
		*etime = 20;
	}
	else if (gametime >= 40 && gametime < 60)
	{
		*etime = 10;
	}
	else if (gametime >= 60)
	{
		*etime = 5;
	}
}

void Tcreateenemy(double cy, RECT rectView, double enemy[][5], int ecount)//적생성
{
	if (cy < rectView.bottom * 1 / 2)
	{
		enemy[ecount][1] = rand() % rectView.right;
		enemy[ecount][2] = rand() % (rectView.bottom * 1 / 2) + rectView.top;
	}
	else
	{
		enemy[ecount][1] = rand() % rectView.right;
		enemy[ecount][2] = rand() % (rectView.bottom * 1 / 2) + (rectView.bottom * 1 / 2);
	}
	enemy[ecount][3] = rand() % 3 + 5;
}

void Trespawnclear(int level, int block[][20], double enemy[][5], double effect[][17], double dx, double dy)//리스폰 주변 적 제거
{
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			if (level == 1)
			{
				if (i > 3 && i < 10 && j > 5 && j < 15)
				{
					block[i][j] = 0;
				}
			}
			else if (level == 0)
			{
				if (i > 3 && i < 9 && j > 5 && j < 10)
				{
					block[i][j] = 0;
				}
			}
			else if (level == 2)
			{
				if (i > 6 && i < 9 && j > 9 && j < 12)
				{
					block[i][j] = 0;
				}
			}

			for (int k = 0; k < LIMIT_ENEMY; k++)
			{
				if (enemy[k][0] == 1)
				{
					if (level == 1)
					{
						if (5 * dx <= enemy[k][1] && enemy[k][1] <= 15 * dx && 3 * dy <= enemy[k][2] && enemy[k][2] <= 10 * dy)
						{
							enemy[k][0] = 0;
							DeathEffect(effect, enemy[k][1], enemy[k][2], dx, k + 1);
						}
					}
					else if (level == 0)
					{
						if (5 * dx < enemy[k][1] && enemy[k][1] < 10 * dx && 3 * dy < enemy[k][2] && enemy[k][2] < 9 * dy)
						{
							enemy[k][0] = 0;
							DeathEffect(effect, enemy[k][1], enemy[k][2], dx, k + 1);
						}
					}
					else if (level == 2)
					{
						if (7 * dx < enemy[k][1] && enemy[k][1] < 13 * dx && 5 * dy < enemy[k][2] && enemy[k][2] < 10 * dy)
						{
							enemy[k][0] = 0;
							DeathEffect(effect, enemy[k][1], enemy[k][2], dx, k + 1);
						}
					}
				}
			}
		}
	}
}

void multireset(player *parray,RECT rectView,double dx,double dy)
{
	for (int i = 0; i < 2; i++)
	{
		for (int k = 0; k<6; k++)
			parray[i].bullet[k][0] = 0;
	}

	parray[0].cx = dx * 2;
	parray[0].cy = dy * 4;

	parray[1].cx = rectView.right - dx * 2;
	parray[1].cy = rectView.bottom - dy * 2;
}