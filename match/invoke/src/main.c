/*
	$ Invoke   (C) 2005-2012 MF
	$ main.c, 0.1.1124
*/

#pragma comment (linker, "/subsystem:\"windows\"")

#define _INVOKE_MAIN_

#include "base.h"
#include "gui.h"


/* ���󱨸���� */
void mainError(char *command) {
	if(strlen(command) < 9) {
		return;
	}
	if(strncmp(command, "--error ", 8)) {
		return;
	}

	error(NULL, command + 8);

	ExitProcess(EXIT_FAILURE);
}

/* Win32 ����� */
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	/* ���뺯�� */
	Function(&processSuspend,	"ntdll.dll",	"NtSuspendProcess", FUNCTION_PASSIVE);
	Function(&processResume,	"ntdll.dll",	"NtResumeProcess",	FUNCTION_PASSIVE);
	Function(&disasm,			"disasm.dll",	"disasm",			FUNCTION_PASSIVE);

	/* ���󱨸� */
	mainError(lpCmdLine);

	/* @ RUNTIME */
	ZeroMemory(&RT, sizeof RT);
	/* @ HIJACKED-FUNCTIONS/MODULES for debugging process */
	NEW(HIJ, hijack_s, 1);
	NEW(MOD, module_s, 1);

	/* ��ʼ���ź� */
	symbolInit();

	/* ��ǰ������Ϣ */
	RT.instance	= hInstance;
	RT.process	= GetCurrentProcess();
	/* �����������߳� */
	RT.debug	= (HANDLE) threadRun(runDebug, NULL);

	/* ��ʼ�� Europa GUI */
	erpInit();

	/* ѡ����� */
	guiProc();

	/* ģ�麯���ϵ����� */
	moduleShow();

	/* ���� ivh �ļ� */
	packageAuto();
	/* �ֶ����ýٳֺ��� */
	guiHij(NULL);

	/* ������ */
	guiMain();

	/* ���Խ���������ϵ� */
	if(RT.uninstall) {
		hookUninstallAll();
	}
}
