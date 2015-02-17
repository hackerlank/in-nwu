/*
	$ Monitor Loader   (C) 2005-2014 MF
	$ base.h
*/


#ifndef _LOADER_BASE_H_
#define _LOADER_BASE_H_


#pragma warning (disable: 4102)
#pragma warning (disable: 4800)
#pragma warning (disable: 4996)


#ifdef UNICODE
	#undef UNICODE
#endif

#ifdef _UNICODE
	#undef _UNICODE
#endif



#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <Windows.h>
#include <Commctrl.h>

#include "error.h"




/* ��Ҫ��Ŀ������з�����ڴ�ռ� */
#define REMOTE_MEMORY_SIZE			0x1000

/* ע������С */
#define REMOTE_MEMORY_CODE			0x100

/* �����ռ��С */
#define REMOTE_MEMORY_CONSTANT		0x400

/* ������·����С */
#define REMOTE_MEMORY_HANDLER		0x200




#ifdef __cplusplus
	extern "C" {
#endif




/* process.c */
void *		process_start	(const char *command, void **thread);
void		process_close	(void *process, void *thread);
uintptr_t	process_memory	(void *process);


/* remote.c */
bool	remote_write	(void *process, uintptr_t address, void *buffer, unsigned int size);
bool	remote_inject	(void *process, uintptr_t address, const char *dll, const char *function);
void *	remote_thread	(void *process, uintptr_t address, uintptr_t argument);


/* call.c */
void	call	(const char *run, const char *handler, bool console);


/* gui.c */
int		gui_start		(const char *run, const char *handler, bool console, void *instance, int show);
void	gui_response	(const char *error);




#ifdef __cplusplus
	}
#endif

#endif   /* BASE */