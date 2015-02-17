/*
	$ Sinope PE Loader  (C) 2005-2012 mfboy
	$ base.h, 0.1.1104
*/

#ifndef _SNP_BASE_H_
#define _SNP_BASE_H_

#pragma warning (disable: 4996)

#include <stdio.h>
#include <stdlib.h>


/* �ֽڿ��صĶ�д */
#define bitGet(v, offset)				((v) & (0x01 << (offset)))
#define bitSet(v, offset, condition)	v = v | ((condition) ? (0x01 << (offset)) : 0)

/* ���漯�� */
#define packNew(name, ...)				void *name[] = {__VA_ARGS__};
#define packPtr(var, id, type)			(type *) ((void **) var)[id]
#define packVar(var, id, type)			*((type *) ((void **) var)[id])


/* ��ʾ��Ϣ */
#define error(message)					messageBoxFormat(MB_OK | MB_ICONERROR, "%s", message)
#define errorFormat(format, ...)		messageBoxFormat(MB_OK | MB_ICONERROR, format, __VA_ARGS__)
#define warning(message)				messageBoxFormat(MB_OK | MB_ICONWARNING, "%s", message)
#define warningFormat(format, ...)		messageBoxFormat(MB_OK | MB_ICONWARNING, format, __VA_ARGS__)
#define notice(message)					messageBoxFormat(MB_OK | MB_ICONINFORMATION, "%s", message)
#define noticeFormat(format, ...)		messageBoxFormat(MB_OK | MB_ICONINFORMATION, format, __VA_ARGS__)

/* ȷ�϶Ի��� */
#define confirm(message)				(messageBoxFormat(MB_YESNO | MB_ICONQUESTION, "%s", message) == IDYES)
#define confirmNo(message)				(messageBoxFormat(MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, "%s", message) == IDYES)
#define confirmFormat(format, ...)		(messageBoxFormat(MB_YESNO | MB_ICONQUESTION, format, __VA_ARGS__) == IDYES)
#define confirmNoFormat(format, ...)	(messageBoxFormat(MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, format, __VA_ARGS__) == IDYES)

/* �쳣�˳� */
#define die(message)					error(message); exit(EXIT_FAILURE);


/* �����ڴ� */
#define mem_alloc(p, type, count) \
	if((p = (type *) calloc(count, sizeof(type))) == NULL) { \
		die("Ӧ�ó��������ڴ�ʧ�ܡ�"); \
	}

/* �������ڴ� */
#define mem_realloc(p, type, count) \
	if((p = (type *) realloc(p, sizeof(type) * (count))) == NULL) { \
		die("Ӧ�ó��������ڴ�ʧ�ܡ�"); \
	}

/* �ͷ��ڴ� */
#define mem_delete(p) \
	if(p != NULL) { \
		free(p); p = NULL; \
	}


/* �̺߳��� */
#define thNew(name)			void __thread##name(void *arg)
#define thNewChar(name)		void __thread##name(char *arg)

/* �����߳� */
#define thRun(name, arg)	_beginthread((void (*)(void *)) __thread##name, 0, (void *) arg)


/* ��ʽ�� MessageBox */
int messageBoxFormat(size_t type, char *format, ...);

/* ������ԱȨ�� */
BOOL uacIsAdmin();

/* ѡ���ļ��򿪻򱣴� */
char * selectFileOpen(char *ext, char *title);
char * selectFileSave(char *file, char *title);

/* ��ȡ�ַ��� */
char * readFromFile();
char * readFromBuffer(char *buffer, size_t bufsize);


/* �ַ�����Сдת�� */
void strtolower(char *source);
void strtoupper(char *source);


/* ����ڴ��Ƿ�Ϊ 0 */
BOOL isBlank(void *buffer, int size);


#endif   /* _SNP_BASE_H_ */