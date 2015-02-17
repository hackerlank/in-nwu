/*
	$ Monitor Module   (C) 2005-2014 MF
	$ handler/dll.cpp
*/


#include "../header/base.h"
#include "../header/handler.h"



/* ���غ��� */
bool handler_dll::load (unsigned int index, const char *name) {
	// ��ȡ������ַ
	uintptr_t	address	= (uintptr_t) GetProcAddress((HMODULE) this->handle, (LPCSTR) name);

	if(address == 0) {
		// �������
		Console::warning("#HANDLER#Callback %hs => NOT FOUND\n", name);

		// ����ʧ��������Ч��ַ
		this->pointer [index]	= 0xFFFFFFFF;

		return false;
	} else {
		// �������
		Console::notice("#HANDLER#Callback %hs => 0x%08X\n", name, address);

		// �����ַ
		this->pointer [index]	= address;

		return true;
	}
}




/* ���� */
bool handler_dll::start (const char *path) {
	// ��ʼ��������ַ
	for(int i = 0; i < MM_HANDLER_MAX; i ++) {
		this->pointer [i] = 0;
	}

	// ���ؾ��
	this->handle	= (void *) LoadLibraryA(path);

	// �ж��Ƿ���سɹ�
	if(this->handle == NULL) {
		Console::error("#HANDLER#Failed to load dynamic library: %s\n", path);

		return false;
	}

	return true;
}


/* �ر� */
void handler_dll::close (void) {
	// ж��ģ��
	FreeLibrary((HMODULE) this->handle);
}


/* �������� */
bool handler_dll::send (unsigned int index, const char *function, int argc, void *argv) {
	// �жϵ�ַ�Ƿ��ѱ�����
	if(this->pointer[index] == 0) {
		// ���Լ���
		if(this->load(index, function) == false) {
			// ����������ֱ�ӷ�������
			return true;
		}
	}

	// �жϵ�ַ�Ƿ�Ϊδ�����ַ
	if(this->pointer[index] == 0xFFFFFFFF) {
		return true;
	}

	// ������ַ
	uintptr_t	address	= this->pointer [index];

	// ���������ؽ��
	int			result	= 0;

	__try {
		__asm {
				mov		ecx,	argc

				mov		ebx,	ecx
				dec		ebx
				shl		ebx,	0x03

				add		ebx,	argv
				add		ebx,	0x04

			next:
				push	dword ptr [ebx]

				sub		ebx,	0x08

				loop	next

			
				mov		eax,	address
				call	eax

				mov		result,	eax

				mov		ecx,	argc
				shl		ecx,	0x02

				add		esp,	ecx
		}
	} __except (1) {
		// ���δ�����쳣��Ϣ
		Console::warning("#HANDLER#Unhandled exception at callback %s   [ACCEPT]\n", function);

		// ����Ĭ�Ϸ�������
		result	= 1;
	}

	// ����ܾ�
	if(result == 0) {
		Console::warning("#HANDLER#%s handled   [REJECT]\n", function);
	}

	return (bool) result;
}

