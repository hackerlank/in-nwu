/*
	$ P2MP3  (C) 2005-2012 mfboy
	$ win32.c, 0.1.1005
*/

#include "base.h"
#include "win32.h"

/* Ĭ���ļ�ɸѡ�� */
#define FILTER		"�����ļ� (*.*)\0*.*\0"

/* ���봰�ڵĳ��� */
#define IN_WIDTH	380
#define IN_HEIGHT	145


/* ���봰�ڽṹ��װ */
static struct {
	HINSTANCE inst;

	char *message;		/* ��ʾ��Ϣ */
	char *deftext;		/* Ĭ���ı� */

	char *buffer;		/* �����ı������� */
	seek_t maxlen;		/* ������볤�� */

	char cancel;		/* ������ȡ����ť */

	HWND edit;			/* ������� */
} in = {NULL, NULL, NULL, NULL, 0, 0, NULL};


void explorer(char *filename) {
	char *cmd;

	ms_new(cmd, strlen(filename) + 8);

	strcat(cmd, "/select,");
	strcat(cmd, filename);

	ShellExecute(NULL, "open", "explorer.exe", cmd, NULL, SW_SHOWNORMAL);

	m_del(cmd);
}

char *fileext(char *filename) {
	char *ext = NULL;
	char *pos = filename + strlen(filename);

	while(pos >= filename) {
		if((*pos == '/') || (*pos == '\\')) {
			return ext;
		}
		if(*pos == '.') {
			return pos + 1;
		}

		pos--;
	}

	return NULL;
}


char *select_file_open(const char *filter, const char *title) {
	OPENFILENAME file;

	char *name;
	char buffer[MAX_PATH] = {0};

	/* ʹ��Ĭ��ɸѡ�� */
	if(filter == NULL) {
		filter = FILTER;
	}

	ZeroMemory(&file, sizeof file);

	file.lStructSize = sizeof file;
	file.hwndOwner = hwnd;
	file.lpstrTitle = TEXT(title);
	file.lpstrFilter = TEXT(filter);
	file.nFilterIndex = 1;
	file.lpstrFile = buffer;
	file.nMaxFile = sizeof(buffer);
	file.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

	if(GetOpenFileName(&file) == 1) {
		ms_new(name, strlen(buffer));

		strcpy(name, buffer);

		return name;
	} else {
		return NULL;
	}
}

char *select_file_save(char *filename, const char *title) {
	OPENFILENAME file;

	char buffer[MAX_PATH] = {0};
	char *name = NULL;

	char *filter = FILTER;
	char *ext = NULL, *extup, *extlw;

	seek_t i, j = 0;

	if(filename != NULL) {
		if(strlen(filename) < MAX_PATH) {
			strcpy(buffer, filename);
		}

		ext = fileext(filename);
	}

	if(ext != NULL) {
		i = 0;
		j = (seek_t) strlen(ext);

		ms_new(extup, j);
		ms_new(extlw, j);

		while(*ext) {
			extup[i] = (char) toupper(*ext);
			extlw[i] = (char) tolower(*ext);

			i++;
			ext++;
		}

		ms_new(filter, j * 3 + strlen(FILTER) + 32);

		strcat(filter, extup);
		strcat(filter, " �ļ� (*.");
		strcat(filter, extlw);
		strcat(filter, ")");

		i = (seek_t) strlen(filter) + 1;
		m_copy(filter + i, "*.", 2);
		m_copy(filter + i + 2, extlw, j);
		m_copy(filter + i + j + 3, FILTER, strlen(FILTER) + 5);
	}

	ZeroMemory(&file, sizeof(OPENFILENAME));

	file.lStructSize = sizeof(OPENFILENAME);
	file.hwndOwner = hwnd;
	file.lpstrTitle = TEXT(title);
	file.lpstrFilter = TEXT(filter);
	file.nFilterIndex = 1;
	file.lpstrFile = buffer;
	file.lpstrDefExt = TEXT(extlw);
	file.nMaxFile = sizeof(buffer);
	file.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

	if(GetSaveFileName(&file) == 1) {
		ms_new(name, strlen(buffer));

		strcpy(name, buffer);
	}

	if(j) {
		m_del(extup);
		m_del(extlw);

		m_del(filter);
	}

	return name;
}


