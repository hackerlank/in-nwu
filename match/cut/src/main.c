/*
	$ main.c, 0.2.1221
*/

/* @ ������: ��ϵͳ
     ���� vc11, ��ʹ�� v110_xp ƽ̨���߼����ֶ�������ϵͳ */
#if _MSC_VER < 1700
	#pragma comment (linker, "/subsystem:windows")
#endif

/* @ ������: �嵥�ļ� */
#pragma comment (linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
/* @ ComCtl32.lib */
#pragma comment (lib, "ComCtl32.lib")


#include "base.h"
#include <Windows.h>


/* �������� */
number_s	*NUM		= NULL;
/* ��������Ϣ */
HWND		 win		= NULL;
HINSTANCE	 instance	= NULL;


/* ����� */
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	instance = hInstance;

	/* ��ʼ������ */
	NEW(NUM, number_s, 1);

	/* ����ͼ���û����� */
	gui();

	return 0;
}
