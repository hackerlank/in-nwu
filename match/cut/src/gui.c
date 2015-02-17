/*
	$ gui.c, 0.2.1221
*/

#include <Windows.h>
#include <Commctrl.h>
#include <windowsx.h>
#include "base.h"


/* �������� */
#define HELP	"���� 2012 ��������ѧ��ʮ��������Ļ������ڳ�����ƴ���������ڶ���Ĳ�����Ʒ��\n\n"	\
				"Ҫʹ���������������׼���ļ� file.1���ļ���Ӧ��������ɸ���Ҫ���д������������ÿ������һ�С�Ȼ�����������Ͻǵġ��������ݡ���ť��ѡ������ļ���\n\n"	\
				"�������ݺ��뵥���������Ͻǵġ���ʼ���㡱��ť�����򼴶Ե��������������������㣬������ǵĳ˻���\n\n"	\
				"�ڼ�����ɺ󣬵��������������ť���Խ���������ĳ˻����浽 file.2 �ļ��С�����˻���λ�������ڳ������½ǵ��ı����н������á����������Ч������������ 0������ᱣ�������ļ�������"


/* ������Ϣ */
#define TITLE			""
#define CLASSNAME		"cut_window_class"
#define WIDTH			800
#define HEIGHT			350

/* �ؼ� menuid */
#define ID_LABEL_PRE	1000
#define ID_LABEL_POST	1001
#define ID_OPEN			2000
#define ID_SAVE			2001
#define ID_CALC			2002
#define ID_HELP			2003
#define ID_TEXT			3000
#define ID_SSE			4000
#define ID_LIST			5000


/* �ؼ���� */
static HWND hwndOpen, hwndSave, hwndCalc, hwndHelp;
static HWND hwndText, hwndSSE, hwndList;



/* ���ڽ������ */
static void draw(HWND hwnd) {
	size_t	i;
	size_t	ids[] = {ID_LABEL_PRE, ID_LABEL_POST, ID_OPEN, ID_SAVE, ID_CALC, ID_HELP, ID_TEXT, ID_SSE, ID_LIST};

	/* �б��������Ϳ�� */
	char	*title[] = {"����", "���ʽ", "�˻�", "�˻�λ��", "ʹ���ڴ�", "�����ʱ", "ת����ʱ", "�ܺ�ʱ"};
	size_t	 width[] = {85, 120, 145, 70, 95, 75, 75, 75};

	RECT	rect;
	HDC		hdc;
	HFONT	hfont;

	LVCOLUMN column;
	ZeroMemory(&column, sizeof column);

	/* ��ȡ�ɻ����� */
	GetClientRect(hwnd, &rect);

	/* ��ʾ�ı� */
	CreateWindowEx(0, TEXT("STATIC"), TEXT("����"),		WS_CHILD | WS_VISIBLE,  15, rect.bottom - 30, 30, 25, hwnd, (HMENU) ID_LABEL_PRE,	instance, 0);
	CreateWindowEx(0, TEXT("STATIC"), TEXT("λ����"),		WS_CHILD | WS_VISIBLE, 100, rect.bottom - 30, 50, 25, hwnd, (HMENU) ID_LABEL_POST,	instance, 0);
	/* ���ư�ť */
	hwndOpen	= CreateWindowEx(0, TEXT("BUTTON"), TEXT("��������"),	 WS_CHILD | WS_VISIBLE, 10,					10,					90, 28, hwnd, (HMENU) ID_OPEN, instance, 0);
	hwndSave	= CreateWindowEx(0, TEXT("BUTTON"), TEXT("������"),	 WS_CHILD | WS_VISIBLE, 105,				10,					90, 28, hwnd, (HMENU) ID_SAVE, instance, 0);
	hwndCalc	= CreateWindowEx(0, TEXT("BUTTON"), TEXT("��ʼ����"),	 WS_CHILD | WS_VISIBLE, rect.right - 100,	10,					90, 28, hwnd, (HMENU) ID_CALC, instance, 0);
	hwndHelp	= CreateWindowEx(0, TEXT("BUTTON"), TEXT("ʹ�ð���"),	 WS_CHILD | WS_VISIBLE, rect.right - 100,	rect.bottom - 38,	90, 28, hwnd, (HMENU) ID_HELP, instance, 0);
	/* ��������� */
	hwndText	= CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("100"), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 45, rect.bottom - 33, 50, 22, hwnd, (HMENU) ID_TEXT, instance, 0);
	/* ���Ƹ�ѡ�� */
	hwndSSE		= CreateWindowEx(0, TEXT("BUTTON"), TEXT("ʹ�� SSE2 ָ��Լ�������Ż�"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 210, rect.bottom - 35, 190, 25, hwnd, (HMENU) ID_SSE, instance, 0);
	/* ���ƽ���б� */
	hwndList	= CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL, WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER, 10, 45, rect.right - 20, rect.bottom - 90, hwnd, (HMENU) ID_LIST, instance, 0);

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

	/* �б�����ʽ */
	column.fmt	= LVCFMT_LEFT;
	column.mask	= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	/* Ϊ����б������ */
	for(i = 0; i < (sizeof title / sizeof(char *)); i++) {
		column.pszText	= title[i];
		column.cx		= width[i];

		ListView_InsertColumn(hwndList, i, &column);
	}

	ListView_SetExtendedListViewStyle(hwndList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	/* ���ð�ť */
	saveEnable(0);
	calcEnable(0);

	/* SSE ѡ�� */
	if(cpuSSE()) {
		SendMessage(hwndSSE, BM_SETCHECK, 1, 0);
	} else {
		sseEnable(0);
	}
}

