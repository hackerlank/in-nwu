/*
	$ Tracker w64   (C) 2005-2014 MF
	$ tracker.exe/main.cpp   # 1312
*/


#include "base.h"



/* �ڴ����ʧ�ܻص����� */
static void __cdecl callback_new_exception (void) {
	// �����ʾ
	MessageBoxA (NULL, "�ڴ�ռ�����ʧ�ܣ�Ӧ�ó����޷��������С�\n�����ȷ������Ӧ�ó����Զ���ֹ��", "", MB_ICONERROR | MB_OK);

	// ǿ�ƽ�������
	exit(0);
}




/* ����� */
int CALLBACK WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// ���� new ʧ�ܻص�
	std::set_new_handler (callback_new_exception);

	// ��ʼ��������
	load::start	((const char *) lpCmdLine);

	// ���� GUI
	gui::start	(hInstance, nCmdShow);

	return 0;
}

