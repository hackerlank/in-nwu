/*
	$ Robot for LinLink   (C) 2005-2013 MF
	$ main.c
*/

/* @ for: Psapi.h */
#pragma comment (lib, "Psapi.lib")
/* @ for: Commctrl.h */
#pragma comment (lib, "ComCtl32.lib")

/* @ ������: ��ϵͳ */
#pragma comment (linker, "/subsystem:\"windows\"")
/* @ ������: �嵥�ļ� */
#pragma comment (linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")


#include <Windows.h>
#include <Commctrl.h>
#include "base.h"


/* ����ʵ�� */
HINSTANCE	instance;
/* ͬ���ź� */
HANDLE		symbolRun, symbolStop, symbolExit, symbolDestroy;


/* ��ʼ�� ComCtl32.dll */
static void initGui() {
	INITCOMMONCONTROLSEX iccx;

	iccx.dwSize	= sizeof iccx;
	iccx.dwICC	= ICC_STANDARD_CLASSES;

	InitCommonControlsEx(&iccx);
}

/* ��ʼ����������, ���õ���Ȩ�� */
static void initToken() {
	HANDLE				token;
	TOKEN_PRIVILEGES	privilege;

	do {
		if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token) == FALSE) {
			break;
		}
		if(LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &privilege.Privileges[0].Luid) == FALSE) {
			break;
		}

		privilege.PrivilegeCount = 1;
		privilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if(AdjustTokenPrivileges(token, FALSE, &privilege, sizeof privilege, NULL, NULL) == FALSE) {
			break;
		}
		if(GetLastError() != ERROR_SUCCESS) {
			break;
		}

		CloseHandle(token);

		return;
	} while(0);

	DIE("��ȡ����Ȩ��ʧ�ܣ��볢��ʹ�ù���ԱȨ��������������");
}

/* ��ʼ�������߳�ͬ���ź��¼� */
static void initEvent() {
	int i;

	/* ���е��ź� */
	void * symbols[] = {&symbolRun, &symbolStop, &symbolExit, &symbolDestroy};

	for(i = 0; i < (sizeof(symbols) / sizeof(void *)); i++) {
		if((*((HANDLE *) symbols[i]) = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL) {
			DIE("���򴴽��߳�ͬ���ź�ʱ�������󣬱�����ֹ��");
		}
	}
}


/* �˳������� */
static void exitWait() {
	/* �ȴ�����ִ����� */
	WAIT(Exit);
	/* ���������ź� */
	SEND(Destroy);

	/* �ȴ� 1s */
	Sleep(1000);
}


/* ����� */
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	instance = hInstance;

	/* ��ʼ������ */
	initGui();
	initToken();
	initEvent();

	/* ���� GUI �߳� */
	THRDRUN(gui);
	/* ���������߳� */
	THRDRUN(debug);

	/* �ȴ�����ִ����� */
	exitWait();

	return 0;
}
