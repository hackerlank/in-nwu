/*
	$ Monitor Loader   (C) 2005-2014 MF
	$ process.cpp
*/


#include "base.h"



/* �����½��� */
void * process_start (const char *command, void **thread) {
	// ��ȡ������������
	size_t	length	= strlen(command);
	// ����������������
	char *	line	= new char [length + 1];

	// ����������������
	strcpy(line, command);

	// ������Ϣ
	STARTUPINFOA		startup;
	// ������Ϣ
	PROCESS_INFORMATION	process;
	
	// �������������Ϣ
	ZeroMemory(& startup,	sizeof(startup));
	ZeroMemory(& process,	sizeof(process));

	// ��ʼ��������Ϣ
	startup.cb		= sizeof(startup);

	// ��������
	BOOL result = CreateProcessA (
		/* lpApplicationName */		NULL,
		/* lpCommandLine */			(LPSTR) line,
		/* lpProcessAttributes */	NULL,
		/* lpThreadAttributes */	NULL,
		/* bInheritHandles */		FALSE,
		/* dwCreationFlags */		CREATE_SUSPENDED,
		/* lpEnvironment */			NULL,
		/* lpCurrentDirectory */	NULL,
		/* lpStartupInfo */			& startup,
		/* lpProcessInformation */	& process
	);

	// �ͷ�������������
	delete [] line;

	// �ж��Ƿ������ɹ�
	if(result == FALSE) {
		return NULL;
	}

	if(thread == NULL) {
		// �ر��߳̾��
		CloseHandle(process.hThread);
	} else {
		// �����߳̾��
		*thread	= (void *) process.hThread;
	}

	// ���ؽ��̾��
	return (void *) process.hProcess;
}



/* ��ֹ���� */
void process_close (void *process, void *thread) {
	// ��������ʧ��
	if(process == NULL) {
		return;
	}

	// �ͷ��߳̾��
	CloseHandle(thread);

	// ��ֹ����
	TerminateProcess(process, 0);

	// �ȴ����̽���
	WaitForSingleObject(process, INFINITE);

	// �ͷŽ��̾��
	CloseHandle(process);
}




/* ��Ŀ������з����ڴ�ռ� */
uintptr_t process_memory (void *process) {
	// �����ڴ�
	return (uintptr_t) VirtualAllocEx(process, NULL, (SIZE_T) REMOTE_MEMORY_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

