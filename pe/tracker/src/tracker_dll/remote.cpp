/*
	$ Tracker w64   (C) 2005-2014 MF
	$ tracker.dll/remote.cpp   # 1312
*/


#include "base.h"



/* ��Ŀ������з����ڴ�ռ� */
uintptr_t remote::allocate (HANDLE process, unsigned int size) {
	return (uintptr_t) VirtualAllocEx (process, NULL, (SIZE_T) size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}




/* ��Ŀ������ڴ��ȡ���� */
bool remote::read (HANDLE process, uintptr_t address, void *buffer, unsigned int size) {
	// ��ȡ���ڴ��С
	unsigned int read;

	// ��ȡ�ڴ�
	if(ReadProcessMemory (process, (LPCVOID) address, (LPVOID) buffer, (SIZE_T) size, (SIZE_T *) & read) == FALSE) {
		return false;
	}

	// �ж��Ƿ���ȫ��ȡ
	if(size != read) {
		return false;
	}

	return true;
}


/* ��Ŀ������ڴ�д������ */
bool remote::write (HANDLE process, uintptr_t address, void *buffer, unsigned int size) {
	// д����ڴ��С
	unsigned int written;

	// д�뵽�����ڴ�
	if(WriteProcessMemory (process, (LPVOID) address, (LPCVOID) buffer, size, (SIZE_T *) & written) == FALSE) {
		return false;
	}

	// �ж��Ƿ���ȫд��
	if(size != written) {
		return false;
	}

	return true;
}



/* ��Ŀ������д����߳� */
HANDLE remote::thread (HANDLE process, uintptr_t address, uint32_t argument) {
	return CreateRemoteThread (
		/* hProcess */				(HANDLE) process,
		/* lpThreadAttributes */	NULL,
		/* dwStackSize */			0,
		/* lpStartAddress */		(LPTHREAD_START_ROUTINE) address,
		/* lpParameter */			(LPVOID) argument,
		/* dwCreationFlags */		0,
		/* lpThreadId */			NULL
	);
}

