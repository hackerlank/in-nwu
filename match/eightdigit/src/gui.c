/*
	$ gui.c, 0.1.0115
*/

#include <Windows.h>
#include <WindowsX.h>
#include <Commctrl.h>

#include "base.h"


/* ������Ϣ */
#define CLASSNAME				"mf_eightdigit_class"
#define TITLE					""
#define WIDTH					700
#define HEIGHT					250

/* ����Ԫ�� */
#define ID_RECT					1000
#define ID_DIGIT_1				1001
#define ID_DIGIT_2				1002
#define ID_DIGIT_3				1003
#define ID_DIGIT_4				1004
#define ID_DIGIT_5				1005
#define ID_DIGIT_6				1006
#define ID_DIGIT_7				1007
#define ID_DIGIT_8				1008
#define ID_ENTER_LABEL			2000
#define ID_ENTER_TEXT			2001
#define ID_ENTER_TIP_1			2002
#define ID_ENTER_TIP_2			2003
#define ID_STEP_LABEL			3000
#define ID_STEP_TEXT			3001
#define ID_STEP_TIP				3002
#define ID_CALC					4000
#define ID_PLAY					4001


/* �����ھ�� */
HWND		hWin;
/* ����ʵ�� */
HINSTANCE	instance;

/* ���ؼ���� */
static HWND hDigit[8], hEnter, hStep, hCalc, hPlay;


/*
	$ draw					���ƴ���

	@ HWND hwnd
*/
static void draw(HWND hwnd) {
	size_t ids[] = {
		ID_RECT,
		ID_DIGIT_1, ID_DIGIT_2, ID_DIGIT_3, ID_DIGIT_4, ID_DIGIT_5, ID_DIGIT_6, ID_DIGIT_7, ID_DIGIT_8,
		ID_ENTER_LABEL, ID_ENTER_TEXT, ID_ENTER_TIP_1, ID_ENTER_TIP_2,
		ID_STEP_LABEL, ID_STEP_TEXT, ID_STEP_TIP, ID_CALC, ID_PLAY
	};

	size_t i;

	RECT	rect;
	HDC		hdc;
	HFONT	hfont;

	/* ��ȡ�ɻ����� */
	GetClientRect(hwnd, &rect);

	/* �������ֱ߿� */
	CreateWindowEx(WS_EX_STATICEDGE, TEXT("STATIC"), NULL, WS_CHILD | WS_VISIBLE, 13, 13, 185, 185, hwnd, (HMENU) ID_RECT, instance, 0);

	/* �������� */
	for(i = 0; i < 8; i++) {
		hDigit[i] = CreateWindowEx(0, TEXT("BUTTON"), TEXT("?"), WS_CHILD | WS_VISIBLE, 15 + 60 * (i % 3), 15 + 60 * (i / 3), 60, 60, hwnd, (HMENU) (ID_DIGIT_1 + i), instance, 0);
	}

	/* �������� */
	CreateWindowEx(0, TEXT("STATIC"), TEXT("��ʼ���У�"), WS_CHILD | WS_VISIBLE, 220, 18, 100, 20, hwnd, (HMENU) ID_ENTER_LABEL, instance, 0);
	CreateWindowEx(0, TEXT("STATIC"), TEXT("�����Ϸ������ʼ���������У��������� 1~8 �Ϳո�"), WS_CHILD | WS_VISIBLE, 220, 65, 500, 20, hwnd, (HMENU) ID_ENTER_TIP_1, instance, 0);
	CreateWindowEx(0, TEXT("STATIC"), TEXT("�ַ�֮�䲻Ҫ�ָ���1~8 ֮����ַ�������Ϊ�ǿո�"), WS_CHILD | WS_VISIBLE, 220, 80, 500, 20, hwnd, (HMENU) ID_ENTER_TIP_2, instance, 0);
	hEnter = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 220, 35, 300, 22, hwnd, (HMENU) ID_ENTER_TEXT, instance, 0);

	/* ������� */
	CreateWindowEx(0, TEXT("STATIC"), TEXT("�ƶ����裺"), WS_CHILD | WS_VISIBLE, 220, 125, 100, 20, hwnd, (HMENU) ID_STEP_LABEL, instance, 0);
	CreateWindowEx(0, TEXT("STATIC"), TEXT("�ո���ƶ����裬U=���ϣ�R=���ң�D=���£�L=����"), WS_CHILD | WS_VISIBLE, 220, 172, 500, 20, hwnd, (HMENU) ID_STEP_TIP, instance, 0);
	hStep = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_READONLY, 220, 142, 300, 22, hwnd, (HMENU) ID_STEP_TEXT, instance, 0);

	/* ��ť */
	hCalc = CreateWindowEx(0, TEXT("BUTTON"), TEXT("���㲽��"), WS_CHILD | WS_VISIBLE, rect.right - 130, 30, 100, 30, hwnd, (HMENU) ID_CALC, instance, 0);
	hPlay = CreateWindowEx(0, TEXT("BUTTON"), TEXT("��̬��ʾ"), WS_CHILD | WS_VISIBLE, rect.right - 130, 65, 100, 30, hwnd, (HMENU) ID_PLAY, instance, 0);

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

	/* ������ʾ */
	EnableWindow(hPlay, FALSE);
}

