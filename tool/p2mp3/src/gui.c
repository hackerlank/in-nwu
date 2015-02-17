#define _GUI_

/* windows �������ʽ���� */
#pragma comment (linker, "/subsystem:windows /entry:mainCRTStartup")

/* �� manifest ��ʵ�� XP ��ʽ */
#if defined _M_IX86
	#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
	#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
	#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
	#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif


#include "base.h"
#include "gui.h"
#include "execute.h"


/* ���ڱ��� */
#define WIN_TITLE	"Push To MP3"
/* ���ڳ��� */
#define WIN_WIDTH	600
#define WIN_HEIGHT	195


static HWND buttonPush, buttonPop;
int running = 0;


static int WINAPI gui_callback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int i;

	HDC hdc;
	HFONT font;

	switch(uMsg) {
		case WM_CREATE:
			/* ˵�� */
			CreateWindow("Static", TEXT("������ϲ��ļ������ɽ�һ���������͵��ļ��ϲ��� MP3 �ļ���"), WS_CHILD | WS_VISIBLE, 15, 18, 430, 15, hWnd, (HMENU) 1, NULL, 0);
			CreateWindow("Static", TEXT("�����������ѡ�� MP3 Դ�ļ�����Ҫ�ϲ����ļ��ͺϲ���������·��"), WS_CHILD | WS_VISIBLE, 15, 34, 430, 15, hWnd, (HMENU) 2, NULL, 0);
			CreateWindow("Static", TEXT("�����Ҫ�Ժϲ����ļ����ݽ��м��ܣ��ںϲ������и�����ʾ������Կ����"), WS_CHILD | WS_VISIBLE, 15, 50, 430, 15, hWnd, (HMENU) 3, NULL, 0);
			CreateWindow("Static", TEXT("�������ȡ�ļ������ɽ�ʹ�ñ�����ϲ��� MP3 �ļ��е�������ȡ����"), WS_CHILD | WS_VISIBLE, 15, 90, 430, 15, hWnd, (HMENU) 5, NULL, 0);
			CreateWindow("Static", TEXT("����ļ��ںϲ�ʱ���ݾ������ܣ�������ȡ��������Ҫ���ṩ��ȷ����Կ"), WS_CHILD | WS_VISIBLE, 15, 106, 430, 15, hWnd, (HMENU) 6, NULL, 0);
			CreateWindow("Static", TEXT("��ȡ��Ϻ���ѡ�����·�������򽫻��Զ�����ȡ���������ݱ��浽��·��"), WS_CHILD | WS_VISIBLE, 15, 122, 430, 15, hWnd, (HMENU) 7, NULL, 0);

			/* ��ť */
			buttonPush	= CreateWindowEx(0, "Button", "�ϲ��ļ�", WS_CHILD | WS_VISIBLE, 450, 15, 120, 55, hWnd, (HMENU) 4, NULL, 0);
			buttonPop	= CreateWindowEx(0, "Button", "��ȡ�ļ�", WS_CHILD | WS_VISIBLE, 450, 85, 120, 55, hWnd, (HMENU) 8, NULL, 0);

			hdc = GetDC(hWnd);
			font = CreateFont(12, 6, 0, 0, 12, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "����");

			SelectObject(hdc, font);
			ReleaseDC(hWnd, hdc);

			for(i = 1; i <= 8; i++) {
				SendDlgItemMessage(hWnd, i, WM_SETFONT, (WPARAM) font, 0);
			}

			break;

		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case 4: push(); break;
				case 8: pop(); break;
			}

			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return (int) DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 1;
}


void gui() {
	HINSTANCE inst;
	HWND hDesktop;
	RECT rcDesktop;
	MSG msg;
	WNDCLASS wnd;

	int posX, posY;

	inst		= GetModuleHandle(NULL);
	hDesktop	= GetDesktopWindow();

	GetWindowRect(hDesktop, &rcDesktop);

	posX = (rcDesktop.right - WIN_WIDTH) / 2;
	posY = (rcDesktop.bottom - WIN_HEIGHT - 20) / 2;

	m_zero(&wnd, sizeof wnd);

	wnd.hInstance		= inst;
	wnd.hbrBackground	= (HBRUSH) COLOR_BTNSHADOW;
	wnd.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wnd.hCursor			= LoadCursor(0, IDC_ARROW);
	wnd.lpfnWndProc		= (WNDPROC) gui_callback;
	wnd.lpszClassName	= "P2MP3_CLASS";
	wnd.style			= CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wnd);

	if((hwnd = CreateWindow("P2MP3_CLASS", WIN_TITLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, posX, posY, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, inst, NULL)) == NULL) {
		die("����Ӧ�ó��򴰿�ʧ�ܡ�");
	}

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, 0, 0, 0) > 0) {
		if(msg.message == WM_KEYDOWN) {
			SendMessage(hwnd, msg.message, msg.wParam, msg.wParam);
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


void buttonEnable(int enable) {
	EnableWindow(buttonPush, enable);
	EnableWindow(buttonPop, enable);
}

void buttonPushText(char *text) {
	if(text == NULL) {
		SetWindowText(buttonPush, "�ϲ��ļ�");
	} else {
		SetWindowText(buttonPush, text);
	}
}
