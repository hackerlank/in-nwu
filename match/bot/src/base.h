/*
	$ Robot for LinLink   (C) 2005-2013 MF
	$ base.h
*/

#ifndef _BASE_H_
#define _BASE_H_

/* for _beginthread */
#include <process.h>
/* for all */
#include <Windows.h>

#if defined(__cplusplus)
	extern "C" {
#endif


/* �ձ��� */
#define T_NULL	TEXT("")


/* ���д��� */
#define DIE(message)													\
	MessageBox(hWin, TEXT(message),	T_NULL, MB_OK | MB_ICONERROR);		\
	ExitProcess(EXIT_FAILURE);

/* �ڴ�������ͷ� */
#define NEW(p, type, count)												\
	if((p = (type *) calloc(count, sizeof(type))) == NULL) {			\
		DIE("���������ڴ�ʱ�������󣬱�����ֹ��");								\
	}
#define DEL(p)															\
	if(p) {																\
		free(p); p = NULL;												\
	}

/* �źŷ��͡��ȴ�����λ */
#define SEND(name)														\
	if(SetEvent(symbol##name) == FALSE) {								\
		DIE("�������߳�ͬ���ź�ʱ�������󣬱�����ֹ��");						\
	}
#define WAIT(name)														\
	if(WaitForSingleObject(symbol##name, INFINITE) != WAIT_OBJECT_0) {	\
		DIE("����ȴ��߳�ͬ���ź�ʱ�������󣬱�����ֹ��");						\
	}																	\
	RESET(name)
#define TIMEOUT(val, name, millisec)									\
	val = WaitForSingleObject(symbol##name, millisec);					\
	RESET(name);														\
	if((val != WAIT_TIMEOUT) && (val != WAIT_OBJECT_0)) {				\
		DIE("����ȴ��ӳ����߳�ͬ���ź�ʱ�������󣬱�����ֹ��");					\
	} else if(val == WAIT_OBJECT_0) 
#define RESET(name)														\
	ResetEvent(symbol##name)

/* �̶߳��� */
#define THREAD(name)			void name (void *dummy)
/* �߳����� */
#define THRDRUN(name)			((HANDLE) _beginthread((void (__cdecl *)(void *)) name, 0, NULL))

/* ��Ϣ��ʾ */
#define NOTICE(message)			 MessageBox(hWin, TEXT(message), T_NULL, MB_OK | MB_ICONINFORMATION)
#define WARNING(message)		 MessageBox(hWin, TEXT(message), T_NULL, MB_OK | MB_ICONWARNING)


/* ����ʵ�� */
extern HINSTANCE	instance;
/* �����ھ�� */
extern HWND			hWin;
/* ���󴰿ںͽ��� */
extern HWND			form;
extern HANDLE		process;

/* ��ͣ�ź� */
extern HANDLE		symbolRun, symbolStop;
/* ��ֹ�ź� */
extern HANDLE		symbolExit, symbolDestroy;


/* �߳� */
THREAD(gui);
THREAD(debug);
THREAD(mouse);

/* gui ��� */
void buttonLoad(int enable);
void buttonRun(int enable);

/* ������� */
int procMount();
void procAddress(size_t address, CONTEXT *reg);
void procRestore();
int procClose();
void procRun();

/* �ڴ��д */
int memoryRead(DWORD address, size_t size, void *buffer);
int memoryWrite(DWORD address, size_t size, void *buffer);
int memoryWriteV(DWORD address, size_t size, ...);
int memorySet(DWORD address, size_t size, int data);
int memoryEqual(DWORD address, size_t size, void *buffer);
int memoryEqualV(DWORD address, size_t size, ...);


#if defined(__cplusplus)
	}
#endif

#endif   /* _BASE_H_ */