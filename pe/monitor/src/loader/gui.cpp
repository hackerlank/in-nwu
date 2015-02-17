/*
	$ Monitor Loader   (C) 2005-2014 MF
	$ gui.cpp
*/


#pragma comment (linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")


#include "base.h"



/* ������Ϣ */
#define CLASS			TEXT("monitor_loader_class")
#define TITLE			TEXT("Monitor ������")
#define WIDTH			420
#define HEIGHT			230


/* ���ڿؼ� */
#define ID_LABEL_RUN			0x1000
#define ID_LABEL_HANDLER		0x1001
#define ID_TEXT_RUN				0x2000
#define ID_TEXT_HANDLER			0x2001
#define ID_CHECK_CONSOLE		0x3000
#define ID_BUTTON_OK			0x4000
#define ID_BUTTON_BROWSE		0x4001




/* ������Ϣ */
static	LPCSTR		start_run;
/* ������·�� */
static	LPCSTR		start_handler;

/* ��ʾ����̨���� */
static	bool		start_console;


/* ���������ź� */
static	HANDLE		status;


/* ����ʵ�� */
static	HINSTANCE	application;


/* ���ھ�� */
static	HWND		window;
/* �ؼ���� */
static	HWND		handle_label_run, handle_label_handler;
static	HWND		handle_text_run, handle_text_handler;
static	HWND		handle_check_console;
static	HWND		handle_button_ok, handle_button_browse;




/* �������� */
static void disable (bool set) {
	if(set == true) {
		EnableWindow(handle_text_run,		FALSE);
		EnableWindow(handle_text_handler,	FALSE);
		EnableWindow(handle_check_console,	FALSE);
		EnableWindow(handle_button_ok,		FALSE);
		EnableWindow(handle_button_browse,	FALSE);
	} else {
		EnableWindow(handle_text_run,		TRUE);
		EnableWindow(handle_text_handler,	TRUE);
		EnableWindow(handle_check_console,	TRUE);
		EnableWindow(handle_button_ok,		TRUE);
		EnableWindow(handle_button_browse,	TRUE);
	}
}


/* ѡ���ļ� */
static void browse (void) {
	// ���뻺����
	char	buffer	[MAX_PATH + 1];

	// �ļ���Ϣ
	OPENFILENAME	file;

	ZeroMemory(& buffer,	sizeof buffer);
	ZeroMemory(& file,		sizeof file);

	// �����ļ���Ϣ
	file.lStructSize	= sizeof file;
	file.hwndOwner		= window;
	file.lpstrTitle		= "ѡ��Ӧ�ó���";
	file.lpstrFilter	= "��ִ���ļ�\0*.exe\0";
	file.nFilterIndex	= 1;
	file.lpstrFile		= buffer;
	file.nMaxFile		= MAX_PATH;
	file.Flags			= OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

	// ���ļ�ѡ��Ի���
	if(GetOpenFileNameA(& file) == FALSE) {
		return;
	}

	// ����ѡ�ļ�·�����õ��ı�����
	SetWindowTextA(handle_text_run, buffer);
}