/*
	$ input					���봦��
*/
static void input() {
	size_t	 length;
	char	*buffer;

	/* ��ȡ���� */
	if(length = GetWindowTextLength(hEnter)) {
		if((buffer = (char *) calloc(length + 1, sizeof(char))) == NULL) {
			exit(0);
		}

		GetWindowText(hEnter, buffer, length + 1);
	} else {
		buffer = NULL;
	}

	/* ���ô����� */
	actCalc(buffer, length);

	if(buffer) {
		free(buffer);
	}
}

/*
	$ message				��Ϣѭ��

	@ HWND hwnd
	@ UINT msg
	@ WPARAM wparam
	@ LPARAM lparam

	# LRESULT
*/
static LRESULT CALLBACK message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch(msg) {
		/* ���ƴ��� */
		case WM_CREATE:		draw(hwnd);			return 0;
		/* ���ٴ��� */
		case WM_DESTROY:	PostQuitMessage(0);	return 0;

		/* ����ַ� */
		case WM_COMMAND:
			switch(LOWORD(wparam)) {
				case ID_CALC: input();		break;
				case ID_PLAY: actPlay();	break;
			}
			break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}


/*
	$ gui					��������
*/
void gui() {
	MSG						msg;
	RECT					rect;
	WNDCLASS				wndclass;
	INITCOMMONCONTROLSEX	iccx;

	iccx.dwSize	= sizeof iccx;
	iccx.dwICC	= ICC_STANDARD_CLASSES;

	/* ��ʼ�� ComCtl32.dll */
	InitCommonControlsEx(&iccx);

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
		return;
	}

	/* ���������� */
	if((hWin = CreateWindowEx (
			/* ex style */	0,
			/* classname */	CLASSNAME,
			/* title */		TITLE,
			/* style */		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			/* left */		(rect.right - WIDTH) / 2,
			/* top */		(rect.bottom - HEIGHT) / 2,
			/* width */		WIDTH,
			/* height */	HEIGHT,
			/* parent */	NULL,
			/* menu */		NULL,
			/* instance */	instance,
			/* lpparam */	0
		)) == NULL) {
			return;
	}

	ShowWindow(hWin, SW_SHOW);
	UpdateWindow(hWin);

	while(GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*
	$ show					��ʾ����

	@ int select			ѡ����ı�
	@ unsigned char *number
*/
void show(int select, unsigned char *number) {
	char	data[16];
	size_t	i, j;

	/* �ո�λ�� */
	size_t position = number[0] - 1;

	/* ѡ���ı� */
	if(select >= 0) {
		Edit_SetSel(hStep, select, select + 1);
	}

	/* ������� */
	for(i = j = 0; i < 9; i++) {
		/* �жϿո� */
		if(position == i) {
			continue;
		}

		sprintf(data, "%ld", number[j + 1]);
		SetWindowTextA(hDigit[j], data);

		/* �ƶ���ť */
		SetWindowPos(hDigit[j], HWND_TOP, 15 + 60 * (i % 3), 15 + 60 * (i / 3), 60, 60, SWP_NOZORDER);

		j++;
	}
}

/*
	$ enable				��ť����

	@ int calc
	@ int play
*/
void enable(int calc, int play) {
	EnableWindow(hEnter, (BOOL) calc);

	EnableWindow(hCalc, (BOOL) calc);
	EnableWindow(hPlay, (BOOL) play);
}

/*
	$ step					�������

	@ char *buffer
*/
void step(char *buffer) {
	SetWindowTextA(hStep, buffer);
}

/*
	$ focus					Ϊ�����ı������ý���

	@ int set
*/
void focus(int set) {
	if(set) {
		Edit_SetSel(hStep, 0, -1);
		SendMessage(hStep, WM_SETFOCUS, (WPARAM) 0, (LPARAM) 0);
	} else {
		SendMessage(hStep, WM_KILLFOCUS, (WPARAM) 0, (LPARAM) 0);
	}
}
