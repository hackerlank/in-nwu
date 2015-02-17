/*
	$ Monitor Module   (C) 2005-2014 MF
	$ router.cpp
*/


#include "header/base.h"
#include "header/error.h"
#include "header/handler.h"



/* ת������ʼ */
#define ROUTE_BEGIN()													\
	__asm {		push	offset critical							}		\
	__asm {		call	EnterCriticalSection					}


/* ת�������� */
#define ROUTE_END(handle, argc, function, size)							\
	__asm {		call	handle									}		\
	__asm {		add		esp, argc								}		\
	__asm {		test	eax, eax								}		\
	__asm {		jnz		run										}		\
	__asm {		push	offset critical							}		\
	__asm {		call	LeaveCriticalSection					}		\
	__asm {		ret		size									}		\
	run:																\
	__asm {		push	offset critical							}		\
	__asm {		call	LeaveCriticalSection					}		\
	__asm {		mov		eax, function							}		\
	__asm {		add		eax, 2									}		\
	__asm {		jmp		eax										}





/* �ٽ��� */
static	CRITICAL_SECTION	critical;



/* ������ַ */
void *	Router::address_createfilew		= NULL;
void *	Router::address_loadlibraryexw	= NULL;
void *	Router::address_createprocessw	= NULL;





/* ��ת CreateFileW */
__declspec(naked) void Router::jump_createfilew (void) {
	ROUTE_BEGIN();

	__asm {
			push	dword ptr [esp + 0x14]					; dwCreationDisposition
			push	dword ptr [esp + 0x10]					; dwShareMode
			push	dword ptr [esp + 0x10]					; dwDesiredAccess
			push	dword ptr [esp + 0x10]					; lpFileName
	}

	ROUTE_END(Router::run_createfilew, 0x10, Router::address_createfilew, 0x1C);
}


/* ��ת LoadLibraryExW */
__declspec(naked) void Router::jump_loadlibraryexw (void) {
	ROUTE_BEGIN();

	__asm {
			push	dword ptr [esp + 0x04]					; lpFileName
	}

	ROUTE_END(Router::run_loadlibraryexw, 0x04, Router::address_loadlibraryexw, 0x0C);
}


/* ��ת CreateProcessW */
__declspec(naked) void Router::jump_createprocessw (void) {
	ROUTE_BEGIN();

	__asm {
			push	dword ptr [esp + 0x08]					; lpCommandLine
			push	dword ptr [esp + 0x08]					; lpApplicationName
	}

	ROUTE_END(Router::run_createprocessw, 0x08, Router::address_createprocessw, 0x28);
}





/* ���� CreateFileW */
bool Router::run_createfilew (wchar_t *name, int access, int share, int creation) {
	// û���ļ���ֱ�ӷ���ʧ��
	if(name == NULL) {
		Console::warning("#ROUTER#CreateFileW (lpFileName: NULL)  [REJECT]\n");

		return false;
	}

	__try {
		Console::notice("#ROUTER#CreateFileW (lpFileName: \"%ls\", dwDesiredAccess: 0x%08X, dwShareMode: 0x%08X, dwCreationDisposition: 0x%08X)\n", name, access, share, creation);

		// ������
		if(Handler::send(0, "CreateFile", 4, "%ls", name, "%d", access, "%d", share, "%d", creation) == false) {
			return false;
		}
	} __except (1) {
		Console::notice("CreateFileW (lpFileName: 0x%08X)  [ACCESS VIOLENT]\n", name);
	}

	return true;
}


/* ���� LoadLibraryExW */
bool Router::run_loadlibraryexw (wchar_t *name) {
	// û���ļ���ֱ�ӷ���ʧ��
	if(name == NULL) {
		Console::warning("#ROUTER#LoadLibraryExW (lpFileName: NULL)  [REJECT]\n");

		return false;
	}

	__try {
		Console::notice("#ROUTER#LoadLibraryExW (lpFileName: \"%ls\")\n", name);

		// ������
		if(Handler::send(1, "LoadLibrary", 1, "%ls", name) == false) {
			return false;
		}
	} __except (1) {
		Console::notice("LoadLibraryExW (lpFileName: 0x%08X)  [ACCESS VIOLENT]\n", name);
	}

	return true;
}


/* ���� CreateProcessW */
bool Router::run_createprocessw (wchar_t *application, wchar_t *command) {
	__try {
		if(application == NULL) {
			if(command == NULL) {
				// û����������ֱ�ӷ���ʧ��
				Console::warning("#ROUTER#CreateProcessW (lpApplicationName: NULL, lpCommandLine: NULL)  [REJECT]\n");

				return false;
			} else {
				Console::notice("#ROUTER#CreateProcessW (lpCommandLine: \"%ls\")\n", command);
			}
		} else {
			if(command == NULL) {
				Console::notice("#ROUTER#CreateProcessW (lpApplicationName: \"%ls\")\n", application);
			} else {
				Console::notice("#ROUTER#CreateProcessW (lpApplicationName: \"%ls\", lpCommandLine: \"%ls\")\n", application, command);
			}
		}
	} __except (1) {
		Console::notice("CreateProcessW (lpApplicationName: 0x%08X, lpCommandLine: 0x%08X)  [ACCESS VIOLENT]\n", application, command);
	}

	// ��֤���ò�����Ϊ NULL
	if(application == NULL) {
		application = L"";
	}
	if(command == NULL) {
		command = L"";
	}

	// ���ÿ��ƽű�
	if(Handler::send(2, "CreateProcess", 2, "%ls", application, "%ls", command) == false) {
		return false;
	}

	return true;
}





