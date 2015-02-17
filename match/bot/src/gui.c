/*
	$ Robot for LinLink   (C) 2005-2013 MF
	$ gui.c
*/

#include "base.h"


/* ���� */
#define TITLE					"MF's Robot For LinLink5"
#define TITLE_ON				"[������] " TITLE
#define TITLE_OFF				"[��ֹͣ] " TITLE

/* ���� */
#define CLASSNAME				"mf_robot_class"
#define WIDTH					350
#define HEIGHT					80

/* ��ť */
#define ID_LOAD					1000
#define ID_RUN					1001


/* ��ť��� */
static HWND hLoad, hRun;
/* �����ھ�� */
HWND hWin = NULL;


/* ���ڻ��� */
static void draw(HWND hwnd) {
	int		i, w, h;
	int		ids[] = {ID_LOAD, ID_RUN};

	RECT	rect;
	HDC		hdc;
	HFONT	hfont;

	/* ��ȡ�ɻ����� */
	GetClientRect(hwnd, &rect);

	/* ���㰴ť��� */
	w = (rect.right - 15) / 2;
	h = rect.bottom - 10;

	/* ���ư�ť */
	hLoad	= CreateWindowEx(0, TEXT("BUTTON"), TEXT("������Ϸ����"), WS_CHILD | WS_VISIBLE, 5, 5,						w, h, hwnd, (HMENU) ID_LOAD,	instance, 0);
	hRun	= CreateWindowEx(0, TEXT("BUTTON"), TEXT("�����Զ�����"), WS_CHILD | WS_VISIBLE, rect.right - w - 5, 5,	w, h, hwnd, (HMENU) ID_RUN,		instance, 0);

	/* �豸 */
	hdc		= GetDC(hwnd);
	/* Ĭ������ */
	hfont	= (HFONT) GetStockObject(DEFAULT_GUI_FONT);

	SelectObject(hdc, hfont);
	ReleaseDC(hwnd, hdc);

	/* �������� */
	for(i = 0; i < (sizeof ids / sizeof(size_t)); i++) {
		SendDlgItemMessage(hwnd, ids[i], WM_SETFONT, (WPARAM) hfont, 0);
	}

	/* ���ð�ť */
	EnableWindow(hRun, FALSE);
}

/* ����� */
static void command(int id) {
	switch(id) {
		/* ���ذ�ť */
		case ID_LOAD:
			EnableWindow(hLoad,	FALSE);
			SEND(Run);
			break;

		/* ִ�й��� */
		case ID_RUN:
			buttonRun(0);
			THRDRUN(mouse);
			break;
	}
}

/* ��Ϣѭ�� */
static LRESULT CALLBACK message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch(msg) {
		/* ���ƴ��� */
		case WM_CREATE:		draw(hwnd);					return 0;
		/* ���ٴ��� */
		case WM_DESTROY:	PostQuitMessage(0);			return 0;
		/* ����ַ� */
		case WM_COMMAND:	command(LOWORD(wparam));	break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}


/* GUI �߳� */
THREAD(gui) {
	MSG						msg;
	RECT					rect;
	WNDCLASS				wndclass;

	/* ��ȡ�����С */
	GetWindowRect(GetDesktopWindow(), &rect);

	ZeroMemory(&wndclass, sizeof wndclass);

	wndclass.hInstance		= instance;
	wndclass.lpszClassName	= TEXT(CLASSNAME);
	wndclass.lpfnWndProc	= &message;
	wndclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH) COLOR_BTNSHADOW;

	/* ע�ᴰ���� */
	if(RegisterClass(&wndclass) == 0) {
		DIE("����ע�ᴰ����ʱ�������󣬱�����ֹ��");
	}

	/* ���������� */
	if((hWin = CreateWindowEx (
			/* ex style */	0,
			/* classname */	CLASSNAME,
			/* title */		TITLE,
			/* style */		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
			/* left */		(rect.right - WIDTH) / 2,
			/* top */		(rect.bottom - HEIGHT) / 2,
			/* width */		WIDTH,
			/* height */	HEIGHT,
			/* parent */	NULL,
			/* menu */		NULL,
			/* instance */	instance,
			/* lpparam */	0
		)) == NULL) {
			DIE("���򴴽�����ʧ�ܣ�������ֹ��");
	}

	ShowWindow(hWin, SW_SHOW);
	UpdateWindow(hWin);

	while(GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	SEND(Exit);
}


/* ���ذ�ť */
void buttonLoad(int enable) {
	EnableWindow(hLoad, (BOOL) enable);
}

/* ����/�رհ�ť */
void buttonRun(int enable) {
	EnableWindow(hRun, (BOOL) enable);
	
	if(enable) {
		SetWindowText(hWin, TEXT(TITLE_OFF));
	} else {
		SetWindowText(hWin, TEXT(TITLE_ON));
	}
}
