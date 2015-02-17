/*
	$ Monitor Loader   (C) 2005-2014 MF
	$ call.cpp
*/


#include "base.h"



/* ��ȡģ��·�� */
static bool call_module (char *buffer, const char *dll) {
	// ����ڴ�
	memset(buffer, 0, MAX_PATH + 1);

	// ��ȡ EXE �ļ�����·��
	if(GetModuleFileNameA(NULL, buffer, MAX_PATH) == 0) {
		return false;
	}

	// �������һ�� '\'
	char * end	= strrchr(buffer, '\\');

	// ���û�� '\', ������ȡʧ��
	if(end == NULL) {
		return false;
	}

	// ���� '\' ���Ƿ��㹻�Ŀռ����� dll �ļ���
	if((buffer + MAX_PATH - end - 1) <= (int) strlen(dll)) {
		return false;
	}

	// ���� dll �ļ���
	strcpy(end + 1, dll);

	return true;
}


/* ��ȡ����ϵͳ�汾 */
static bool call_system (void) {
	// ��ȡ�汾��Ϣ
	unsigned long version = (unsigned long) GetVersion();

	// 5.x Ϊ 2000/XP ϵͳ
	if(LOBYTE(LOWORD(version)) <= 5) {
		return true;
	}

	return false;
}




/* ִ��ע�� */
static const char * call_inject (void *process, const char *handler, bool console, const char *dll, const char *function) {
	// Զ���߳�
	void *		thread;

	// ע��ģ�鹤���������
	uintptr_t	entry;
	// ע��ģ�鹤���������ؽ��
	uintptr_t	result;

	// ����ע������ڴ�
	uintptr_t	address	= process_memory (process);

	// �ж��Ƿ����ɹ�
	if(address == 0) {
		return "�޷���Ŀ������з����ڴ档";
	}

	// ע�����
	if(remote_inject(process, address, dll, function) == 0) {
		return "��Ŀ�����ע�����ʧ�ܡ�";
	}

	// ���ƿ���̨������Ϣ
	if(remote_write(process, address + REMOTE_MEMORY_CONSTANT + REMOTE_MEMORY_CODE, & console, 4) == false) {
		return "��Ŀ����̸��ƿ���̨����״̬ʧ��";
	}

	// ���ƿ�����·��
	if(remote_write(process, address + REMOTE_MEMORY_CONSTANT + REMOTE_MEMORY_CODE + 4, (void *) handler, REMOTE_MEMORY_HANDLER) == false) {
		return "��Ŀ����̸��ƿ�����·��ʧ�ܡ�";
	}

	// ������ʼ���߳�
	if((thread = remote_thread (process, address + REMOTE_MEMORY_CONSTANT, 0)) == NULL) {
		return "��Ŀ�������������ʼ���߳�ʧ�ܡ�";
	}

	// �ȴ��߳̽���
	WaitForSingleObject(thread, INFINITE);

	// ��ȡע��ģ�鹤���������
	if(GetExitCodeThread(thread, (LPDWORD) & entry) == FALSE) {
		return "��ȡ Monitor ע��ģ�鹤���������ʧ�ܡ�";
	}

	// �жϺ�������Ƿ�Ϸ�
	if(entry == 0) {
		return "��ȡ Monitor ע��ģ�鹤���������ʧ�ܡ�";
	}

	// �ͷų�ʼ���߳̾��
	CloseHandle(thread);

	// �������������߳�
	if((thread = remote_thread (process, entry, address + REMOTE_MEMORY_CONSTANT + REMOTE_MEMORY_CODE)) == NULL) {
		return "��Ŀ�����������ע��ģ�鹤�������߳�ʧ�ܡ�";
	}

	// �ȴ��߳̽���
	WaitForSingleObject(thread, INFINITE);

	// ��ȡ�����������ؽ��
	if(GetExitCodeThread(thread, (LPDWORD) & result) == FALSE) {
		return "��ȡ Monitor ע��ģ�鹤���������ؽ��ʧ�ܡ�";
	}

	// �ͷŹ��������߳̾��
	CloseHandle(thread);

	// �жϴ�����
	switch(result) {
		// �ɹ�
		case ML_ERROR_SUCCEED:		break;

		// ʧ��
		case ML_ERROR_CONSOLE:		return "�����µĿ���̨����ʧ�ܡ�\n\nÿһ������ֻ��ӵ��һ������̨���ڣ����ʹ�� Monitor ���ؿ���̨Ӧ�ó��������ò���ʾ����̨���ڡ�";
		case ML_ERROR_THREAD:		return "���� Monitor �����߳�ʧ�ܡ�";

		// ����������
		case ML_ERROR_HANDLER_UNKNOWN:				return "��֧�ֵ�ǰ���͵Ŀ�������";
		case ML_ERROR_HANDLER_DLL:					return "���� DLL ������ʧ�ܡ�";
		case ML_ERROR_HANDLER_PHP:					return "���� PHP ������ʧ�ܡ�";

		// �ٳֺ���ʧ��
		case ML_ERROR_FUNCTION_CREATEFILEW:			return "�޷��ٳ� CreateFileW() ������";
		case ML_ERROR_FUNCTION_LOADLIBRARYEXW:		return "�޷��ٳ� LoadLibraryExW() ������";
		case ML_ERROR_FUNCTION_CREATEPROCESSW:		return "�޷��ٳ� CreateProcessW() ������";

		// δ֪����
		default:		return "Monitor ע��ģ�鹤���������ش���";
	}

	return NULL;
}


