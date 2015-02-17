/*
	$ P2MP3  (C) 2005-2012 mfboy
	$ base.h, 0.1.1005
*/

#pragma once
#pragma warning (disable: 4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <Windows.h>


#ifndef _BASE_H_
#define _BASE_H_

/* ����ȫ�ֱ��� */
#ifdef _GUI_
	HWND hwnd;
	int running;
#else
	extern HWND hwnd;
	extern int running;
#endif


/* ֵ���� */
typedef unsigned char var_t;
/* Ѱַλ�ü��������� */
typedef unsigned long seek_t;
/* ��Ŀ���� */
typedef unsigned int count_t;
/* CRC32 ��� */
typedef unsigned int crc32_t;


/* ��ʾ */
#define success(m) MessageBox(hwnd, m, "", MB_OK | MB_ICONINFORMATION);
/* ���� */
#define alert(m) MessageBox(hwnd, m, "", MB_OK | MB_ICONEXCLAMATION);
/* ȷ�� */
#define confirm(m) (MessageBox(hwnd, m, "", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
/* �쳣�˳� */
#define die(m) MessageBox(hwnd, m, "", MB_OK | MB_ICONERROR); exit(EXIT_FAILURE);

/* �����ڴ� */
#define m_new(p, type, count) \
	if((p = (type *) calloc(count, sizeof(type))) == NULL) { \
		die("Ӧ�ó����������ڴ�ʧ�ܣ��ѱ�����ֹ���С�"); \
	}
/* ���������ڴ� */
#define m_renew(p, type, count) \
	if((p = (type *) realloc(p, sizeof(type) * (count))) == NULL) { \
		die("Ӧ�ó����������ڴ�ʧ�ܣ��ѱ�����ֹ���С�"); \
	}
/* �ͷ��ڴ� */
#define m_del(p) \
	if(p != NULL) { \
		free(p); \
		p = NULL; \
	}
/* �ڴ����� */
#define m_zero(p, size) memset(p, 0, size)
/* �����ڴ� */
#define m_copy(dst, src, size) memcpy(dst, src, size)
/* �Ƚ��ڴ� */
#define m_compare(dst, src, size) memcmp(dst, src, size)
/* Ϊ�ַ��������ڴ� */
#define ms_new(p, length) m_new(p, char, (length) + 1)
/* Ϊ�ַ������������ڴ� */
#define ms_renew(p, length) m_renew(p, char, (length) + 1)
/* �ڴ�����ַ����Ƚ� */
#define ms_compare(buf, str) m_compare(buf, str, strlen(str))

/* �����߳� */
#define t_start(func, arg) _beginthread(thread_##func, 0, arg)
/* ע���߳� */
#define t_register(func) void thread_##func(void *arg)


#endif
