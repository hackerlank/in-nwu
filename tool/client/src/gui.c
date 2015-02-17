/*
	$ MF Client   (C) 2005-2013 MF
	$ gui.c
*/

#define _CLIENT_INCLUDE_
#define _CLIENT_GUI_

#define  UNICODE
#define _UNICODE


#include <Windows.h>
#include <Commctrl.h>

#include "base.h"
#include "../resource.h"



/* ���� */
#define CLASSNAME			TEXT ("MF_CLIENT_CLASS")
#define TITLE				TEXT ("������ѧ 802.1x ����ͻ���")
#define WIDTH				500
#define HEIGHT				320



/* �������ؽ���Ϣ */
static UINT WM_TASKBARCREATED;



/*
	draw
*/
static void draw(HWND hwnd) {
	int			i;
	DWORD		width;

	HINSTANCE	instance;
	RECT		rect;
	HFONT		font_normal;
	HFONT		font_large;

	instance	= (HINSTANCE)	GetModuleHandle(NULL);

	font_normal	= (HFONT)		GetStockObject(DEFAULT_GUI_FONT);
	font_large	= CreateFont(14, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, NULL);

	GetClientRect(hwnd, &rect);

	/* ������� */
	width		= (rect.right - 54) / 2;

	/* ���� */
	CreateWindowEx(0, TEXT("STATIC"), NULL, WS_CHILD | WS_VISIBLE, 0, 0,				WIDTH, 60,					hwnd, (HMENU) 1000, instance, 0);
	CreateWindowEx(0, TEXT("STATIC"), NULL, WS_CHILD | WS_VISIBLE, 0, 60,				WIDTH, rect.bottom - 120,	hwnd, (HMENU) 1001, instance, 0);
	CreateWindowEx(0, TEXT("STATIC"), NULL, WS_CHILD | WS_VISIBLE, 0, rect.bottom - 60,	WIDTH, 1,					hwnd, (HMENU) 1002, instance, 0);

	/* ��ʾ���� */
	handle_object_tip		= CreateWindowEx(0, TEXT("STATIC"), TEXT(""), WS_CHILD | WS_VISIBLE, 15, 22, WIDTH, 30, hwnd, (HMENU) 0, instance, 0);
	
	/* ��ǩ */
	CreateWindowEx(0, TEXT("STATIC"), TEXT("�û�����"),	WS_CHILD | WS_VISIBLE, 20,						 80, 100, 20, hwnd, (HMENU) 1, instance, 0);
	CreateWindowEx(0, TEXT("STATIC"), TEXT("���룺"),		WS_CHILD | WS_VISIBLE, rect.right - width - 16,	 80, 100, 20, hwnd, (HMENU) 2, instance, 0);
	CreateWindowEx(0, TEXT("STATIC"), TEXT("�����豸��"),	WS_CHILD | WS_VISIBLE, 20,						140, 100, 20, hwnd, (HMENU) 3, instance, 0);
	
	/* ����� */
	handle_object_username	= CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"),		NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,					18,							98,		width,				20, hwnd, (HMENU) 4, instance, 0);
	handle_object_password	= CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"),		NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | ES_PASSWORD,	rect.right - width - 18,	98,		width,				20, hwnd, (HMENU) 5, instance, 0);
	/* ѡ��� */
	handle_object_adapter	= CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("COMBOBOX"),	NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,				18,							158,	rect.right - 36,	25, hwnd, (HMENU) 6, instance, 0);

	/* ѡ�� */
	handle_object_hidden	= CreateWindowEx(0, TEXT("BUTTON"), TEXT("�������ߺ���������"),	WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, rect.right - 150,	185,				135, 18, hwnd, (HMENU) 7, instance, 0);
	handle_object_login		= CreateWindowEx(0, TEXT("BUTTON"), TEXT("�Զ���������"),		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 15,				rect.bottom - 38,	100, 18, hwnd, (HMENU) 8, instance, 0);
	handle_object_remember	= CreateWindowEx(0, TEXT("BUTTON"), TEXT("��ס��¼��Ϣ"),		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 130,				rect.bottom - 38,	100, 18, hwnd, (HMENU) 9, instance, 0);

	/* ���Ӱ�ť */
	handle_object_button	= CreateWindowEx(0, TEXT("BUTTON"), TEXT("��������"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, rect.right - 130, rect.bottom - 45, 115, 30, hwnd, (HMENU) 10, instance, 0);

	/* ������ʾ�������� */
	SendDlgItemMessage(hwnd, 0, WM_SETFONT, (WPARAM) font_large, 0);

	/* ������������ */
	for(i = 1; i <= 10; i++) {
		SendDlgItemMessage(hwnd, i, WM_SETFONT, (WPARAM) font_normal, 0);
	}
}

/*
	list
*/
static void list(int load) {
	char	**list;

	int		basic;
	size_t	count;

	if(CHECK_GET(hidden)) {
		basic = 1;
	} else {
		basic = 0;
	}

	/* ����б� */
	SendMessage(handle_object_adapter, CB_RESETCONTENT, 0, 0);
	SendMessage(handle_object_adapter, CB_SETCURSEL,	0, 0);

	/* ��ȡ�豸 */
	if(count = deviceList(basic, &list)) {
		int		i		= 0;
		int		index	= 0;
		BYTE	*mac;

		if(load) {
			if(mac = configGetAdapter()) {
				index = deviceGet(basic, mac);
			}
		}

		while(count--) {
			SendMessageA(handle_object_adapter, CB_ADDSTRING, 0, (LPARAM) list[i++]);
		}

		SendMessage(handle_object_adapter, CB_SETCURSEL, index, 0);
	}
}

