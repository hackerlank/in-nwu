/*
	$ Tracker w64   (C) 2005-2014 MF
	$ tracker.exe/gui.cpp   # 1312
*/


#pragma comment (linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")


#include "base.h"




/* ������Ϣ */
#define CLASS				"tracker_loader_class"
#define TITLE				"Tracker w64"
#define WIDTH				420
#define HEIGHT				230



/* ���ڿؼ���Ÿ�λ */
#define ID							0x1000

/* ���ڿؼ���ŵ�λ */
#define ID_LABEL_COMMAND			0x01
#define ID_LABEL_CONTROLLER			0x02
#define ID_TEXT_COMMAND				0x03
#define ID_TEXT_CONTROLLER			0x04
#define ID_BUTTON_OK				0x05
#define ID_BUTTON_BROWSE			0x06





// ��ǰ״̬
HANDLE		gui::status		= NULL;

// Ӧ�ó���ʵ��
HINSTANCE	gui::instance	= NULL;

// ���ھ��
HWND		gui::window		= NULL;
// �ؼ����
HWND		gui::component	[8];





/* ѡ���ļ� */
void gui::browse (void) {
	// ���뻺����
	char	buffer	[MAX_PATH + 1];

	// �ļ���Ϣ
	OPENFILENAME	file;

	ZeroMemory (& buffer,	sizeof buffer);
	ZeroMemory (& file,		sizeof file);

	// �����ļ���Ϣ
	file.lStructSize	= sizeof file;
	file.lpstrFile		= buffer;
	file.hwndOwner		= window;
	file.lpstrTitle		= "ѡ��Ӧ�ó���";
	file.lpstrFilter	= "��ִ���ļ�\0*.exe\0";
	file.nFilterIndex	= 1;
	file.nMaxFile		= MAX_PATH;
	file.Flags			= OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

	// ���ļ�ѡ��Ի���
	if(GetOpenFileNameA (& file) == FALSE) {
		return;
	}

	// ����ѡ�ļ�·�����õ��ı�����
	SetWindowTextA (gui::component [ID_TEXT_COMMAND], buffer);
}



/* ��ֹ���� */
void gui::disable (bool status) {
	// ϵͳ�˵�
	HMENU	menu	= GetSystemMenu (gui::window, FALSE);

	if(status == true) {
		// ��������
		EnableWindow	(gui::component [ID_TEXT_COMMAND	], FALSE);
		EnableWindow	(gui::component [ID_TEXT_CONTROLLER	], FALSE);
		EnableWindow	(gui::component [ID_BUTTON_OK		], FALSE);
		EnableWindow	(gui::component [ID_BUTTON_BROWSE	], FALSE);

		// ���ùرհ�ť
		EnableMenuItem	(menu, SC_CLOSE, MF_DISABLED | MF_GRAYED);
	} else {
		// ��������
		EnableWindow	(gui::component [ID_TEXT_COMMAND	], TRUE);
		EnableWindow	(gui::component [ID_TEXT_CONTROLLER	], TRUE);
		EnableWindow	(gui::component [ID_BUTTON_OK		], TRUE);
		EnableWindow	(gui::component [ID_BUTTON_BROWSE	], TRUE);

		// ���ùرհ�ť
		EnableMenuItem	(menu, SC_CLOSE, MF_ENABLED);
	}
}




