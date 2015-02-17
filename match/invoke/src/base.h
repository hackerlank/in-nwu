/*
	$ Invoke   (C) 2005-2012 MF
	$ base.h, 0.1.1124
*/

#ifndef _INVOKE_BASE_H_
#define _INVOKE_BASE_H_

#pragma warning (disable: 4996)

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <Windows.h>
#include "dynmfunc.h"
#include "europa.h"


#if defined(__cplusplus)
	extern "C" {
#endif

#if defined(_INVOKE_MAIN_)
	#define _INVOKE_BASE_EXTERN_
#else
	#define _INVOKE_BASE_EXTERN_ extern
#endif


/* �ṹ�� */
struct hijack_s;
struct function_s;
struct module_s;


/* ���ù淶 */
typedef enum {
	CL_CDECL, CL_STDCALL, CL_FASTCALL
} call_e;

/* ���� */
typedef enum {
	VOID_0,
	SG_CHAR, SG_INT_16, SG_INT_32, SG_INT_64,
	US_CHAR, US_INT_16, US_INT_32, US_INT_64,
	WDCHAR, BYTE_8, REAL_32, REAL_64
} type_e;


/* ���Ͷ��� */
typedef struct type_s {
	/* ���� */
	type_e		type;
	/* ָ�뼶�� */
	size_t		pointer;
	/* �������� */
	size_t		arrindex;

	/* ��С */
	size_t		size;
} type_s;


/* ���ò��� */
typedef struct argument_s {
	char	*name;
	type_s	type;

	struct argument_s *next;
} argument_s;

/* �ٳֱ� */
typedef struct hijack_s {
	/* ���ù淶 */
	call_e	invoke;

	/* �������� */
	type_s	ret;

	/* ���� */
	size_t	argCount;
	argument_s *argList;

	/* ����ѡ�� */
	BOOL	vararg;
	BOOL	esp;

	struct function_s	*function;

	struct hijack_s		*prev;
	struct hijack_s		*next;
} hijack_s;


/* ������ */
typedef struct function_s {
	/* ���ƻ���� */
	char	*name;
	DWORD	ordinal;

	/* ��ڵ�ַ */
	DWORD	entry;
	/* �ϵ��ַ */
	DWORD	bp;

	/* @ hook.c  (v1) */
	/*
		// ԭʼָ���ַ
		DWORD	code;
		// ԭʼָ��
		int		codeSize;
		char	codeData[32];
	*/

	/* �Ѿ� hook */
	BOOL	hook;

	/* ԭʼָ����� */
	unsigned char code;

	/* �ٳֱ� */
	struct hijack_s		*hijack;

	struct module_s		*module;
	struct function_s	*next;
} function_s;

/* ģ��� */
typedef struct module_s {
	/* ��ַ */
	DWORD	base;
	/* ��С */
	DWORD	size;

	/* ģ���� */
	char	*modname;
	/* ģ���ļ�·�� */
	char	*filename;

	/* ���������� */
	function_s *function;

	struct module_s *next;
} module_s;


/* ������Ϣ */
typedef struct {
	/* ��ǰ���� */
	HINSTANCE	instance;
	HANDLE		process;

	/* �������߳� */
	HANDLE		debug;
	/* �Ƿ񸽼ӱ�Ľ��� */
	BOOL		attach;
	/* �˳�ʱ����ϵ� */
	BOOL		uninstall;

	/* �����ļ�����ģ�� */
	char		*file;
	char		*module;
	/* CUI */
	int			console;
	/* ����� */
	DWORD		entry;
	/* ������Ϣ */
	DWORD		pid;
	HANDLE		handle;
	HANDLE		snapshot;
	/* ��ģ�� */
	DWORD		mainStart;
	DWORD		mainEnd;
	/* ��ǰ������Ϣ */
	HANDLE		thread;
	function_s	*function;
} runtime_s;


/* �������� */
_INVOKE_BASE_EXTERN_ hijack_s		*HIJ;
_INVOKE_BASE_EXTERN_ module_s		*MOD;

/* ������Ϣ */
_INVOKE_BASE_EXTERN_ runtime_s		RT;



/* ���漯�� */
#define packNew(name, ...)				void *name[] = {__VA_ARGS__};
#define packPtr(var, id, type)			((type *) ((void **) var)[id])
#define packVar(var, id, type)			(*((type *) ((void **) var)[id]))


/* ��ʾ��Ϣ */
#define error(win, message)					messageBoxFormat(win, MB_OK | MB_ICONERROR, "%s", message)
#define errorFormat(win, format, ...)		messageBoxFormat(win, MB_OK | MB_ICONERROR, format, __VA_ARGS__)
#define warning(win, message)				messageBoxFormat(win, MB_OK | MB_ICONWARNING, "%s", message)
#define warningFormat(win, format, ...)		messageBoxFormat(win, MB_OK | MB_ICONWARNING, format, __VA_ARGS__)
#define notice(win, message)				messageBoxFormat(win, MB_OK | MB_ICONINFORMATION, "%s", message)
#define noticeFormat(win, format, ...)		messageBoxFormat(win, MB_OK | MB_ICONINFORMATION, format, __VA_ARGS__)

/* ȷ�϶Ի��� */
#define confirm(win, message)				(messageBoxFormat(win, MB_YESNO | MB_ICONQUESTION, "%s", message) == IDYES)
#define confirmNo(win, message)				(messageBoxFormat(win, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, "%s", message) == IDYES)
#define confirmFormat(win, format, ...)		(messageBoxFormat(win, MB_YESNO | MB_ICONQUESTION, format, __VA_ARGS__) == IDYES)
#define confirmNoFormat(win, format, ...)	(messageBoxFormat(win, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, format, __VA_ARGS__) == IDYES)


/* �����ڴ� */
#define NEW(p, type, count) \
	if((p = (type *) calloc(count, sizeof(type))) == NULL) { \
		die("Ӧ�ó��������ڴ�ʧ�ܡ�"); \
	}

/* �������ڴ� */
#define RENEW(p, type, count) \
	if((p = (type *) realloc(p, sizeof(type) * (count))) == NULL) { \
		die("Ӧ�ó��������ڴ�ʧ�ܡ�"); \
	}

/* �ͷ��ڴ� */
#define DEL(p) \
	if(p != NULL) { \
		free(p); p = NULL; \
	}


/* �����߳� */
#define threadRun(name, arg)	_beginthread((void (*)(void *)) threadFunc##name, 0, (void *) arg)
/* �����߳� */
#define threadNewVoid(name)		void threadFunc##name(void *arg)
#define threadNewChar(name)		void threadFunc##name(char *arg)



/* �����˳� */
void die(char *message);

/* �ź���� */
void symbolInit();
int symbolRun(int arg);
void symbolRunAsyn(int arg);
int symbolWait();
void symbolReturn(int data);

/* ������ԱȨ�� */
int uacAdminCheck();

/* ��ʽ�� MessageBox */
int messageBoxFormat(HWND win, unsigned int type, char *format, ...);

/* ����� */
void activeWindow(HWND win);

/* ѡ���ļ��򿪻򱣴� */
char * selectFileOpen(HWND win, char *ext, char *title);
char * selectFileOpenByFilter(HWND win, char *filter, char *title);
char * selectFileSave(HWND win, char *filename, char *title);
char * selectFileSaveByFilter(HWND win, char *filename, char *ext, char *filter, char *title);

/* DOS ·��ת�� */
void device2NtLetter(char *path);

/* �ַ������� */
int match(char *str, char *sub, int vague, int sencase);
int strEqual(char *str1, char *str2);
char * wchar2char(wchar_t *str);


/* �߳���� */
void threadCreate(DWORD threadid, HANDLE handle);
void threadExit(DWORD threadid);
HANDLE threadGet(DWORD threadid);
function_s * threadFunctionGet(DWORD threadid);
void threadFunctionSet(DWORD threadid, function_s *func);
DWORD threadRetGet(DWORD threadid, BOOL *retBk);
void threadRetSet(DWORD threadid, DWORD ret, BOOL retBk);
void threadSuspend(DWORD threadid);
void threadResume(DWORD threadid);

/* ������� */
int processList(DWORD **pid, char ***name);
char * processSelect(int index);
char * processFile();
void processFree();
int processRead(DWORD addr, void *buffer, unsigned int size);
int processWrite(DWORD addr, void *buffer, unsigned int size);

/* �����ڴ� */
void memoryRewind();
int memorySeek(int offset, int mode);
int memoryTell();
int memoryAvail(int offset);
int memoryRead(void *buffer, unsigned int size);
char * memoryReadString(unsigned int max, int display);
int memoryWrite(void *buffer, unsigned int size);

/* ģ�� */
void moduleShow();
void moduleSet(module_s *module, DWORD offset, BOOL moveStack, BOOL moveCall, BOOL moveJump, BOOL moveRet);
function_s * moduleFind(HWND win, char *module, char *function, unsigned long int ordinal, int *mode);
void moduleDelete(DWORD base);
module_s * moduleLoad(HANDLE handle, DWORD base);
void moduleDump();

/* HOOK */
void hookSetCode(function_s *func);
void hookSetBp(function_s *func);
int hookInstall(function_s *func);
void hookInstallAll(HWND win);
int hookUninstall(function_s *func);
void hookUninstallAll();
function_s * hookFind(DWORD address, module_s **module);

/* ���ݷ�װ */
void packageCreate(HWND win);
int packageLoad(HWND win, char *file, BOOL silent);
void packageAuto();

/* ������� */
char * typeReadSimple(DWORD addr, type_s *type);
char * typeReadDetail(DWORD addr, type_s *type, char **pointer);
char * type2string(type_s *type);
void typeSet(type_s *p, type_e type, size_t pointer, size_t arrindex);
void typeComboInit(erp_object_combo_s *obj, type_s *type, BOOL allowVoid);
int typeComboChange(int index, erp_object_combo_s *obj, type_s *type, BOOL allowVoid);
char * typeArgument(argument_s *arg, BOOL vararg);


/* ��ͣ�ָ� */
void runSuspendResume();

/* �������߳� */
threadNewVoid(runDebug);



/* ntdll.dll */
FUNCTION(processSuspend,	long, NTAPI, HANDLE process);
FUNCTION(processResume,		long, NTAPI, HANDLE process);
/* disasm.dll */
FUNCTION(disasm, int, __cdecl, unsigned int address, char *buffer, unsigned int bufSize, char *output, unsigned int outSize);


#if defined(__cplusplus)
	}
#endif

#endif   /* _INVOKE_BASE_H_ */