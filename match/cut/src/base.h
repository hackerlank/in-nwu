/*
	$ base.h, 0.2.1221
*/

#pragma warning (disable: 4996)


#ifndef _BASE_H_
#define _BASE_H_

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <process.h>
#include <Windows.h>


#if defined(__cplusplus)
	extern "C" {
#endif


/* �������� (32 bit) */
typedef unsigned int int_t;
/* ���������� */
typedef int_t * bigint_t;


/* �������� */
typedef struct number_s {
	/* ���� */
	int_t		number;

	/* �ֽ���������ָ�� */
	int_t		remainder;
	int_t		exponent;

	/* ���������� */
	bigint_t	data;
	/* �������� (DW) */
	size_t		countHex;
	size_t		countDec;

	/* ������� */
	size_t		lenAll;
	/* ���λ (10^10 ����) ���� */
	size_t		lenHigh;

	/* ����ʱ�� */
	clock_t		startMul, endMul;
	clock_t		startConv, endConv;

	/* ��������� */
	int			doneCalc;
	/* �ѽ������ */
	int			doneOutput;

	struct number_s *next;
} number_s;



/* ת��ʱʹ�õ���ת���� (10 ^ 10) */
#define NOTATION	0x3B9ACA00

/* 10 ^ 10 �������ݴ���ƫ�� */
#define OFFSET		2



/* �쳣�˳� */
#define DIE(message)													\
	MessageBox (														\
		win,															\
		TEXT(message "��Ӧ�ó����ѱ�����ֹ����"),							\
		"",																\
		MB_OK | MB_ICONERROR											\
	);																	\
	ExitProcess(EXIT_FAILURE);


/* �ڴ�������ͷ� */
#define NEW(p, type, count)												\
	if((p = (type *) calloc(count, sizeof(type))) == NULL) {			\
		DIE("�����ڴ�ʧ��");												\
	}
#define DEL(p)															\
	if(p) {																\
		free(p); p = NULL;												\
	}

/* �� 16 λ������ڴ�������ͷ� */
#define NEWA(p, type, count)											\
	if(p = (type *) _aligned_malloc(sizeof(type) * (count), 16)) {		\
		memset(p, 0, sizeof(type) * (count));							\
	} else 
#define DELA(p)															\
	if(p) {																\
		_aligned_free(p); p = NULL;										\
	}


/* Ϊ x * 3 ^ n �����㹻��С�Ĵ����ռ� */
#define BIGINT(p, n)													\
	NEWA(p, int_t, (((n + 1) << 2) + 4) << 2)


/* �����߳� */
#define THRD_NEW(name)													\
	void __cdecl name (int_t arg)

/* �����߳� */
#define THRD_RUN(name, arg)												\
	_beginthread((void (__cdecl *)(void *)) name, 0, (void *) (arg))


/* ��Ϣ���ѶԻ��� */
#define ALERT(message)		MessageBox(win, TEXT(message), TEXT(""), MB_OK | MB_ICONWARNING)
#define NOTICE(message)		MessageBox(win, TEXT(message), TEXT(""), MB_OK | MB_ICONINFORMATION)

/* ȷ�϶Ի��� */
#define CONFIRM(message)	(MessageBox(win, TEXT(message), TEXT(""), MB_YESNO | MB_ICONQUESTION) == IDYES)



/* �������� */
extern number_s		*NUM;

/* ���ھ����ʵ�� */
extern HWND			 win;
extern HINSTANCE	 instance;



/* ͼ���û����� */
void gui();

/* ��ť���� */
void openEnable(int enable);
void saveEnable(int enable);
void calcEnable(int enable);
void calcText(char *text);

/* ��ȡ���λ�� */
size_t textRead();

/* ʹ�� SSE ѡ�� */
int sseChecked();
void sseEnable(int enable);

/* �б���� */
void listAppend(number_s *number);
void listSet(int_t index, int_t column, char *format, ...);
void listClear();


/* ��ȡ���� */
void input();
/* ������ */
void output();

/* �������� */
void dispatch();
/* �ر�ȷ�� */
int close();


/* ��� CPU �Ƿ�֧�� SSE2 */
int cpuSSE();


/* �˷����� */
size_t powWithALU(bigint_t data, int_t ratio, int_t exp);
size_t powWithSSE(bigint_t data, int_t ratio, int_t exp);

/* ����ת�� */
size_t convert(bigint_t data, size_t count);


#if defined(__cplusplus)
	}
#endif

#endif   /* _BASE_H_ */