/* ���ڻ��� */
static int draw (HWND hwnd) {
	// �ɻ�������
	RECT	rect;

	// ��ȡ�ɻ��������С
	GetClientRect(hwnd, & rect);

	// �ı�
	handle_label_run		= CreateWindowExA (0, "STATIC", "Ӧ�ó���·��������������",		WS_CHILD | WS_VISIBLE, 15, 15, rect.right - 30, 30, hwnd, (HMENU) ID_LABEL_RUN,		application, 0);
	handle_label_handler	= CreateWindowExA (0, "STATIC", "Ӧ�ó���ӿڵ��ÿ�����·����",	WS_CHILD | WS_VISIBLE, 15, 75, rect.right - 30, 30, hwnd, (HMENU) ID_LABEL_HANDLER,	application, 0);

	// �����ı���
	handle_text_run		= CreateWindowExA (WS_EX_CLIENTEDGE, "EDIT", start_run,		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,	15, 37, rect.right - 30, 22, hwnd, (HMENU) ID_TEXT_RUN,		application, 0);
	handle_text_handler	= CreateWindowExA (WS_EX_CLIENTEDGE, "EDIT", start_handler,	WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,	15, 97, rect.right - 30, 22, hwnd, (HMENU) ID_TEXT_HANDLER,	application, 0);

	// ��ѡ��
	handle_check_console	= CreateWindowExA (0, "BUTTON", "��ʾ����̨����",		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 15, rect.bottom - 43, 150, 25, hwnd, (HMENU) ID_CHECK_CONSOLE,	application, 0);

	// ��ť
	handle_button_ok		= CreateWindowExA (0, "BUTTON", "ȷ��",				WS_CHILD | WS_VISIBLE, rect.right - 230, rect.bottom - 43,  90, 28, hwnd, (HMENU) ID_BUTTON_OK,		application, 0);
	handle_button_browse	= CreateWindowExA (0, "BUTTON", "ѡ��Ӧ�ó��� ...",	WS_CHILD | WS_VISIBLE, rect.right - 135, rect.bottom - 43, 120, 28, hwnd, (HMENU) ID_BUTTON_BROWSE,	application, 0);

	// ��ȡ��ͼ�豸
	HDC		hdc		= GetDC(hwnd);
	// ��ȡϵͳĬ������
	HFONT	hfont	= (HFONT) GetStockObject(DEFAULT_GUI_FONT);

	// ѡ������
	SelectObject	(hdc, hfont);
	// �ͷ��豸
	ReleaseDC		(hwnd, hdc);

	// ��������
	SendDlgItemMessage(hwnd, ID_LABEL_RUN,		WM_SETFONT, (WPARAM) hfont, 0);
	SendDlgItemMessage(hwnd, ID_LABEL_HANDLER,	WM_SETFONT, (WPARAM) hfont, 0);
	SendDlgItemMessage(hwnd, ID_TEXT_RUN,		WM_SETFONT, (WPARAM) hfont, 0);
	SendDlgItemMessage(hwnd, ID_TEXT_HANDLER,	WM_SETFONT, (WPARAM) hfont, 0);
	SendDlgItemMessage(hwnd, ID_CHECK_CONSOLE,	WM_SETFONT, (WPARAM) hfont, 0);
	SendDlgItemMessage(hwnd, ID_BUTTON_OK,		WM_SETFONT, (WPARAM) hfont, 0);
	SendDlgItemMessage(hwnd, ID_BUTTON_BROWSE,	WM_SETFONT, (WPARAM) hfont, 0);

	// �ж��Ƿ���ʾ����̨����
	if(start_console == true) {
		SendMessage(handle_check_console, BM_SETCHECK, BST_CHECKED, 0);
	}

	// �ж��Ƿ��Զ���ʼ
	if(start_run != NULL) {
		// ���Ͱ�ť�����¼�
		SendMessage(handle_button_ok, BM_CLICK, 0, 0);
	}

	return 0;
}



