/*
	$ Monitor Module   (C) 2005-2014 MF
	$ base.h
*/


#include "header/base.h"
#include "header/error.h"



/* �������� */
#pragma comment (linker, "/EXPORT:monitor_main=_monitor_main@4,PRIVATE")




/* �����ڴ�ʧ�ܻص����� */
static void __cdecl callback_new_exception (void) {
	// ������ʾ
	MessageBoxA(NULL, "�����ڴ�ʧ�ܣ�Ӧ�ó�������ֹ��", "", MB_ICONERROR | MB_OK);

	// ��������
	exit(0);
}



/* ׼���߳� */
static DWORD WINAPI monitor_thread (LPVOID lpParameter) {
	// �������
	Console::notice("#SYSTEM#Monitor thread executed\n");

	return 0;
}




/* �������� */
extern "C" int __stdcall monitor_main (uintptr_t argument) {
	// ���� new ʧ�ܻص�����
	std::set_new_handler (callback_new_exception);

	// ���ò���ֵ
	bool			console	= *((bool *) argument);
	const char *	script	= (const char *) (argument + 4);

	// ��������
	int		error	= 0;

	// �������̨����
	if(console == true) {
		if(Console::open() == false) {
			return MM_ERROR_CONSOLE;
		}
	}

	// ����������
	if((error = Handler::start(script)) != MM_ERROR_SUCCEED) {
		return error;
	}

	// ��װ�ٳ�
	if((error = Router::install()) != MM_ERROR_SUCCEED) {
		return error;
	}

	// ����׼���߳�
	if(CreateThread(NULL, 0, monitor_thread, NULL, 0, NULL) == NULL) {
		return MM_ERROR_THREAD;
	}

	// �ɹ���ɹ���
	return MM_ERROR_SUCCEED;
}




/* DLL ��� */
BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	switch(fdwReason) {
		// ���̿�ʼ: ���� DLL ���
		case DLL_PROCESS_ATTACH:	Handler::dll = (void *) hinstDLL;	break;
		// ���̽���: ����������
		case DLL_PROCESS_DETACH:	Handler::close();					break;

		// �߳̿�ʼ
		case DLL_THREAD_ATTACH:		break;
		// �߳̽���
		case DLL_THREAD_DETACH:		break;
	}

	return TRUE;
}

