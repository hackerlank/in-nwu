/*
	$ Tracker w64   (C) 2005-2014 MF
	$ tracker.exe/base.h   # 1312
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



/* DLL �ļ��� */
#define TRACKER_DLL_FILENAME			"tracker.dll"




/* for C++ */
#include <new>

/* for C */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* for Windows API */
#include <Windows.h>





/* ������ */
class load {
	private:
		// DLL ·��
		static	char *	library;


		// ���� DLL ·��
		static	void	build_library	(void);
		// ����Ĭ��������Ϣ
		static	void	build_argument	(const char *argument);


		// ִ���߳�
		static	int		__stdcall	thread	(void *dummy);



	public:
		// ������Ϣ
		static	char *	command;
		// ������
		static	char *	controller;


		// ��ʼ����Ϣ
		static	void	start	(const char *argument);

		// ��������
		static	void	run		(void);
};




/* GUI */
class gui {
	private:
		// ��ǰ״̬
		static	HANDLE	status;

		// Ӧ�ó���ʵ��
		static	HINSTANCE	instance;

		// ���ھ��
		static	HWND	window;
		// �ؼ����
		static	HWND	component	[8];


		// ѡ���ļ�
		static	void	browse	(void);

		// ��ֹ����
		static	void	disable	(bool status);


		// ���ƴ���
		static	int		draw	(HWND hwnd);
		// ���ٴ���
		static	bool	close	(void);
		// �¼�����
		static	void	command	(WPARAM wparam);

		// ��Ϣѭ��
		static	LRESULT __stdcall	loop	(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);



	public:
		// ���� GUI
		static	void	start	(HINSTANCE instance, int show);

		// ��Ӧ�ɹ�
		static	void	response_succeed	(void);
		// ��Ӧʧ��
		static	void	response_error		(void);
		static	void	response_error		(const char *error);
};



#endif   /* HEADER */