/* ���ƴ��� */
int gui::draw (HWND hwnd) {
	// �ɻ�������
	RECT	rect;

	// ��ȡ�ɻ��������С
	GetClientRect (hwnd, & rect);

	// ��ǩ
	gui::component [ID_LABEL_COMMAND	]	= CreateWindowExA (0, "STATIC", "Ӧ�ó���·��������������",	WS_CHILD | WS_VISIBLE, 15, 15, rect.right - 30, 30, hwnd, (HMENU) (ID | ID_LABEL_COMMAND),		gui::instance, 0);
	gui::component [ID_LABEL_CONTROLLER	]	= CreateWindowExA (0, "STATIC", "׷�ٿ�����ģ��·����",		WS_CHILD | WS_VISIBLE, 15, 75, rect.right - 30, 30, hwnd, (HMENU) (ID | ID_LABEL_CONTROLLER),	gui::instance, 0);

	// �ı���
	gui::component [ID_TEXT_COMMAND		]	= CreateWindowExA (WS_EX_CLIENTEDGE, "EDIT", load::command,		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,	15, 37, rect.right - 30, 22, hwnd, (HMENU) (ID | ID_TEXT_COMMAND),		gui::instance, 0);
	gui::component [ID_TEXT_CONTROLLER	]	= CreateWindowExA (WS_EX_CLIENTEDGE, "EDIT", load::controller,	WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,	15, 97, rect.right - 30, 22, hwnd, (HMENU) (ID | ID_TEXT_CONTROLLER),	gui::instance, 0);

	// ��ť
	gui::component [ID_BUTTON_OK		]	= CreateWindowExA (0, "BUTTON", "ȷ��",				WS_CHILD | WS_VISIBLE, rect.right - 230, rect.bottom - 43,  90, 28, hwnd, (HMENU) (ID | ID_BUTTON_OK),		gui::instance, 0);
	gui::component [ID_BUTTON_BROWSE	]	= CreateWindowExA (0, "BUTTON", "ѡ��Ӧ�ó��� ...",	WS_CHILD | WS_VISIBLE, rect.right - 135, rect.bottom - 43, 120, 28, hwnd, (HMENU) (ID | ID_BUTTON_BROWSE),	gui::instance, 0);

	// ���洰�ھ��
	gui::window		= hwnd;

	// ��ȡ��ͼ�豸
	HDC		hdc		= GetDC (hwnd);
	// ��ȡϵͳĬ������
	HFONT	hfont	= (HFONT) GetStockObject (DEFAULT_GUI_FONT);

	// ѡ������
	SelectObject	(hdc, hfont);
	// �ͷ��豸
	ReleaseDC		(hwnd, hdc);

	// ��������
	SendDlgItemMessage (hwnd, ID | ID_LABEL_COMMAND,	WM_SETFONT, (WPARAM) hfont, 0);
	SendDlgItemMessage (hwnd, ID | ID_LABEL_CONTROLLER,	WM_SETFONT, (WPARAM) hfont, 0);
	SendDlgItemMessage (hwnd, ID | ID_TEXT_COMMAND,		WM_SETFONT, (WPARAM) hfont, 0);
	SendDlgItemMessage (hwnd, ID | ID_TEXT_CONTROLLER,	WM_SETFONT, (WPARAM) hfont, 0);
	SendDlgItemMessage (hwnd, ID | ID_BUTTON_OK,		WM_SETFONT, (WPARAM) hfont, 0);
	SendDlgItemMessage (hwnd, ID | ID_BUTTON_BROWSE,	WM_SETFONT, (WPARAM) hfont, 0);

	// �ж��Ƿ��Զ���ʼ
	if((load::command != NULL) && (load::controller != NULL)) {
		// ���Ͱ�ť�����¼�
		SendMessage (gui::component [ID_BUTTON_OK], BM_CLICK, 0, 0);
	}

	return 0;
}


/* ���ٴ��� */
bool gui::close (void) {
	// �ж��Ƿ�Ϊ����״̬
	if(WaitForSingleObject (gui::status, 0) == WAIT_OBJECT_0) {
		MessageBoxA (gui::window, "Ӧ�ó������������У���Ӧ�ó�����������󣬴��ڽ��Զ��رա�", "", MB_ICONINFORMATION | MB_OK);

		return true;
	}

	return false;
}



/* �¼����� */
void gui::command (WPARAM wparam) {
	// �ؼ� ID
	WORD id = LOWORD(wparam);

	// ���˸�λ
	id	= id & (~ ID);

	// �����¼�
	switch(id) {
		case ID_BUTTON_OK:
			{
				// ��������ֳ���
				int		length_command		= GetWindowTextLengthA (gui::component [ID_TEXT_COMMAND		]) + 1;
				int		length_controller	= GetWindowTextLengthA (gui::component [ID_TEXT_CONTROLLER	]) + 1;

				// �ж��Ƿ�������������Ϣ
				if(length_command <= 1) {
					MessageBoxA (gui::window, "������Ӧ�ó���·��������������", "", MB_ICONWARNING | MB_OK);

					break;
				}

				// �ж��Ƿ������˿�����
				if(length_controller <= 1) {
					MessageBoxA (gui::window, "������׷�ٿ�����ģ��·����", "", MB_ICONWARNING | MB_OK);

					break;
				}

				// ���仺����
				load::command		= new char [length_command		];
				load::controller	= new char [length_controller	];

				// ��ȡ������Ϣ
				GetWindowTextA (gui::component [ID_TEXT_COMMAND		], load::command,		length_command);
				GetWindowTextA (gui::component [ID_TEXT_CONTROLLER	], load::controller,	length_controller);

				// �жϿ������Ƿ����������·��
				if((strchr (load::controller, '\\') == NULL) && (strchr (load::controller, '/') == NULL)) {
					if(MessageBoxA (gui::window, "�����׷�ٿ�����ģ��·��δ��������Ŀ¼�����������������仯ʱ�����ܻᵼ�¼��ؿ�����ʧ�ܡ��Ƿ������", "", MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) == IDNO) {
						// �ͷŻ�����
						delete [] load::command;
						delete [] load::controller;

						return;
					}
				}

				// �����ֹ����ģʽ
				gui::disable (true);

				// ����״̬
				SetEvent (gui::status);

				// ��������
				load::run();
			}

			break;

		case ID_BUTTON_BROWSE:
			// ѡ���ļ�
			gui::browse();

			break;
	}
}




