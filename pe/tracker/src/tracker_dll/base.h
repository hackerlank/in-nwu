/*
	$ Tracker w64   (C) 2005-2014 MF
	$ tracker.dll/base.h   # 1312
*/


#ifndef _TRACKER_HEADER_
#define _TRACKER_HEADER_


#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE


#ifdef UNICODE
	#undef UNICODE
#endif

#ifdef _UNICODE
	#undef _UNICODE
#endif




/* PEB ��־λ�� */
#define OFFSET_PEB						0x0ACC
/* TEB ����λ�� */
#define OFFSET_TEB						0x0ACC


/* �����ڴ���С */
#define ALLOCATE						0x3000				// �����ڴ���С (4 pages)


/* ע���̴߳����С */
#define SIZE_THREAD_CODE				0x0100
/* ע���߳��������������С */
#define SIZE_THREAD_ARGUMENT			0x0100

/* ��̬���ӿ�·����С */
#define SIZE_LIBRARY					0x0200				// tracker.dll
/* ׷�ٿ�����·����С */
#define SIZE_CONTROLLER					0x0200

/* ��ַ���С */
#define SIZE_ADDRESS					0x0100
/* ��Ϣ���С */
#define SIZE_INFORMATION				0x0100
/* �ص����С */
#define SIZE_TABLE						0x0200

/* ��������С */
#define SIZE_HANDLER					0x1000				// ������

/* ������С */
#define SIZE_FUNCTION_JUMPER			0x0200				// WoW64 ϵͳ����ת����
#define SIZE_FUNCTION_INJECTOR			0x0200				// ע����
#define SIZE_FUNCTION_INJECTOR_LOCAL	0x0200				// ����ע����
#define SIZE_FUNCTION_INJECTOR_REMOTE	0x0200				// Զ��ע����
#define SIZE_FUNCTION_LOADER			0x0200				// �߳�������





/* for C++ */
#include <new>

/* for C */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* for Windows API */
#include <Windows.h>





/* Զ�̽��� */
class remote {
	public:
		// ��Ŀ������з����ڴ�ռ�
		static	uintptr_t	allocate	(HANDLE process, unsigned int size);

		// ��Ŀ������ڴ��ȡ����
		static	bool	read	(HANDLE process, uintptr_t address, void *buffer, unsigned int size);
		// ��Ŀ������ڴ�д������
		static	bool	write	(HANDLE process, uintptr_t address, void *buffer, unsigned int size);

		// ��Ŀ������д����߳�
		static	HANDLE	thread	(HANDLE process, uintptr_t address, uint32_t argument);
};




/* ��װ */
class install {
	private:
		// DLL ģ����
		static	HMODULE		module_ntdll;
		static	HMODULE		module_controller;


		// ��װ�ص�����
		static	bool	install_table_function (uintptr_t address, uintptr_t load, const char *system, const char *callback);

		// ӳ�亯��
		static	void	map_function_jumper				(uintptr_t base, uintptr_t information, uintptr_t table);
		static	void	map_function_injector			(uintptr_t base, uintptr_t address, uintptr_t information);
		static	void	map_function_injector_local		(uintptr_t base, uintptr_t address, uintptr_t information);
		static	void	map_function_injector_remote	(uintptr_t base, uintptr_t address, uintptr_t information);
		static	void	map_function_loader				(uintptr_t base, uintptr_t jumper);



	public:
		// ��ַ
		static	uintptr_t	base;

		// ����/��/��������ַ
		static	uintptr_t	base_library;
		static	uintptr_t	base_controller;
		static	uintptr_t	base_address;
		static	uintptr_t	base_information;
		static	uintptr_t	base_table;
		static	uintptr_t	base_handler;

		// ������ַ
		static	uintptr_t	base_function_jumper;
		static	uintptr_t	base_function_injector;
		static	uintptr_t	base_function_injector_local;
		static	uintptr_t	base_function_injector_remote;
		static	uintptr_t	base_function_loader;


		// ���û�ַ
		static	void	update		(uintptr_t base);
		// ����ģ��
		static	bool	controller	(void);

		// ������
		static	void	build_address		(void);
		static	void	build_information	(void);

		// ��װ�ص���
		static	bool	install_table		(void);
		// ��װ������
		static	void	install_handler		(void);

		// ӳ�亯��
		static	void	map_function		(void);
};




#endif   /* HEADER */