/*
	$ MF Client   (C) 2005-2013 MF
	$ icon.c
*/

#include "base.h"
#include "../resource.h"


/* icon object */
static NOTIFYICONDATA icon;


/*
	iconCreate										����������ͼ��
*/
void iconCreate() {
	HINSTANCE instance;

	instance = (HINSTANCE) GetModuleHandle(NULL);

	ZeroMemory(&icon, sizeof icon);

	icon.cbSize				= sizeof icon;
	icon.hWnd				= handle_window;
	icon.uID				= 0;
	icon.uFlags				= NIF_MESSAGE | NIF_ICON | NIF_TIP;
	icon.uCallbackMessage	= WM_CLIENT;
	icon.hIcon				= LoadIcon(instance, MAKEINTRESOURCE(IDI_ICON2));

	_tcscpy_s(icon.szTip, _countof(icon.szTip), TEXT("��������"));

	Shell_NotifyIcon(NIM_ADD, &icon);
}

/*
	iconDestroy										����������ͼ��
*/
void iconDestroy() {
	Shell_NotifyIcon(NIM_DELETE, &icon);
}

/*
	iconTip											��ʾ������ʾ

	@ PTCHAR	title
	@ PTCHAR	message
	@ int		type								ͼ����ʽ
*/
void iconTip(PTCHAR title, PTCHAR message, int type) {
	icon.uFlags			= NIF_MESSAGE | NIF_ICON | NIF_INFO;
	icon.uTimeout		= 5000;

	switch(type) {
		case 1:		icon.dwInfoFlags = NIIF_WARNING;	break;
		case 2:		icon.dwInfoFlags = NIIF_ERROR;		break;
		default:	icon.dwInfoFlags = NIIF_INFO;		break;
	}

	_tcscpy_s(icon.szInfoTitle,	_countof(icon.szInfoTitle),	title);
	_tcscpy_s(icon.szInfo,		_countof(icon.szInfo),		message);

	Shell_NotifyIcon(NIM_MODIFY, &icon);
}

/*
	iconDraw										���Ʋ˵�
*/
void iconDraw() {
	POINT	point;
	HMENU	menu;

	if((menu = CreatePopupMenu()) == NULL) {
		return;
	}

	AppendMenu(menu, MF_STRING,			0x2000,		TEXT("�������� (&C)"));
	AppendMenu(menu, MF_STRING,			0x3000,		TEXT("�Ͽ����� (&D)"));
	AppendMenu(menu, MF_SEPARATOR,		0,			NULL);
	AppendMenu(menu, MF_STRING,			0x4000,		TEXT("�����Ӵ��� (&W)"));
	AppendMenu(menu, MF_SEPARATOR,		0,			NULL);
	AppendMenu(menu, MF_STRING,			0x1000,		TEXT("�ر� (&X)"));

	/* ���ò˵� */
	switch(callStatus()) {
		/* δ������ */
		case 0:
			EnableMenuItem(menu, 0x3000, MF_DISABLED);
			break;

		/* �������� */
		case 1:
			EnableMenuItem(menu, 0x2000, MF_DISABLED);
			EnableMenuItem(menu, 0x4000, MF_DISABLED);
			break;

		/* �������� */
		case 2:
			EnableMenuItem(menu, 0x2000, MF_DISABLED);
			EnableMenuItem(menu, 0x3000, MF_DISABLED);
			EnableMenuItem(menu, 0x4000, MF_DISABLED);
			break;
	}

	GetCursorPos		(&point);

	SetForegroundWindow	(handle_window);
	TrackPopupMenu		(menu, TPM_LEFTALIGN, point.x, point.y, 0, handle_window, NULL);
	DestroyMenu			(menu);

	PostMessage			(handle_window, WM_NULL, 0, 0);
}

/*
	iconInvoke										�˵����ܻص�
*/
void iconInvoke(WPARAM id) {
	switch(id) {
		/* �ر� */
		case 1:
			callClose();
			break;

		/* �������� */
		case 2:
			callStart();
			break;

		/* �Ͽ����� */
		case 3:
			callDisconnect();
			break;

		/* ��ʾ���� */
		case 4:
			iconDestroy();
			guiShow(NULL);
			break;
	}
}