/* ��ť�¼� */
static void command(WPARAM wparam, LPARAM lparam) {
	/* �ؼ� ID */
	WORD id = LOWORD(wparam);

	switch(id) {
		case ID_OPEN:	input();		break;
		case ID_SAVE:	output();		break;
		case ID_CALC:	dispatch();		break;
		case ID_HELP:	NOTICE(HELP);	break;
	}
}

/* ������Ϣѭ�� */
static LRESULT CALLBACK message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch(msg) {
		/* ���ƴ��� */
		case WM_CREATE:
			draw(hwnd); return 0;

		/* �رմ��� */
		case WM_CLOSE:
			if(close() == 0) {
				return 0;
			}
			break;

		/* ���ٴ��� */
		case WM_DESTROY:
			PostQuitMessage(0); return 0;

		/* ����ַ� */
		case WM_COMMAND:
			command(wparam, lparam); break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}



void gui() {
	MSG						msg;
	RECT					rect;
	WNDCLASS				wndclass;
	INITCOMMONCONTROLSEX	iccx;

	/* ��ȡ�����С */
	GetWindowRect(GetDesktopWindow(), &rect);

	ZeroMemory(&iccx, sizeof iccx);
	ZeroMemory(&wndclass, sizeof wndclass);

	iccx.dwSize	= sizeof iccx;
	iccx.dwICC	= ICC_STANDARD_CLASSES | ICC_LISTVIEW_CLASSES;

	/* ��ʼ�� ListView */
	InitCommonControlsEx(&iccx);

	wndclass.hInstance		= instance;
	wndclass.lpszClassName	= TEXT(CLASSNAME);
	wndclass.lpfnWndProc	= &message;
	wndclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH) COLOR_BTNSHADOW;

	/* ע�ᴰ���� */
	if(RegisterClass(&wndclass) == 0) {
		DIE("ע�ᴰ����ʧ��");
	}

	/* ���������� */
	if((win = CreateWindowEx (
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
			DIE("����ͼ�δ���ʧ��");
	}

	ShowWindow(win, SW_SHOW);
	UpdateWindow(win);

	while(GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


/* ���û���� �������� ��ť */
void openEnable(int enable) {
	EnableWindow(hwndOpen, (BOOL) enable);
}

/* ���û���� ������ ��ť */
void saveEnable(int enable) {
	EnableWindow(hwndSave, (BOOL) enable);
}

/* ���û���� ��ʼ����/��ֹ���� ��ť */
void calcEnable(int enable) {
	EnableWindow(hwndCalc, (BOOL) enable);
}

/* ���� ��ʼ����/��ֹ���� ��ť������ */
void calcText(char *text) {
	SetWindowText(hwndCalc, TEXT(text));
}

/* ��ȡ ���λ�� */
size_t textRead() {
	int		input;
	char	buffer[16];

	ZeroMemory(buffer, sizeof buffer);
	GetWindowText(hwndText, buffer, sizeof buffer);

	if(sscanf(buffer, "%d", &input) < 1) {
		return 0;
	}
	if(input < 0) {
		return 0;
	}

	return (size_t) input;
}

/* ���û���� ʹ��SSE ѡ�� */
void sseEnable(int enable) {
	if(cpuSSE() == 0) {
		enable = 0;
	}

	EnableWindow(hwndSSE, (BOOL) enable);
}

/* ��ȡ�Ƿ�ѡ���� ʹ��SSE */
int sseChecked() {
	if(cpuSSE()) {
		if(Button_GetCheck(hwndSSE) == BST_CHECKED) {
			return 1;
		}
	}

	return 0;
}

/* ���б�������� */
void listAppend(number_s *number) {
	int		index;
	LVITEM	item;

	ZeroMemory(&item, sizeof item);

	item.mask	= LVIF_TEXT;
	item.iItem	= ListView_GetItemCount(hwndList);

	if((index = ListView_InsertItem(hwndList, &item)) == -1) {
		DIE("���������ʧ��");
	}

	/* ���� */
	listSet(index, 0, "%lu", number->number);

	/* ���ʽ */
	if(number->remainder == 0) {
		listSet(index, 1, "3 * %lu", number->exponent);
	} else {
		listSet(index, 1, "%lu + 3 * %lu", number->remainder, number->exponent);
	}

	/* �˻� */
	listSet(index, 2, "?");
	/* ��λ�� */
	listSet(index, 3, "?");
	/* �ڴ�ռ�� */
	listSet(index, 4, "?");
	/* ʱ�� */
	listSet(index, 5, "-");
	listSet(index, 6, "-");
	listSet(index, 7, "-");
}

/* ����ĳ��ĳ�е����� */
void listSet(int_t index, int_t column, char *format, ...) {
	char *text;

	va_list arg;
	va_start(arg, format);

	/* ���ֻ����� */
	NEW(text, char, _vscprintf(format, arg) + 1);
	/* �������� */
	vsprintf(text, format, arg);

	ListView_SetItemText(hwndList, index, column, text);

	va_end(arg);
	DEL(text);
}

/* ����б� */
void listClear() {
	ListView_DeleteAllItems(hwndList);
}