static int WINAPI inputbox_callback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int i;

	HDC hdc;
	HFONT font;

	switch(uMsg) {
		case WM_CREATE:
			/* ��ʾ��Ϣ */
			CreateWindowEx(0, "Static", in.message, WS_CHILD | WS_VISIBLE, 15, 15, 275, 70, hWnd, (HMENU) 1, NULL, 0);
			/* ��ť */
			CreateWindowEx(0, "Button", "ȷ ��", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 15, 68, 65, 23, hWnd, (HMENU) 2, NULL, 0);
			CreateWindowEx(0, "Button", "ȡ ��", WS_CHILD | WS_VISIBLE, 90, 68, 65, 23, hWnd, (HMENU) 3, NULL, 0);
			/* ����� */
			in.edit = CreateWindowEx(WS_EX_CLIENTEDGE, "Edit", in.deftext, WS_CHILD | WS_VISIBLE, 15, 35, IN_WIDTH - 45, 20, hWnd, (HMENU) 4, in.inst, 0);

			hdc = GetDC(hWnd);
			font = CreateFont(12, 6, 0, 0, 12, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "����");

			SelectObject(hdc, font);
			ReleaseDC(hWnd, hdc);

			/* �������� */
			for(i = 1; i <= 4; i++) {
				SendDlgItemMessage(hWnd, i, WM_SETFONT, (WPARAM) font, 0);
			}

			break;

		case WM_KEYDOWN:
			/* ��Ӧ Enter �� */
			if(wParam == VK_RETURN) {
				SendMessage(hWnd, WM_COMMAND, 2, 0);
			}
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case 2:
					GetDlgItemText(hWnd, 4, in.buffer, (int) in.maxlen);

					DestroyWindow(hWnd); break;

				case 3:
					in.cancel = 1;

					DestroyWindow(hWnd); break;
			}

			break;

		case WM_SETFOCUS:
			SetFocus(in.edit);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return (int) DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 1;
}

char *inputbox(const char *title, char *message, char *deftext) {
	HWND hDesktop;
	RECT rcDesktop;
	HWND hWnd;
	MSG msg;
	WNDCLASS wnd;

	int posX, posY;

	if(!in.inst) {
		in.inst = GetModuleHandle(NULL);
	}

	in.message	= message;
	in.deftext	= deftext;
	in.maxlen	= 256;
	in.cancel	= 0;

	m_new(in.buffer, char, in.maxlen);
	
	hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &rcDesktop);

	posX = (rcDesktop.right - IN_WIDTH) / 2;
	posY = (rcDesktop.bottom - IN_HEIGHT - 20) / 2;

	m_zero(&wnd, sizeof wnd);

	wnd.hInstance		= in.inst;
	wnd.hbrBackground	= (HBRUSH) COLOR_BTNSHADOW;
	wnd.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wnd.hCursor			= LoadCursor(0, IDC_ARROW);
	wnd.lpfnWndProc		= (WNDPROC) inputbox_callback;
	wnd.lpszClassName	= "INPUTBOX_CLASS";
	wnd.style			= CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wnd);

	if((hWnd = CreateWindow("INPUTBOX_CLASS", title, 0, posX, posY, IN_WIDTH, IN_HEIGHT, hwnd, NULL, in.inst, NULL)) == NULL) {
		die("����Ӧ�ó��򴰿�ʧ�ܡ�");
	}

	/* ������ʧЧ */
	EnableWindow(hwnd, 0);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	while(GetMessage(&msg, 0, 0, 0) > 0) {
		if(msg.message == WM_KEYDOWN) {
			if(msg.wParam == VK_RETURN) {
				SendMessage(hWnd, msg.message, msg.wParam, msg.wParam);
			}
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	/* �ָ������� */
	EnableWindow(hwnd, 1);
	SetForegroundWindow(hwnd);

	if(in.cancel) {
		m_del(in.buffer);

		return NULL;
	}

	return in.buffer;
}
