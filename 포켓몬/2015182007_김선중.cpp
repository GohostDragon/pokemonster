
#include "resource.h"
#include "2015182007_김선중.h"

HINSTANCE g_hinst;
LPCTSTR lpszClass = "Window Class Name";
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam);

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
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	//윈도우 클래스 등록
	RegisterClassEx(&WndClass);
	//윈도우 생성
	hWnd = CreateWindow(lpszClass, "window program", WS_OVERLAPPEDWINDOW, 0, 0, 1024, 768, NULL, (HMENU)NULL, hinstance, NULL);
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)
{
	PAINTSTRUCT ps;
	HDC hDC, hMemDC;
	HBITMAP bitmap, OldBitmap;
	HBRUSH hBrush, oldBrush,eBrush,ehBrush, unBrush;
	HBRUSH hBrush2, hBrush3, oldBrush2;
	HPEN MyPen,ePen, OldPen, cPen, unPen,iPen;
	HFONT Font,OldFont;
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

	static double effect[LIMIT_ENEMY+1][17];//사망 이펙트
	static double reffect[LIMIT_ENEMY][4] = { 0 };//리스폰 이펙트

	static double enemy[LIMIT_ENEMY][5] = { 0 };//적
	static int ecolor[3];//적 색깔
	static int ecount=0; //적 카운트
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
		SetGame(hWnd, &rectView,&tect, &dx, &dy, block, bullet, ecolor, &cx, &cy ,&seta, &reload
			, effect,reffect, enemy, &ecount, &etime, &death, &dcount, &life, &score, &combo
			,&ten,&gametime,level,sgun,&scount, multi);
		ZeroMemory(&lf, sizeof(lf));
		multireset(parray,rectView,dx,dy);
		return 0;
	case WM_PAINT:
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

		hMemDC = CreateCompatibleDC(hDC);
		bitmap = CreateCompatibleBitmap(hDC, rectView.right, rectView.bottom);
		OldBitmap = (HBITMAP)SelectObject(hMemDC, bitmap);

		//-------------------------------------------------------
		//보드판
		Font = CreateFontIndirect(&lf);
		OldFont = (HFONT)SelectObject(hMemDC, Font);
		oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
		Hscorebord(oldBrush, hBrush, hBrush2, eBrush, hMemDC, rectView, dx, dy, life, Font, OldFont, str, score, combo, tect, &lf,multi);

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
					Hshotbullet(bullet, regg, hMemDC, i, 0, ecolor,0);
					//총알-블록 충돌
					Hcolblock(dx, dy, regg, block, &score, &combo, bullet, i, 0,0);
					//총알-적 충돌
					Hcolenemy(dx, dy, regg, block, &score, &combo, bullet, i, effect, enemy, sgun, &scount, 0);
				}
				else if (bullet[i][0] == 4)
				{
					//총알 쏜거
					Hshotbullet(bullet, segg, hMemDC, i, 1, ecolor,0);
					//총알-블록 충돌
					Hcolblock(dx, dy, segg, block, &score, &combo, bullet, i, 1,0);
					//총알-적 충돌
					Hcolenemy(dx, dy, segg, block, &score, &combo, bullet, i, effect, enemy, sgun, &scount, 1);
				}
			}

			SelectObject(hMemDC, OldPen);
			oldBrush = (HBRUSH)SelectObject(hMemDC, eBrush);
			Hsgun(sgun, seta, dx, dy, hMemDC);
		}
		else
		{
			OldPen = (HPEN)SelectObject(hMemDC, cPen);
			//주인공
			oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush2);
			if (death == false)
			{
				oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush2);
				Rectangle(hMemDC, parray[0].cx - dx / 2, parray[0].cy - dy / 2, parray[0].cx + dx / 2, parray[0].cy + dy / 2);
				oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
				Rectangle(hMemDC, parray[1].cx - dx / 2, parray[1].cy - dy / 2, parray[1].cx + dx / 2, parray[1].cy + dy / 2);
			}
			OldPen = (HPEN)SelectObject(hMemDC, cPen);
			oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);

			//총알
			for (int i = 0; i < 6; i++)
			{
				//총알 도는거
				if (parray[0].bullet[i][0] == 1 || parray[0].bullet[i][0] == 2)
				{
					Hrotategun(parray[0].rx, parray[0].ry, parray[0].cx, parray[0].cy, dx, dy, seta, i, death, parray[0].bullet, oldBrush, hBrush, eBrush, hMemDC);
				}
				else if (parray[0].bullet[i][0] == 3)
				{
					//총알 쏜거
					Hshotbullet(parray[0].bullet, parray[0].regg, hMemDC, i, 0, ecolor, 0);
					//총알-블록 충돌
					Hcolblock(dx, dy, parray[0].regg, block, &score, &combo, parray[0].bullet, i, 0, 0);
					//총알-적 충돌
					Hcolplayer(parray[1].cx, parray[1].cy, dx, dy, parray[0].regg, effect, i, hMemDC, rectView, 0, str, hWnd, &death);
				}

				if (parray[1].bullet[i][0] == 1 || parray[1].bullet[i][0] == 2)
				{
					Hrotategun(parray[1].rx, parray[1].ry, parray[1].cx, parray[1].cy, dx, dy, seta, i, death, parray[1].bullet, oldBrush, hBrush2, eBrush, hMemDC);
				}
				else if (parray[1].bullet[i][0] == 3)
				{
					Hshotbullet(parray[1].bullet, parray[1].regg, hMemDC, i, 0, ecolor, 1);
					Hcolblock(dx, dy, parray[1].regg, block, &score, &combo, parray[1].bullet, i, 0, 1);
					Hcolplayer(parray[0].cx, parray[0].cy, dx, dy, parray[1].regg, effect, i, hMemDC, rectView, 1, str, hWnd, &death);
				}
			}
		}
		//사망이펙트
		Hdeatheffect(effect, oldBrush, hBrush2, eBrush, hMemDC);

		if(life == 0)
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
		//-----------------------------------------------------------------------------------
		BitBlt(hDC, 0, 0, rectView.right, rectView.bottom, hMemDC, 0, 0, SRCCOPY);
		SelectObject(hMemDC, OldBitmap);
		DeleteObject(bitmap);
		DeleteDC(hMemDC);

		EndPaint(hWnd, &ps);
		break;
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
				MoveBullet(rectView, bullet, 50, &combo,multi,regg,regg);
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
					MovePlayer(&cx, &cy, dx, dy, block, rectView, 10, 0);
				}
			}
			else
			{
				ReloadBullet(&reload, parray[0].bullet, 20);
				ReloadBullet(&reload, parray[1].bullet, 20);
				MoveBullet(rectView, parray[0].bullet, 50, &combo, multi, parray[0].regg, parray[1].regg);
				MoveBullet(rectView, parray[1].bullet, 50, &combo, multi, parray[1].regg, parray[0].regg);
				Tdetaheffect(effect);

				if (death == false)
				{
					MovePlayer(&parray[0].cx, &parray[0].cy, dx, dy, block, rectView, 10, 0);
					MovePlayer(&parray[1].cx, &parray[1].cy, dx, dy, block, rectView, 10, 1);
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
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_KEYDOWN:
		if (multi == false)
		{
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
		}
		else
		{
			if (wParam == VK_NUMPAD5)
			{
				Kshotbullet(parray[1].bullet, parray[1].cx, parray[1].cy, 2);
			}
			else if (wParam == VK_NUMPAD2)
			{
				Kshotbullet(parray[1].bullet, parray[1].cx, parray[1].cy, 3);
			}
			else if (wParam == VK_NUMPAD1)
			{
				Kshotbullet(parray[1].bullet, parray[1].cx, parray[1].cy, 1);
			}
			else if (wParam == VK_NUMPAD3)
			{
				Kshotbullet(parray[1].bullet, parray[1].cx, parray[1].cy, 0);
			}

			if (death == true)
			{
				if (wParam == VK_RETURN)
				{
					SetGame(hWnd, &rectView, &tect, &dx, &dy, block, bullet, ecolor, &cx, &cy, &seta, &reload
						, effect, reffect, enemy, &ecount, &etime, &death, &dcount, &life, &score, &combo
						, &ten, &gametime, level, sgun, &scount, multi);
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
			}
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_KEYUP:

		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_CHAR:
		if (multi == true)
		{
			if (wParam == 'u' || wParam == 'U')
			{
				Kshotbullet(parray[0].bullet, parray[0].cx, parray[0].cy, 2);
			}
			else if (wParam == 'j' || wParam == 'J')
			{
				Kshotbullet(parray[0].bullet, parray[0].cx, parray[0].cy, 3);
			}
			else if (wParam == 'h' || wParam == 'H')
			{
				Kshotbullet(parray[0].bullet, parray[0].cx, parray[0].cy, 1);
			}
			else if (wParam == 'k' || wParam == 'K')
			{
				Kshotbullet(parray[0].bullet, parray[0].cx, parray[0].cy, 0);
			}
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_40001://초급
			level = 0;
			SetGame(hWnd, &rectView, &tect, &dx, &dy, block, bullet, ecolor, &cx, &cy, &seta, &reload
				, effect, reffect, enemy, &ecount, &etime, &death, &dcount, &life, &score, &combo
				, &ten, &gametime, level,sgun,&scount, multi);
			multireset(parray, rectView, dx, dy);
			break;
		case ID_40002://중급
			level = 1;
			SetGame(hWnd, &rectView, &tect, &dx, &dy, block, bullet, ecolor, &cx, &cy, &seta, &reload
				, effect, reffect, enemy, &ecount, &etime, &death, &dcount, &life, &score, &combo
				, &ten, &gametime, level, sgun, &scount, multi);
			multireset(parray, rectView, dx, dy);
			break;
		case ID_40003://고급
			level = 2;
			SetGame(hWnd, &rectView, &tect, &dx, &dy, block, bullet, ecolor, &cx, &cy, &seta, &reload
				, effect, reffect, enemy, &ecount, &etime, &death, &dcount, &life, &score, &combo
				, &ten, &gametime, level, sgun, &scount, multi);
			multireset(parray, rectView, dx, dy);
			break;
		case ID_40004:
			multi = false;
			SetGame(hWnd, &rectView, &tect, &dx, &dy, block, bullet, ecolor, &cx, &cy, &seta, &reload
				, effect, reffect, enemy, &ecount, &etime, &death, &dcount, &life, &score, &combo
				, &ten, &gametime, level, sgun, &scount, multi);
			multireset(parray, rectView, dx, dy);
			break;

		case ID_40005:
			multi = true;
			SetGame(hWnd, &rectView, &tect, &dx, &dy, block, bullet, ecolor, &cx, &cy, &seta, &reload
				, effect, reffect, enemy, &ecount, &etime, &death, &dcount, &life, &score, &combo
				, &ten, &gametime, level, sgun, &scount, multi);
			multireset(parray, rectView, dx, dy);
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProcA(hWnd, iMessage, wParam, IParam));//위의 세 메세지 외의 나머지 메세지는 OS로
}