/* ��Ϣѭ�� */
LRESULT __stdcall gui::loop (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch(msg) {
		// ���ƴ���
		case WM_CREATE:
			// ִ�д��ڻ���
			return (LRESULT) gui::draw (hwnd);

		// �رմ���
		case WM_CLOSE:
			// ��ֹ��Ϣѭ��
			if(gui::close() == true) {
				return 0;
			}

			break;

		// ���ٴ���
		case WM_DESTROY:
			PostQuitMessage(0);

			break;

		// ����ַ�
		case WM_COMMAND:
			// ������Ϣ����
			gui::command(wparam);
			
			break;
	}

	// ʹ��Ĭ����Ϣ������
	return DefWindowProc (hwnd, msg, wparam, lparam);
}





/* ���� GUI */
void gui::start (HINSTANCE instance, int show) {
	// Ӧ�ó���ʵ��
	gui::instance	= instance;

	// ����״̬�ź�
	if((gui::status = CreateEvent (NULL, TRUE, FALSE, NULL)) == NULL) {
		return;
	}

	// ������
	WNDCLASSA	wndclass;
	// ��Ϣ
	MSG			message;
	// �����С
	RECT		rectangle;

	// �����ڴ�
	ZeroMemory (& wndclass,		sizeof wndclass);
	ZeroMemory (& message,		sizeof message);
	ZeroMemory (& rectangle,	sizeof rectangle);

	// ������
	wndclass.hInstance		= gui::instance;
	wndclass.lpfnWndProc	= gui::loop;
	wndclass.lpszClassName	= CLASS;
	wndclass.hbrBackground	= (HBRUSH) COLOR_BTNSHADOW;
	wndclass.hCursor		= LoadCursor	(NULL,			IDC_ARROW);
	wndclass.hIcon			= LoadIcon		(gui::instance,	MAKEINTRESOURCEA(101));				// IDI_ICON1

	// ע�ᴰ����
	if(RegisterClassA (& wndclass) == 0) {
		return;
	}

	// ��ȡ�����С
	if(GetWindowRect (GetDesktopWindow(), & rectangle) == FALSE) {
		// ����Ĭ�Ϸֱ���
		rectangle.right		= WIDTH  * 2;
		rectangle.bottom	= HEIGHT * 2;
	}

	// ��������
	if((gui::window = CreateWindowExA (
		/* dwExStyle */		0,
		/* lpClassName */	CLASS,
		/* lpWindowName */	TITLE,
		/* dwStyle */		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		/* x */				(rectangle.right  - WIDTH ) / 2,
		/* y */				(rectangle.bottom - HEIGHT) / 2,
		/* nWidth */		WIDTH,
		/* nHeight */		HEIGHT,
		/* hWndParent */	NULL,
		/* hMenu */			NULL,
		/* hInstance */		gui::instance,
		/* lpParam */		0
	)) == NULL) {
		return;
	}

	// ��ʾ����
	ShowWindow		(gui::window, show);

	// ˢ�´���
	UpdateWindow	(gui::window);

	// ��Ϣѭ��
	while(GetMessage (& message, 0, 0, 0) > 0) {
		TranslateMessage	(& message);
		DispatchMessage		(& message);
	}
}





/* ��Ӧ�ɹ� */
void gui::response_succeed (void) {
	// ��ֹ��Ϣѭ��
	SendMessage (gui::window, WM_DESTROY, 0, 0);
}



/* ��Ӧʧ�� (������Ϣ) */
void gui::response_error (void) {
	// ����״̬
	ResetEvent (gui::status);

	// �ͷŻ�����
	delete [] load::command;
	delete [] load::controller;

	// �˳���ֹ����ģʽ
	gui::disable (false);
}


/* ��Ӧʧ�� (����Ϣ) */
void gui::response_error (const char *error) {
	// ����������Ϣ
	MessageBoxA (gui::window, error, "", MB_ICONERROR | MB_OK);

	// ����ͨ�ô���
	gui::response_error ( );
}

