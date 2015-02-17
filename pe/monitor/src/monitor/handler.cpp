/*
	$ Monitor Module   (C) 2005-2014 MF
	$ handler.cpp
*/


#include "header/base.h"
#include "header/error.h"
#include "header/handler.h"



/* ������ʵ�� */
HandlerClass *	Handler::object	= NULL;

/* DLL ģ��ʵ�� */
void *			Handler::dll	= NULL;




/* ���봦���� */
int Handler::load (const char *path, const char *extension) {
	// ����
	int	error	= 0;

	// DLL
	if(stricmp(extension, "dll") == 0) {
		Handler::object = new handler_dll();

		error	= MM_ERROR_HANDLER_DLL;

		goto start;
	}

	// PHP
	if(stricmp(extension, "php") == 0) {
		Handler::object = new handler_php();

		error	= MM_ERROR_HANDLER_PHP;

		goto start;
	}

	// �޷�ʶ��Ŀ�����
	return MM_ERROR_HANDLER_UNKNOWN;


start:
	// ����������
	if(Handler::object->start(path) == false) {
		// ����������ʾ
		MessageBoxA(NULL, "����������ʧ�ܡ�\n�������̨����������ʾ������������Ϣ��ȡ����ԭ�򡣵��ȷ���󴰿ڽ��Զ��رա�", "", MB_ICONERROR | MB_OK);

		// �رտ�����
		Handler::object->close();

		// ���ش���
		return error;
	}

	// ���سɹ�
	return MM_ERROR_SUCCEED;
}




/* �� */
int Handler::start (const char *path) {
	// û����������
	if(path == NULL) {
		return MM_ERROR_SUCCEED;
	}

	// ��������Ϊ���ַ���
	if(strlen(path) == 0) {
		return MM_ERROR_SUCCEED;
	}

	// ������չ��
	const char *	extension	= strrchr(path, '.');

	// ���û���ҵ���չ��, �޷�ʹ�ö�Ӧ�Ĵ�����
	if(extension == NULL) {
		return MM_ERROR_HANDLER_UNKNOWN;
	}

	// ���봦����
	return Handler::load (path, extension + 1);
}


/* �ر� */
void Handler::close (void) {
	// û�д�����
	if(Handler::object == NULL) {
		return;
	}

	// ���ô������رշ���
	Handler::object->close();
}



/* �������� */
bool Handler::send (unsigned int index, const char *function, int argc, ...) {
	// û�д�����
	if(Handler::object == NULL) {
		// ֱ���������
		return true;
	}

	// ������ַ
	void *	argv	= (void *) ((uintptr_t) & argc + sizeof(uintptr_t));

	// ���ô�������������
	return Handler::object->send(index, function, argc, argv);
}