/* �������Ƿ�ɹ��ٳ� */
bool Router::validate (void *p) {
	// �������
	uintptr_t	entry	= (uintptr_t) p;
	// �н�����
	uintptr_t	jump	= entry - 0x05;

	// ��⺯����� (mov edi, edi)
	if(*((uint16_t *) entry) != 0xFF8B) {
		return false;
	}

	// ����н�����
	while(jump < entry) {
		uint8_t byte = *((uint8_t *) (jump ++));

		// ����Ϊ int3 �� nop
		if((byte != 0xCC) && (byte != 0x90)) {
			return false;
		}
	}

	return true;
}


/* д���ڴ� */
bool Router::write (uintptr_t address, unsigned int size, uint8_t *buffer) {
	// ��ǰȨ��
	unsigned long current;

	// ����Ȩ��
	if(VirtualProtect((LPVOID) address, (SIZE_T) size, PAGE_EXECUTE_READWRITE, (PDWORD) & current) == FALSE) {
		return false;
	}

	// ����д��ÿһ���ֽ�
	for(unsigned int i = 0; i < size; i ++) {
		*((uint8_t *) (address + i)) = buffer [i];
	}

	// �ָ�Ȩ��
	if(VirtualProtect((LPVOID) address, (SIZE_T) size, current, (PDWORD) & current) == FALSE) {
		return false;
	}

	return true;
}


/* �ٳֺ��� */
bool Router::inject (void *target, void *source) {
	// �����Ϣ
	Console::notice("#SYSTEM#Hook: 0x%08X => 0x%08X\n", source, target);

	// ��Ҫд�������
	uint8_t code [7] = {
		0xE9,							// jmp long ptr
		0x00, 0x00, 0x00, 0x00,			// jmp relative address
		0xEB, 0xF9,						// jmp short ptr -7
	};

	// ���㺯����ת��ֵ
	*((int32_t *) ((uintptr_t) code + 1)) = (int32_t) target - (int32_t) source;

	// ��֤�Ƿ���Խٳ�
	if(Router::validate(source) == false) {
		return false;
	}

	// �޸ĺ���
	if(Router::write((uintptr_t) source - 0x05, sizeof(code), code) == false) {
		return false;
	}

	// �ж��Ƿ�ٳֳɹ�
	if(Router::validate(source) == true) {
		// ����ٳ�ʧ��, ָ��û�б仯
		return false;
	} else {
		// ����ٳֳɹ�, ָ��ض������ı�
		return true;
	}
}




/* ִ�нٳ� */
int Router::install (void) {
	// ��ʼ���ٽ���
	InitializeCriticalSection(& critical);

	// �����ʾ
	Console::notice("#SYSTEM#Ready\n");

	// ���� kernelbase.dll
	HMODULE	handle	= LoadLibraryA("kernelbase.dll");

	if(handle == NULL) {
		// �������
		Console::warning("#SYSTEM#Failed to load dynamic library <kernelbase.dll>\n");
	} else {
		// ������سɹ�, �� kernelbase.dll �л�ȡ������ַ
		Router::address_createfilew		= (void *) GetProcAddress(handle, (LPCSTR) "CreateFileW");
		Router::address_loadlibraryexw	= (void *) GetProcAddress(handle, (LPCSTR) "LoadLibraryExW");
		Router::address_createprocessw	= (void *) GetProcAddress(handle, (LPCSTR) "CreateProcessW");
	}

	// ����ʧ�ܵĴ� kernel32.dll �л�ȡ������ַ
	if(Router::address_createfilew == NULL) {
		Router::address_createfilew = CreateFileW;
	}
	if(Router::address_loadlibraryexw == NULL) {
		Router::address_loadlibraryexw = LoadLibraryExW;
	}
	if(Router::address_createprocessw == NULL) {
		Router::address_createprocessw = CreateProcessW;
	}

	// �����ַ
	Console::notice("#SYSTEM#Peek: CreateFileW (...)    = 0x%08X\n", Router::address_createfilew);
	Console::notice("#SYSTEM#Peek: LoadLibraryExW (...) = 0x%08X\n", Router::address_loadlibraryexw);
	Console::notice("#SYSTEM#Peek: CreateProcessW (...) = 0x%08X\n", Router::address_createprocessw);

	// CreateFileW
	if(Router::inject(Router::jump_createfilew, Router::address_createfilew) == false) {
		return MM_ERROR_FUNCTION_CREATEFILEW;
	}

	// LoadLibraryExW
	if(Router::inject(Router::jump_loadlibraryexw, Router::address_loadlibraryexw) == false) {
		// �������̨����
		Console::error("#SYSTEM#Failed to hook LoadLibraryExW() on old version of Windows\n");
	}

	// CreateProcessW
	if(Router::inject(Router::jump_createprocessw, Router::address_createprocessw) == false) {
		return MM_ERROR_FUNCTION_CREATEPROCESSW;
	}

	// �ٳֳɹ�
	return MM_ERROR_SUCCEED;
}

