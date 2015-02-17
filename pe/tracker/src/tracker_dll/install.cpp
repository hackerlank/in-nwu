/*
	$ Tracker w64   (C) 2005-2014 MF
	$ tracker.dll/install.cpp   # 1312
*/


#include "base.h"



/* DLL ģ���� */
HMODULE		install::module_ntdll		= NULL;
HMODULE		install::module_controller	= NULL;


/* ��ַ */
uintptr_t	install::base		= 0;

/* ����/��/��������ַ */
uintptr_t	install::base_library		= 0;
uintptr_t	install::base_controller	= 0;
uintptr_t	install::base_address		= 0;
uintptr_t	install::base_information	= 0;
uintptr_t	install::base_table			= 0;
uintptr_t	install::base_handler		= 0;

/* ������ַ */
uintptr_t	install::base_function_jumper			= 0;
uintptr_t	install::base_function_injector			= 0;
uintptr_t	install::base_function_injector_local	= 0;
uintptr_t	install::base_function_injector_remote	= 0;
uintptr_t	install::base_function_loader			= 0;





/* ���û�ַ */
void install::update (uintptr_t base) {
	// �����ַ
	install::base	= base;

	// ������ַ
	install::base_library		= install::base				+ SIZE_THREAD_CODE + SIZE_THREAD_ARGUMENT;		// ��̬���ӿ�·��
	install::base_controller	= install::base_library		+ SIZE_LIBRARY;									// ������·��
	install::base_address		= install::base_controller	+ SIZE_CONTROLLER;								// Windows API ��ַ��
	install::base_information	= install::base_address		+ SIZE_ADDRESS;									// ��Ϣ��
	install::base_table			= install::base_information	+ SIZE_INFORMATION;								// �ص���
	install::base_handler		= install::base_table		+ SIZE_TABLE;									// ������

	// ���㺯����ַ
	install::base_function_jumper			= install::base_handler						+ SIZE_HANDLER;						// WoW64 ϵͳ����ת����
	install::base_function_injector			= install::base_function_jumper				+ SIZE_FUNCTION_JUMPER;				// ע����
	install::base_function_injector_local	= install::base_function_injector			+ SIZE_FUNCTION_INJECTOR;			// ����ע����
	install::base_function_injector_remote	= install::base_function_injector_local		+ SIZE_FUNCTION_INJECTOR_LOCAL;		// ����ע����
	install::base_function_loader			= install::base_function_injector_remote	+ SIZE_FUNCTION_INJECTOR_REMOTE;	// �߳�������
}


// ����ģ��
bool install::controller (void) {
	// ���������
	install::module_controller	= LoadLibraryA ((LPCSTR) install::base_controller);

	// �ж��Ƿ�ɹ����������
	if(install::module_controller == NULL) {
		MessageBoxA (NULL, "�޷�����׷�ٿ�������̬���ӿ�ģ�顣", "", MB_ICONERROR | MB_OK);

		return false;
	}

	return true;
}





/* �����ַ�� */
void install::build_address (void) {
	// 00 - 0F
	*((uint32_t *) (install::base_address + 0x00))	= (uint32_t) LoadLibraryA;
	*((uint32_t *) (install::base_address + 0x04))	= (uint32_t) FreeLibrary;
	*((uint32_t *) (install::base_address + 0x08))	= (uint32_t) GetProcAddress;

	// 10 - 1F
	*((uint32_t *) (install::base_address + 0x10))	= (uint32_t) SuspendThread;
	*((uint32_t *) (install::base_address + 0x14))	= (uint32_t) ResumeThread;
	*((uint32_t *) (install::base_address + 0x18))	= (uint32_t) GetThreadContext;
	*((uint32_t *) (install::base_address + 0x1C))	= (uint32_t) SetThreadContext;

	// 20 - 2F
	*((uint32_t *) (install::base_address + 0x20))	= (uint32_t) TerminateThread;
	*((uint32_t *) (install::base_address + 0x24))	= (uint32_t) GetThreadId;
	*((uint32_t *) (install::base_address + 0x28))	= (uint32_t) GetProcessIdOfThread;
	*((uint32_t *) (install::base_address + 0x2C))	= (uint32_t) GetThreadSelectorEntry;

	// 30 - 3F
	*((uint32_t *) (install::base_address + 0x30))	= (uint32_t) TerminateProcess;
}


