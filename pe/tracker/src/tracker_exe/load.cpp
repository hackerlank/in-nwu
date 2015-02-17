/*
	$ Tracker w64   (C) 2005-2014 MF
	$ tracker.exe/main.cpp   # 1312
*/


#include "base.h"



/* DLL ·�� */
char *	load::library		= NULL;

/* ������Ϣ */
char *	load::command		= NULL;
/* ������ */
char *	load::controller	= NULL;





/* ���� DLL ·�� */
void load::build_library (void) {
	// ���仺����
	load::library	= new char [MAX_PATH + 16];

	// ��ʼ��������
	memset (load::library, 0, MAX_PATH + 16);

	// ��ȡ EXE ·��
	GetModuleFileNameA (NULL, load::library, MAX_PATH);

	// �������� "\" ���ҵ�Ŀ¼����
	char *	end	= strrchr (load::library, '\\');

	// ����ܲ�ֳ�Ŀ¼����, ��Ŀ¼���ֽ�β�����ַ���
	if(end != NULL) {
		* (end + 1) = '\0';
	}

	// �����ļ���
	strcat (load::library, TRACKER_DLL_FILENAME);
}



/* ����Ĭ��������Ϣ */
void load::build_argument (const char *argument) {
	// ����Ĭ�ϲ���
	load::command		= NULL;
	load::controller	= NULL;

	// ������������
	unsigned int	length	= (unsigned int) strlen(argument);

	// �ж����������Ƿ�Ϊ��
	if(length == 0) {
		return;
	}

	// �����������������ڴ�
	char *	copy	= new char [length + 1];

	// ����������������
	memcpy (copy, argument, length + 1);

	// ������������Ƿ��� @"*" ��ͷ
	if((*copy == '@') && (*(copy + 1) == '"')) {
		// ������Ϊ @"*" �е�����
		load::controller	= copy + 2;
		// ������Ϣָ�����������
		load::command		= strchr (load::controller, '"');

		// ���û���ҵ���������������
		if(load::command == NULL) {
			// ��������Ϣ������
			load::controller	= NULL;
		} else {
			// ��ԭ�е������޸�Ϊ \0 ��Ϊ�������ַ���������־
			* load::command		= '\0';

			// ��������Ϣ����ָ��ĵڶ����ַ���
			load::command ++;
		}
	} else {
		// ��������������Ϊ������Ϣ
		load::command	= copy;
	}

	// �жϿ�������Ϣ
	if(load::command != NULL) {
		// ����������Ϣ��Ŀո�
		while(isspace (* load::command) != 0) {
			load::command ++;
		}

		// �ж�������Ϣ�Ƿ�Ϊ���ַ���
		if(* load::command == '\0') {
			load::command = NULL;
		}
	}

	// �жϿտ�����
	if((load::controller != NULL) && (* load::controller == '\0')) {
		load::controller = NULL;
	}

	// ���������Ϣ�Ϳ�������Ϊ��, �ͷ�������������
	if((load::command == NULL) && (load::controller == NULL)) {
		delete [] copy;
	}
}





/* ִ���߳� */
int __stdcall load::thread (void *dummy) {
	// ���붯̬���ӿ�
	HMODULE		module		= LoadLibraryA (load::library);
	// ��ȡԶ��ע�뺯����ַ
	uintptr_t	function	= (uintptr_t) GetProcAddress (module, "tracker_remote");

	// �ж��Ƿ��ȡ�ɹ�
	if(function == 0) {
		gui::response_error ("���ض�̬���ӿ� tracker.dll ʧ�ܡ�");

		goto end;
	}

	// ������Ϣ
	STARTUPINFOA		startup;
	// ������Ϣ
	PROCESS_INFORMATION	process;
	
	// �������������Ϣ
	ZeroMemory(& startup,	sizeof(startup));
	ZeroMemory(& process,	sizeof(process));

	// ��ʼ��������Ϣ
	startup.cb		= sizeof(startup);

	// ��������
	if(CreateProcessA (
		/* lpApplicationName */		NULL,
		/* lpCommandLine */			(LPSTR) load::command,
		/* lpProcessAttributes */	NULL,
		/* lpThreadAttributes */	NULL,
		/* bInheritHandles */		FALSE,
		/* dwCreationFlags */		CREATE_SUSPENDED,
		/* lpEnvironment */			NULL,
		/* lpCurrentDirectory */	NULL,
		/* lpStartupInfo */			& startup,
		/* lpProcessInformation */	& process
	) == FALSE) {
		gui::response_error (	"�޷�����ָ����Ӧ�ó���\n\n"
								"���������·���Ƿ���ȷ������пո��·��������ָ���Լ�����Ȩ���Ƿ��㹻������Ҫ����ԱȨ�ޣ��ȡ�"	);

		goto end;
	}

	// ����׷�ٺ���ע������������ (in tracker)
	int result = ((int (__cdecl *) (HANDLE, const char *, const char *)) function) (process.hThread, load::library, load::controller);

	// ����Ƿ�ע��ɹ�
	if(result == 0) {
		// ע��ʧ����ֹ����
		TerminateProcess	(process.hProcess, 0);

		// �ȴ������˳�
		WaitForSingleObject	(process.hProcess, INFINITE);

		// ʧ����Ӧ, ������Ϣ�� tracker_install ���
		gui::response_error	( );
	} else {
		// ע��ɹ��ָ����߳�
		ResumeThread	(process.hThread);

		// �ɹ���Ӧ
		gui::response_succeed	( );
	}

	// �رվ��
	CloseHandle (process.hProcess);
	CloseHandle (process.hThread);


end:
	// ж�ض�̬���ӿ�
	FreeLibrary (module);

	return 0;
}





/* ��ʼ����Ϣ */
void load::start (const char *argument) {
	// ���� DLL ·��
	load::build_library		( );
	// ����Ĭ��������Ϣ
	load::build_argument	(argument);
}



/* �������� */
void load::run (void) {
	// �����߳�
	if(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) load::thread, NULL, 0, NULL) == NULL) {
		gui::response_error ("�޷������µ��̡߳�");
	}
}

