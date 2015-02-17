/*
	$ Dynamic DLL Function Loader   (C) 2005-2012 mfboy
	$ dynmfunc.c, 0.1.1117
*/

#pragma warning (disable: 4996)

#include "base.h"


/* kernel32 */
static void *funcKernel32LoadLibrary	= NULL;
static void *funcKernel32GetProcAddress	= NULL;

/* �쳣���� */
static void *handleException = NULL;

/* ������ */
static void *callerAddr	= NULL;
static void *callerPage	= NULL;
static unsigned short callerSize	= 0;
static unsigned short callerUnused	= 0;


/* �쳣Ĭ�ϴ����� */
static void functionException(int type, ...) {
	/* ���������� */
	char	*p, *dll, *function;
	/* ������Ϣ */
	char	buffer[512] = {0};

	/* MessageBox */
	int (WINAPI *msgbox)(HWND, LPCTSTR, LPCTSTR, UINT) = NULL;

	/* ��� */
	va_list	arg;
	va_start(arg, type);

	switch(type) {
		case 0:
			dll			= va_arg(arg, char *);
			function	= va_arg(arg, char *);

			break;

		case 1:
			p			= va_arg(arg, char *);
			dll			= *((char **) (p + sizeof(void *) * 1));
			function	= *((char **) (p + sizeof(void *) * 2));

			break;

		default:
			exit(EXIT_FAILURE);
	}

	va_end(arg);

	/* ���ɴ�����Ϣ */
	sprintf(buffer, "�޷��� %s ���ҵ� %s ����ڣ�Ӧ�ó����ѱ�����ֹ��", dll, function);

	die(buffer);
}


static void functionInitHandle() {
	if(funcKernel32LoadLibrary == NULL) {
		funcKernel32LoadLibrary	= &LoadLibrary;
	}
	if(funcKernel32GetProcAddress == NULL) {
		funcKernel32GetProcAddress = &GetProcAddress;
	}

	if(handleException == NULL)  {
		handleException = &functionException;
	}
}

static void functionInitCaller() {
	if(callerAddr != NULL) {
		return;
	}

	__asm {
			mov		callerAddr, offset callerStart
			mov		eax, offset callerEnd
			sub		eax, callerAddr
			mov		callerSize, ax
	}

	return;

callerStart:
	__asm {
					; ����ԭ EBX
			push	ebx

			push	0
		entry:
			pop		ebx
			cmp		ebx, 0
			jnz		main
			call	entry

		main:
			sub		ebx, 26
			push	ebx

					; ���� LoadLibrary
			push	DWORD PTR [ebx+4]
			mov		ebx, funcKernel32LoadLibrary
			call	ebx

					; ���� GetProcAddress
			mov		ebx, [esp]
			push	DWORD PTR [ebx+8]
			push	eax
			mov		ebx, funcKernel32GetProcAddress
			call	ebx

					; ��ⷵ��ֵ
			cmp		eax, 0
			je		exception

					; ���Ǽ�����
			pop		ebx
			add		ebx, 12
			mov		BYTE PTR	[ebx],		0xB8		; mov eax, ?
			mov		DWORD PTR	[ebx+1],	eax
			mov		WORD PTR	[ebx+5],	0xE0FF		; jmp eax

					; ������ת
			pop		ebx
			jmp		eax

		exception:
			push	1
			mov		ebx, handleException
			call	ebx
	}
callerEnd:

	return;
}


static int functionPasstive(void *p, char *dll, char *function) {
	char *caller;
	unsigned int size, offset;

	/* ��������С */
	size = sizeof(void *) * 3 + callerSize;

	/* �������ռ� */
	if(callerUnused < size) {
		if((callerPage = VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)) == NULL) {
			exit(EXIT_FAILURE);
		}

		callerUnused = 4096;
	}

	/* ����ָ�� */
	caller = (char *) callerPage;
	*((void **) p) = caller + sizeof(void *) * 3;

	/* ���ٿռ� */
	callerPage		= (char *) callerPage + size;
	callerUnused	= callerUnused - size;

	/* ʣ��ռ䰴 32 �ֽڶ��� */
	if(offset = callerUnused - (callerUnused / 32 * 32)) {
		callerPage		= (char *) callerPage + offset;
		callerUnused	= callerUnused - offset;
	}

	/* д���������Ϣ */
	memcpy(caller + sizeof(void *) * 0, caller, sizeof(void *));
	memcpy(caller + sizeof(void *) * 1, &dll, sizeof(void *));
	memcpy(caller + sizeof(void *) * 2, &function, sizeof(void *));

	/* д������� */
	memcpy(caller + sizeof(void *) * 3, callerAddr, callerSize);

	return -1;
}

static int functionInitiative(void *p, char *dll, char *function) {
	HMODULE module;

	if((module = LoadLibrary(dll)) == NULL) {
		return 0;
	}
	if((*((void **) p) = GetProcAddress(module, function)) == NULL) {
		return 0;
	}

	return 1;
}


int Function(void *p, char *dll, char *function, int flag) {
	int result;

	/* ��ʼ�� */
	functionInitHandle();
	functionInitCaller();

	/* �������� */
	if(flag & FUNCTION_PASSIVE) {
		return functionPasstive(p, dll, function);
	}

	/* �������� */
	result = functionInitiative(p, dll, function);

	if(((flag & FUNCTION_NOT_FATAL) == 0) && (result == 0)) {
		((void (*)(int, char *, char *)) handleException)(0, dll, function);
	}

	return result;
}