/* ��Ϣ���� */
static void command (WPARAM wparam, LPARAM lparam) {
	// �ж��Ƿ�Ϊ����״̬
	if(WaitForSingleObject(status, 0) == WAIT_OBJECT_0) {
		return;
	}

	// �ؼ� ID
	WORD id = LOWORD(wparam);

	switch(id) {
		case ID_BUTTON_OK:
			{
				// ���������
				char *	input_run		= NULL;
				char *	input_handler	= NULL;

				// ��ʾ����̨����
				bool	input_console	= false;

				// ��������ֳ���
				int		length_run		= GetWindowTextLengthA(handle_text_run);
				int		length_handler	= GetWindowTextLengthA(handle_text_handler);

				// ��ȡ������Ϣ
				if(length_run > 0) {
					input_run		= new char [length_run + 1];

					GetWindowTextA(handle_text_run, (LPSTR) input_run, length_run + 1);
				}

				// ��ȡ������·��
				if(length_handler > 0) {
					input_handler	= new char [length_handler + 1];

					GetWindowTextA(handle_text_handler, (LPSTR) input_handler, length_handler + 1);
				}

				// ��ȡ��ʾ����̨���ڸ�ѡ��
				if(SendMessage(handle_check_console, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					input_console	= true;
				}

				// ��������
				disable(true);

				// ��������״̬
				SetEvent(status);

				// ���ô���
				call(input_run, input_handler, input_console);
			}

			break;

		case ID_BUTTON_BROWSE:
			// ѡ���ļ�
			browse();

			break;
	}
}


/* ��Ϣѭ�� */
static LRESULT CALLBACK message (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch(msg) {
		// ���ƴ���
		case WM_CREATE:
			// ִ�л���
			return (LRESULT) draw(hwnd);

		// �رմ���
		case WM_CLOSE:
			// �ж��Ƿ�Ϊ����״̬
			if(WaitForSingleObject(status, 0) == WAIT_OBJECT_0) {
				MessageBox(window, TEXT("��������������ָ����Ӧ�ó������Եȡ�"), TEXT(""), MB_OK | MB_ICONWARNING);

				return 0;
			}

			break;

		// ���ٴ���
		case WM_DESTROY:
			// ��ֹ��Ϣѭ��
			PostQuitMessage(0);

			return 0;

		// ����ַ�
		case WM_COMMAND:
			// ������Ϣ����
			command(wparam, lparam);
			
			break;
	}

	// ʹ��Ĭ�Ϸ���
	return DefWindowProc (hwnd, msg, wparam, lparam);
}




/* GUI ��� */
int gui_start (const char *run, const char *handler, bool console, void *instance, int show) {
	// ��������
	start_run		= (LPCSTR)	run;
	start_handler	= (LPCSTR)	handler;
	start_console	=			console;

	// Ӧ�ó���ʵ��
	application	= (HINSTANCE)	instance;

	// �����ź�
	if((status = CreateEvent (NULL, TRUE, FALSE, NULL)) == NULL) {
		return 0;
	}

	// ������
	WNDCLASS	wndclass;
	// ��Ϣ
	MSG			msg;
	// �����С
	RECT		rect;

	// �����ڴ�
	ZeroMemory(& wndclass,	sizeof wndclass);
	ZeroMemory(& msg,		sizeof msg);
	ZeroMemory(& rect,		sizeof rect);

	// ������
	wndclass.hInstance		=   application;
	wndclass.lpfnWndProc	= & message;
	wndclass.lpszClassName	= CLASS;
	wndclass.hbrBackground	= (HBRUSH) COLOR_BTNSHADOW;
	wndclass.hIcon			= LoadIcon		(application,	MAKEINTRESOURCEA(102));				// IDI_ICON2
	wndclass.hCursor		= LoadCursor	(NULL,			IDC_ARROW);

	// ע�ᴰ����
	if(RegisterClass(& wndclass) == 0) {
		return 0;
	}

	// ��ȡ�����С
	GetWindowRect(GetDesktopWindow(), &rect);

	// ��������
	if((window = CreateWindowEx (
		/* dwExStyle */		0,
		/* lpClassName */	CLASS,
		/* lpWindowName */	TITLE,
		/* dwStyle */		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		/* x */				(rect.right - WIDTH) / 2,
		/* y */				(rect.bottom - HEIGHT) / 2,
		/* nWidth */		WIDTH,
		/* nHeight */		HEIGHT,
		/* hWndParent */	NULL,
		/* hMenu */			NULL,
		/* hInstance */		application,
		/* lpParam */		0
	)) == NULL) {
		return 0;
	}

	// ��ʾ����
	ShowWindow(window, show);

	// ˢ�´���
	UpdateWindow(window);

	// ��Ϣѭ��
	while(GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage	(&msg);
		DispatchMessage		(&msg);
	}

	return 0;
}


/* GUI ���� */
void gui_response (const char *error) {
	// ȡ������״̬
	ResetEvent(status);

	// ȡ���������
	disable(false);

	// �ж��Ƿ���ִ���
	if(error != NULL) {
		// ����������ʾ
		MessageBoxA(window, (LPCSTR) error, "", MB_OK | MB_ICONERROR);

		return;
	}

	// ������Ϣѭ��
	SendMessage(window, WM_DESTROY, 0, 0);
}

