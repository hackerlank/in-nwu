/*
	$ Monitor Loader   (C) 2005-2014 MF
	$ main.cpp
*/


#include "base.h"



/* �����ڴ�ʧ�ܻص����� */
static void __cdecl callback_new_exception (void) {
	// ������ʾ
	MessageBoxA(NULL, "�����ڴ�ʧ�ܣ�Ӧ�ó�������ֹ��", "", MB_ICONERROR | MB_OK);

	// ��������
	exit(0);
}



/* ���������ָ� */
static void command_split (const char *argument, char **run, char **handler, bool *console) {
	// ������������
	size_t	length	= strlen(argument);

	// ���û����������, �����зָ��
	if(length == 0) {
		return;
	}

	// �����������������ڴ�
	char *	command	= new char [length + 1];

	// ����������������
	strcpy(command, argument);

	// �ж��Ƿ��� @"..." ��ͷ
	if((*command == '@') && (*(command + 1) == '"')) {
		// ָ�������
		*handler	= command + 2;
		// ָ��������Ϣ
		*run		= strchr(*handler, '"');

		// �жϿ�����·���Ƿ�����
		if(*run == NULL) {
			// ������·��������
			*handler	= NULL;
		} else {
			// ��ԭ��ָ��� '"' �޸�Ϊ�ַ�������
			**run	= '\0';

			// ������һ���ַ�
			(*run) ++;

			// ���ҵ�һ���ǿո�
			while((*run != '\0') && isspace((int) **run)) {
				(*run) ++;
			}
		}
	} else {
		// ָ��������Ϣ
		*run	= command;
	}

	// �ж��Ƿ����ؿ���̨����
	if(*run != NULL) {
		if(**run == '!') {
			// ���ÿ���̨����Ϊ����״̬
			*console = false;

			// ������һ���ַ�
			(*run) ++;
		}
	}

	// �ж�������Ϣ
	if((*run != NULL) && (**run == '\0')) {
		*run = NULL;
	}

	// �жϿ�����
	if((*handler != NULL) && (**handler == '\0')) {
		*handler = NULL;
	}

	// �����û��������Ϣ��û�п�����, �ͷ�������������
	if((*run == NULL) && (*handler == NULL)) {
		delete [] command;
	}
}




/* ����� */
int CALLBACK WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// ������Ϣ
	char *	run		= NULL;
	// ������·��
	char *	handler	= NULL;
	// ��ʾ����̨����
	bool	console	= true;

	// ���� new ʧ�ܻص�����
	std::set_new_handler (callback_new_exception);

	// �ָ���������
	command_split (lpCmdLine, & run, & handler, & console);

	// ���� GUI
	return gui_start (run, handler, console, (void *) hInstance, nCmdShow);
}