/*
	fill
*/
static void fill() {
	if(configRead()) {
		char *str;

		CHECK_T(remember);

		if(str = configGetUsername()) {
			SetWindowTextA(handle_object_username, str);
		}
		if(str = configGetPassword()) {
			SetWindowTextA(handle_object_password, str);
		}

		if(configGetHidden()) {
			CHECK_T(hidden);
		}

		if(configGetLogin()) {
			CHECK_T(login);

			EnableWindow(handle_object_remember, FALSE);
		}
	} else {
		CHECK_T(hidden);
	}

	list(1);

	/* �Զ���¼ */
	if(CHECK_GET(login)) {
		callStart();
	}
}

/*
	filter
*/
static LRESULT CALLBACK filter(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch(msg) {
		/* ���ƴ��� */
		case WM_CREATE:
			draw(hwnd);
			fill();

			return 0;

		/* ���ٴ��� */
		case WM_DESTROY:
			PostQuitMessage(0);

			return 0;

		/* �������� */
		case WM_KEYDOWN:
			{
				HWND object	= GetFocus();

				switch(wparam) {
					case VK_TAB:
						object = GetNextDlgTabItem(GetParent(object), object, FALSE);

						SetFocus(object);

						break;

					case VK_RETURN:
						callStart();

						break;
				}
			}
			break;

		/* ������� */
		case WM_CTLCOLORSTATIC:
			{
				HBRUSH		brush;

				HDC			dc		= (HDC) wparam;
				COLORREF	color	= 0;
				int			done	= 1;

				switch(GetDlgCtrlID((HWND) lparam)) {
					/* ��ǩ����͸�� */
					case 0:
						SetTextColor(dc, RGB(255, 255, 255));
					case 1:
					case 2:
						SetBkMode(dc, TRANSPARENT);
						done = 0;
						
						return (LRESULT) GetStockObject(NULL_BRUSH);

					/* ����͸�� */
					case 1000: color = RGB( 19, 119, 152);	break;
					case 1001: color = RGB(255, 255, 255);	break;
					case 1002: color = RGB(200, 200, 200);	break;

					default:
						done = 0;
				}

				if(done) {
					brush = CreateSolidBrush(color);

					SetBkColor(dc, color);

					return (LRESULT) brush;
				}
			}
			break;

		/* ��������ͼ������� */
		case WM_CLIENT:
			if((wparam == 0) && ((lparam == WM_LBUTTONUP) || (lparam == WM_RBUTTONUP))) {
				iconDraw();
			}
			break;

		/* ����ַ� */
		case WM_COMMAND:
			switch(LOWORD(wparam)) {
				/* hidden */
				case 7:
					list(0);

					break;

				/* login */
				case 8:
					if(CHECK_GET(login)) {
						CHECK_T			(remember);

						EnableWindow	(handle_object_remember, FALSE);
					} else {
						EnableWindow	(handle_object_remember, TRUE);
					}

					break;

				/* button */
				case 10:
					callStart();

					break;
			}

			if(wparam > 0x0FFF) {
				iconInvoke((((unsigned int) wparam) & 0xF000) >> 12);
			}

			break;

		/* ��Դ���� */
		case WM_POWERBROADCAST:
			callPower(wparam);

			break;

		default:
			/* �������ػ� */
			if(msg == WM_TASKBARCREATED) {
				iconCreate();
			}
			break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}


/*
	guiInit											��ʼ�� GUI

	@ HINSTANCE		instance
*/
void guiInit(HINSTANCE instance) {
	MSG						msg;
	RECT					rect;
	WNDCLASS				wndclass;
	INITCOMMONCONTROLSEX	iccx;

	/* ��ȡ�������ػ���Ϣ */
	WM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));

	iccx.dwSize	= sizeof iccx;
	iccx.dwICC	= ICC_STANDARD_CLASSES;

	/* ��ʼ�� comctl32 */
	InitCommonControlsEx(&iccx);

	/* ��ȡ�����С */
	GetWindowRect(GetDesktopWindow(), &rect);

	ZeroMemory(&wndclass, sizeof wndclass);

	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.hInstance		= instance;
	wndclass.lpszClassName	= CLASSNAME;
	wndclass.lpfnWndProc	= &filter;
	wndclass.hIcon			= LoadIcon(instance, MAKEINTRESOURCE(IDI_ICON1));
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH) (COLOR_BTNFACE + 1);

	if(RegisterClass(&wndclass) == 0) {
		return;
	}

	if((handle_window = CreateWindowEx (
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

	ShowWindow		(handle_window, SW_SHOW);
	UpdateWindow	(handle_window);

	while(GetMessage(&msg, 0, 0, 0) > 0) {
		if(msg.message == WM_KEYDOWN) {
			switch(msg.wParam) {
				case VK_TAB:
				case VK_RETURN:
					SendMessage(handle_window, WM_KEYDOWN, msg.wParam, msg.lParam);
					continue;
			}
		}

		TranslateMessage	(&msg);
		DispatchMessage		(&msg);
	}
}


/*
	guiHide											����������
*/
void guiHide() {
	ShowWindow(handle_window, SW_HIDE);
}

/*
	guiShow											��ʾ����

	@ char		*message							��ʾ��Ϣ
*/
void guiShow(char *message) {
	if(message) {
		SetWindowTextA(handle_object_tip, message);
	}

	ShowWindow(handle_window, SW_SHOW);
}