/* ִ���߳� */
static int WINAPI call_thread (void *command) {
	// ��������
	const char **	argument	= (const char **) command;

	// ������Ϣ
	const char *	run		= argument [0];
	const char *	handler	= argument [1];
	// ��ʾ����̨����
	bool	console	= (bool)  argument [2];

	// �жϿ���������
	if(strlen(handler) >= REMOTE_MEMORY_HANDLER) {
		gui_response ("������·����������ĳ��ȡ�");

		goto done;
	}

	// DLL �ļ���
	char		dll			[MAX_PATH + 1];
	// ע��ģ�鹤��������
	const char	function	[ ]	= "monitor_main";

	// ��ȡ������ DLL �ļ�·��
	if(call_module(dll, "monitor.dll") == false) {
		gui_response ("�޷���ȡ��Ҫע���ģ���·����");

		goto done;
	}

	// ������Ϣ
	void *	process	= NULL;
	void *	thread	= NULL;

	// ��������
	process = process_start(run, & thread);

	// �ж��Ƿ������ɹ�
	if(process == NULL) {
		gui_response ("�޷�����ָ����Ӧ�ó���\n\n���������·���Ƿ���ȷ������пո��·��������ָ���Լ�����Ȩ���Ƿ��㹻������Ҫ����ԱȨ�ޣ��ȡ�");

		goto done;
	}

	// ������ϰ汾 Windows, �����������߳�����һ��ʱ������ɳ�ʼ������
	if(call_system() == true) {
		// ��ʾ
		MessageBoxA(NULL, "������ʹ�õͰ汾�� Windows ����ϵͳ������ע���̲߳����������߳����У����޷�����Ӧ�ó����ʼ���׶εĺ������ù��̡�", "", MB_ICONWARNING | MB_OK);

		// �ָ����߳�
		ResumeThread((HANDLE) thread);

		// �ȴ� 500ms
		Sleep(500);

		// �ٴι������߳�
		SuspendThread((HANDLE) thread);
	}

	// ִ��ע�빤��
	const char * error = call_inject (process, handler, console, dll, function);

	if(error == NULL) {
		// ���û�г��ִ���, �������߳�
		ResumeThread(thread);

		// �ͷž��
		CloseHandle(thread);
		CloseHandle(process);
	} else {
		// ������ִ���, ��ֹ����
		process_close(process, thread);
	}

	// ���÷���
	gui_response (error);


done:
	// �ͷ�������Ϣ
	delete [] run;

	// �ͷſ�����·��
	delete [] handler;

	return 0;
}



/* ִ������ */
void call (const char *run, const char *handler, bool console) {
	// �̲߳���
	static const char * argument [ ] = {
		/* run */		NULL,
		/* handler */	NULL,
		/* console */	NULL,
	};

	// �ж��Ƿ��ṩ����������
	if(run == NULL) {
		gui_response ("û������Ӧ�ó���·��������������");

		return;
	}

	// �ж��Ƿ��ṩ�˿�����·��
	if(handler == NULL) {
		// ���� 4 ���ֽڻ�����
		handler = new char [4];

		// ����Ϊ 32 λ 0
		memset((void *) handler, 0, 4);
	}

	// �����̲߳���
	argument [0] = run;
	argument [1] = handler;
	argument [2] = (const char *) console;

	// ���������߳�
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) call_thread, (LPVOID) argument, 0, NULL);
}

