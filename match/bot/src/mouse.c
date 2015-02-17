/*
	$ Robot for LinLink   (C) 2005-2013 MF
	$ mouse.c
*/

#include "base.h"
#include "game.h"


/* ����λ�� */
static RECT	rect;

/* ��һ������λ�� */
static int	x0, y0;
/* ÿ������Ĵ�С */
static int	w0, h0, w1, h1;

/* ��ʾ������ͼ��λ�� */
int x1, y1;
int x2, y2;
/* ���������ͬ���ź� */
int bpclick;


/* ���� */
static void click(int x, int y, int right) {
	DWORD dw, up;

	if(right) {
		dw = MOUSEEVENTF_RIGHTDOWN;
		up = MOUSEEVENTF_RIGHTUP;
	} else {
		dw = MOUSEEVENTF_LEFTDOWN;
		up = MOUSEEVENTF_LEFTUP;
	}

	/* ����� */
	SetForegroundWindow(form);

	/* ��ȡ����λ�� */
	GetWindowRect(form, &rect);
	Sleep(50);

	/* ���㸺�� x, y */
	if(x < 0) {
		x = rect.right - rect.left + x;
	}
	if(y < 0) {
		y = rect.bottom - rect.top + y;
	}

	/* �ƶ���ָ��λ�� */
	SetCursorPos(rect.left + x, rect.top + y);
	Sleep(50);

	/* ������� */
	mouse_event(dw, 0, 0, 0, 0);
	Sleep(50);

	/* �ͷ���� */
	mouse_event(up, 0, 0, 0, 0);
	Sleep(20);
}

/* ����ĳ������ */
static void map(int x, int y) {
	click(x0 + x * w0 + w1, y0 + y * h0 + h1, 0);
}

/* ����հ״� */
static void blank() {
	click(-50, 50, 1);
}


/* Ҫ���û��������ͼ����ȷ��λ�� */
static int init() {
	int		success;
	int		datint;
	float	datfloat;

	POINT	mouse;
	DWORD	symbol;

	/* �ź����� */
	RESET(Run);
	/* ����״̬ */
	bpclick = -1;
	
	/* ��ȡ��ǰ���λ�� */
	GetCursorPos(&mouse);

	/* ����Ҽ��ͷŽ��� */
	blank();
	/* �����������ϵ� */
	click(-50, 50, 0);

	/* �ƶ���ԭ����λ�� */
	SetCursorPos(mouse.x, mouse.y);

	/* �ȴ������Ӧ */
	TIMEOUT(symbol, Run, 300) {
		;
	} else {
		return 0;
	}

	do {
		success = 0;

		/* ͼ���� */
		if(memoryRead(GAME_ICON_W, sizeof datint, &datint)) {
			w0 = datint;
			w1 = w0 / 2;
		} else {
			break;
		}
		/* ͼ���� */
		if(memoryRead(GAME_ICON_H, sizeof datint, &datint)) {
			h0 = datint;
			h1 = h0 / 2;
		} else {
			break;
		}

		/* ��߾� */
		if(memoryRead(bpclick + 0x30, sizeof datint, &datfloat)) {
			x0 = (int) datfloat;
		} else {
			break;
		}
		/* �ϱ߾� */
		if(memoryRead(bpclick + 0x34, sizeof datint, &datfloat)) {
			y0 = (int) datfloat;
		} else {
			break;
		}

		success = 1;
	} while(0);

	/* �ָ�״̬ */
	bpclick = 0;

	RESET(Run);
	SEND(Stop);

	return success;
}


THREAD(mouse) {
	DWORD	symbol;
	POINT	mouse;

	/* �ϴμ�¼ */
	int		oldx1, oldy1, oldx2, oldy2;
	/* ���Դ��� */
	int		retry = 0, duplicate = 0;

	/* �����ź� */
	RESET(Run);

	/* �������� */
	oldx1 = oldy1 = 0;
	oldx2 = oldy2 = 0;

	/* ��ȡ��һ��λ�� */
	if(init() == 0) {
		buttonRun(1);
		return;
	}

	/* ����ָ�� */
	GetCursorPos(&mouse);
	/* �ڿհ״����� */
	blank();

	while(1) {
		/* �����ʾ��ť */
		click(GAME_HINT_X, GAME_HINT_Y, 0);

		/* �ɹ��ź� */
		TIMEOUT(symbol, Run, 200) {
			retry = 0;

			/* �����ظ����ͬһ���ط� */
			if((x1 == oldx1) && (y1 == oldy1) && (x2 == oldx2) && (y2 == oldy2)) {
				if((++duplicate) >= 3) {
					break;
				}
			} else {
				duplicate = 0;

				oldx1 = x1;
				oldy1 = y1;
				oldx2 = x2;
				oldy2 = y2;
			}

			/* ��� x1 */
			map(x1, y1);
			Sleep(100);

			/* ��� x2 */
			map(x2, y2);
			Sleep(100);
		}

		/* ʧ���ź� */
		TIMEOUT(symbol, Stop, 0) {
			break;
		}

		/* ������� */
		if((++retry) < 3) {
			continue;
		}

		break;
	}

	/* �ָ���� */
	SetCursorPos(mouse.x, mouse.y);

	/* �ָ���ť */
	buttonRun(1);
}
