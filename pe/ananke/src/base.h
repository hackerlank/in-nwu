/*
	$ Ananke PE Loader  (C) 2005-2012 mfboy
	$ base.h, 0.1.1027
*/

#ifndef _ANK_BASE_H_
#define _ANK_BASE_H_

#pragma warning (disable: 4996)

#include <stdio.h>
#include <stdlib.h>
#include "win32.h"
#include "process.h"
#include "gui.h"


/* ��ʾ��Ϣ */
#define error(message)				MessageBoxFormat(MB_OK | MB_ICONERROR, "%s", message)
#define error_fmt(format, ...)		MessageBoxFormat(MB_OK | MB_ICONERROR, format, __VA_ARGS__)
#define warning(message)			MessageBoxFormat(MB_OK | MB_ICONWARNING, "%s", message)
#define warning_fmt(format, ...)	MessageBoxFormat(MB_OK | MB_ICONWARNING, format, __VA_ARGS__)
#define notify(message)				MessageBoxFormat(MB_OK | MB_ICONINFORMATION, "%s", message)
#define notify_fmt(format, ...)		MessageBoxFormat(MB_OK | MB_ICONINFORMATION, format, __VA_ARGS__)

/* ȷ�϶Ի��� */
#define confirm(message)			(MessageBoxFormat(MB_YESNO | MB_ICONQUESTION, "%s", message) == IDYES)
#define confirm_no(message)			(MessageBoxFormat(MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, "%s", message) == IDYES)
#define confirm_fmt(format, ...)	(MessageBoxFormat(MB_YESNO | MB_ICONQUESTION, format, __VA_ARGS__) == IDYES)
#define confirm_no_fmt(format, ...)	(MessageBoxFormat(MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, format, __VA_ARGS__) == IDYES)

/* �쳣�˳� */
#define die(message)				error(message); exit(EXIT_FAILURE);


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
#define th_new(name)		void thread##name(void *arg)
#define th_new_char(name)	void thread##name(char *arg)

/* �����߳� */
#define th_run(name, arg)	_beginthread((void (*)(void *)) thread##name, 0, (void *) arg)



#endif   /* _ANK_BASE_H_ */