/* ������Ϣ�� */
void install::build_information (void) {
	// 00 - 0F
	*((uint32_t *) (install::base_information + 0x00))	= (uint32_t) install::base_library;
	*((uint32_t *) (install::base_information + 0x04))	= (uint32_t) install::base_controller;

	// 10 - 1F
	*((uint32_t *) (install::base_information + 0x10))	= (uint32_t) install::base_function_jumper;

	// 20 - 2F
	*((uint32_t *) (install::base_information + 0x20))	= (uint32_t) install::base_function_injector;
	*((uint32_t *) (install::base_information + 0x24))	= (uint32_t) install::base_function_injector_local;
	*((uint32_t *) (install::base_information + 0x28))	= (uint32_t) install::base_function_injector_remote;

	// 30 - 3F
	*((uint32_t *) (install::base_information + 0x30))	= (uint32_t) install::base_function_loader;
}




/* ��װ�ص��� */
bool install::install_table (void) {
	// ��ȡ ntdll.dll ���
	install::module_ntdll	= LoadLibraryA ("ntdll.dll");

	// ����Ƿ�ɹ���� ntdll.dll ģ����
	if(module_ntdll == NULL) {
		return false;
	}


	// ϵͳ������
	const char *	function_system		[ ] = {	"ZwResumeThread",	"ZwCreateThreadEx",		"ZwCreateUserProcess",	"ZwOpenFile",	"ZwCreateFile"	};
	// �ص�������
	const char *	function_callback	[ ] = {	NULL,				NULL,					"tCreateProcess",		"tCreateFile",	"tCreateFile"	};


	// �������غ�����ַ
	uintptr_t	load	= (uintptr_t) GetProcAddress (install::module_controller, "tLoad");

	// ��ǰ��ָ��
	uintptr_t	pointer	= install::base_table;


	// ���ΰ�װ����
	for(int i = 0; i < (sizeof function_system / sizeof(const char *)); i ++) {
		if(install::install_table_function (pointer, load, function_system [i], function_callback [i]) == false) {
			return false;
		}

		// �ƶ�ָ��ָ����һ��
		pointer	= pointer + 0x10;
	}


	// д��հ�
	memset ((void *) pointer, 0x00, 0x10);


	// ���ʹ����������, ж��ģ��
	if(load != 0) {
		FreeLibrary (install::module_controller);
	}


	return true;
}


/* ��װ�ص����� */
bool install::install_table_function (uintptr_t address, uintptr_t load, const char *system, const char *callback) {
	// ϵͳ������ַ
	uintptr_t	address_system		= (uintptr_t) GetProcAddress (install::module_ntdll, system);
	// �ص�������ַ
	uintptr_t	address_callback	= 0;

	// �ж��Ƿ�ɹ��� ntdll.dll �������ȡ������ַ
	if(address_system == 0) {
		return false;
	}

	// �ж��Ƿ���Ҫ�ص�����
	if(callback != NULL) {
		// ��ȡ�ص�������ַ
		if(load == 0) {
			// ʹ�õ�����
			address_callback	= (uintptr_t) GetProcAddress (install::module_controller, callback);
		} else {
			// ʹ����������
			address_callback	= ((uintptr_t (__cdecl *)(const char *)) load) (callback);
		}

		// �ж��Ƿ�ɹ���ȡ�ص�������ַ
		if(address_callback == 0) {
			return false;
		}
	}

	// ��ȡϵͳ��������Ȩ��
	uint32_t	flag	= PAGE_EXECUTE_READWRITE;

	// �����ڴ�Ȩ��
	if(VirtualProtect ((LPVOID) address_system, 0x0F, flag, (PDWORD) & flag) == FALSE) {
		return false;
	}

	// ��⺯���Ƿ���ȷ
	if(*((uint8_t  *) (address_system + 0x00)) != 0xB8) {
		return false;
	}
	if(*((uint32_t *) (address_system + 0x05)) != 0xC015FF64) {
		return false;
	}
	if(*((uint32_t *) (address_system + 0x09)) != 0xC2000000) {
		return false;
	}

	// д��ص���
	*((uint32_t *) (address + 0x00))	= (uint32_t) address_system + 0x0C;			// �����ķ��ص�ַ
	*((uint32_t *) (address + 0x04))	= 0;										// ��������ַ
	*((uint32_t *) (address + 0x08))	= (uint32_t) address_callback;				// �ص�������ַ

	// �ָ��ڴ�Ȩ��
	VirtualProtect ((LPVOID) address_system, 0x0F, flag, (PDWORD) & flag);

	return true;
}




/* ӳ�亯�� */
void install::map_function (void) {
	install::map_function_jumper			(install::base_function_jumper,				install::base_information,		install::base_table			);

	install::map_function_injector			(install::base_function_injector,			install::base_address,			install::base_information	);
	install::map_function_injector_local	(install::base_function_injector_local,		install::base_address,			install::base_information	);
	install::map_function_injector_remote	(install::base_function_injector_remote,	install::base_address,			install::base_information	);
	install::map_function_loader			(install::base_function_loader,				install::base_function_jumper								);
}

