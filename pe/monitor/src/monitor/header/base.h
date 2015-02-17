/*
	$ Monitor Module   (C) 2005-2014 MF
	$ base.h
*/


#ifndef _MODULE_BASE_H_
#define _MODULE_BASE_H_


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
#include <time.h>
#include <Windows.h>




/* ����̨ */
class Console {
	private:
		static	void *	handle;


	public:
		/* ��������̨���� */
		static	bool	open	(void);

		/* ������ɫ */
		static	void	color	(bool red, bool green, bool blue, bool intensity);

		/* ��ʽ����� */
		static	void	printf	(const char *format, ...);
		/* ��̬������ʽ����� */
		static	void	vprintf	(const char *format, void *argument);

		/* ִ����� */
		static	void	output	(void *p);

		/* �����ɫ��ʾ */
		static	void	notice	(const char *format, ...);
		/* �����ɫ���� */
		static	void	warning	(const char *format, ...);
		/* �����ɫ���� */
		static	void	error	(const char *format, ...);
};




/* ת���� */
class Router {
	private:
		/* ǿ�Ʒ���ֵ */
		static	uintptr_t	value;


		/* ������� */
		static	void *	address_createfilew;
		static	void *	address_loadlibraryexw;
		static	void *	address_getprocaddress;
		static	void *	address_createprocessw;
		static	void *	address_shellexecuteexw;
		static	void *	address_winexec;


		/* ��ת�� */
		static	void	jump_createfilew		(void);
		static	void	jump_loadlibraryexw		(void);
		static	void	jump_getprocaddress		(void);
		static	void	jump_createprocessw		(void);
		static	void	jump_shellexecuteexw	(void);
		static	void	jump_winexec			(void);


		/* ������ */
		static	bool	run_createfilew		(wchar_t *, uint32_t, uint32_t, void *, uint32_t, uint32_t, void *);
		static	bool	run_loadlibraryexw	(wchar_t *, void *, uint32_t);
		static	bool	run_getprocaddress	(void *, char *);
		static	bool	run_createprocessw	(wchar_t *, wchar_t *, void *, void *, int32_t, uint32_t, void *, wchar_t *, void *, void *);
		static	bool	run_shellexecuteexw	(void *);
		static	bool	run_winexec			(char *, uint32_t);


		/* ��֤���� */
		static	bool	validate	(void *p);
		/* �ڴ�д�� */
		static	bool	write		(uintptr_t address, unsigned int size, uint8_t *buffer);
		/* ע�뺯�� */
		static	bool	inject		(void *target, void *source);


	public:
		/* ��װ�ٳִ��� */
		static	int		install		(void);
};




/* �ű������������� */
class HandlerClass {
	public:
		/* ���� */
		virtual	bool	start	(const char *path)	= 0;
		/* �ر� */
		virtual	void	close	(void)				= 0;

		/* �������� */
		virtual	bool	send	(unsigned int index, const char *function, int argc, void *argv)	= 0;
};




/* �ű������� */
class Handler {
	private:
		/* ������ʵ�� */
		static	HandlerClass *	object;

		/* ���봦���� */
		static	int		load	(const char *path, const char *extension);


	public:
		/* DLL ģ��ʵ�� */
		static	void *	dll;

		/* �� */
		static	int		start	(const char *path);
		/* �ر� */
		static	void	close	(void);

		/* �������� */
		static	bool	send	(unsigned int index, const char *function, int argc, ...);
};



#endif   /* BASE */