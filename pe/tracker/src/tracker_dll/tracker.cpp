/*
	$ Tracker w64   (C) 2005-2014 MF
	$ tracker.dll/tracker.cpp   # 1312
*/


#include "base.h"



/* ע���߳� */
static __declspec(naked) void __cdecl tracker_remote_call (void) {
	__asm {
			call	entry

			// 'tracker_install', \0
			__emit	0x74
			__emit	0x72
			__emit	0x61
			__emit	0x63
			__emit	0x6B
			__emit	0x65
			__emit	0x72
			__emit	0x5F
			__emit	0x69
			__emit	0x6E
			__emit	0x73
			__emit	0x74
			__emit	0x61
			__emit	0x6C
			__emit	0x6C
			__emit	0x00


		entry:
			push	ebp
			mov		ebp,	esp

			mov		esi,	[ebp + 0x0C]				; ESI	= {ARGUMENT}

			push	esi									; [ESP]	= {ARGUMENT}
			push	dword ptr 0							; [ESP]	= HMODULE


			mov		eax,	fs:[0x30]					; EAX	= linear address of	PEB
			mov		eax,	[eax + OFFSET_PEB]			; EAX	= tracker loader address in PEB

			test	eax,	eax
			jz		start


			mov		[ebp + 0x04],	eax

			jmp		done


		start:
			push	dword ptr [esi + 0x04]				; LoadLibraryA.lpFileName	= {ARGUMENT.library}
			call	dword ptr [esi + 0x10]				; LoadLibraryA

			test	eax,	eax
			jz		error


			mov		[esp],	eax							; [ESP]	= HMODULE	= return of LoadLibraryA
			mov		esi,	[esp + 0x04]				; ESI	= {ARGUMENT}

			push	dword ptr [ebp + 0x04]				; GetProcAddress.lpProcName	= "tracker_install"
			push	eax									; GetProcAddress.hModule	= HMODULE
			call	dword ptr [esi + 0x18]				; GetProcAddress

			test	eax,	eax
			jz		error


			mov		esi,	[esp + 0x04]				; ESI	= {ARGUMENT}

			push	dword ptr [esi]						; tracker_install.base		= {ARGUMENT.base}
			call	eax									; tracker_install
			add		esp,	0x04
			
			test	eax,	eax
			jz		error


			mov		dword ptr [ebp + 0x04],	eax			; set return code	= return value of tracker_install


			mov		esi,	fs:[0x30]					; ESI	= linear address of	PEB

			mov		[esi + OFFSET_PEB],	eax				; save tracker loader to PEB


			jmp		done


		error:
			mov		dword ptr [ebp + 0x04],	0x00		; set return code	= 0


		done:
			mov		esi,	[esp + 0x04]				; ESI	= {ARGUMENT}

			call	dword ptr [esi + 0x14]				; FreeLibrary


			add		esp,	0x04

			pop		ebp
			pop		eax									; EAX	= return code


			retn	4
	}
}





/* ע�뺯��, �����ڸ����� */
extern "C" __declspec(dllexport) int __cdecl tracker_remote (HANDLE thread, const char *library, const char *controller) {
	// �ж϶�̬���ӿ�·������
	if(strlen (library) > SIZE_LIBRARY) {
		MessageBoxA (NULL, "��̬���ӿ�·�����ȳ���������ơ�", "", MB_ICONERROR | MB_OK);

		return 0;
	}

	// �жϿ�����·������
	if(strlen (controller) > SIZE_CONTROLLER) {
		MessageBoxA (NULL, "׷�ٿ�����ģ��·�����ȳ���������ơ�", "", MB_ICONERROR | MB_OK);

		return 0;
	}


	// ��ȡ�߳� ID
	DWORD	threadid	= GetThreadId (thread);
	// ��ȡӵ������Ȩ�޵��߳̾��
	thread	= OpenThread (THREAD_QUERY_INFORMATION | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, TRUE, threadid);

	// �ж��Ƿ�ɹ���ȡ���̾��
	if(thread == NULL) {
		MessageBoxA (NULL, "�޷����ӵ��������Ȩ�޵��߳̾����", "", MB_ICONERROR | MB_OK);

		return 0;
	}


	// ��ȡ���� ID
	DWORD	processid	= GetProcessIdOfThread	(thread);
	// ��ȡ���̾��
	HANDLE	process		= OpenProcess	(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, processid);

	// �ж��Ƿ�ɹ���ȡ���̾��
	if(process == NULL) {
		MessageBoxA (NULL, "�޷�ͨ���߳̾����ȡ�����Ľ��̾����", "", MB_ICONERROR | MB_OK);

		return 0;
	}


	// �����ڴ��
	uintptr_t	base	= remote::allocate (process, ALLOCATE);

	// �ж��Ƿ����ɹ�
	if(base == 0) {
		MessageBoxA (NULL, "��Ŀ����̿ռ��ڷ����ڴ�ʧ�ܡ�", "", MB_ICONERROR | MB_OK);

		return 0;
	}


	// ���ַ
	uintptr_t	base_thread_code		= base;
	uintptr_t	base_thread_argument	= base_thread_code		+ SIZE_THREAD_CODE;
	uintptr_t	base_library			= base_thread_argument	+ SIZE_THREAD_ARGUMENT;
	uintptr_t	base_controller			= base_library			+ SIZE_LIBRARY;


	// �߳���������
	uintptr_t	argument [ ]	= {
		/* 0x00 */	base,									// �ڴ���ַ
		/* 0x04 */	base_library,							// ��̬���ӿ�·����ַ
		/* 0x08 */	0,
		/* 0x0C */	0,
		/* 0x10 */	(uintptr_t) LoadLibraryA,				// @ LoadLibraryA
		/* 0x14 */	(uintptr_t) FreeLibrary,				// @ FreeLibrary
		/* 0x18 */	(uintptr_t) GetProcAddress,				// @ GetProcAddress
		/* 0x1C */	0,
	};


	// ��ʵ�ʴ�С
	uint32_t	size_thread_code		= 0x86;
	uint32_t	size_thread_argument	= sizeof argument;
	uint32_t	size_library			= strlen (library)		+ 1;
	uint32_t	size_controller			= strlen (controller)	+ 1;


	// д�����
	if(remote::write (process, base_thread_code, tracker_remote_call, size_thread_code) == false) {
		MessageBoxA (NULL, "��Ŀ������ڴ�д��ע���̴߳���ʧ�ܡ�", "", MB_ICONERROR | MB_OK);

		return 0;
	}

	// д�����
	if(remote::write (process, base_thread_argument, argument, size_thread_argument) == false) {
		MessageBoxA (NULL, "��Ŀ������ڴ�д��ע���߳���������ʧ�ܡ�", "", MB_ICONERROR | MB_OK);

		return 0;
	}

	// д�붯̬���ӿ�·��
	if(remote::write (process, base_library, (void *) library, size_library) == false) {
		MessageBoxA (NULL, "��Ŀ������ڴ�д�붯̬���ӿ�·��ʧ�ܡ�", "", MB_ICONERROR | MB_OK);

		return 0;
	}

	// д��׷�ٿ�����·��
	if(remote::write (process, base_controller, (void *) controller, size_controller) == false) {
		MessageBoxA (NULL, "��Ŀ������ڴ�д��׷�ٿ�����·��ʧ�ܡ�", "", MB_ICONERROR | MB_OK);

		return 0;
	}


	// ����Զ���߳�
	HANDLE	handle	= remote::thread (process, base_thread_code, (uint32_t) base_thread_argument);

	// �ж��Ƿ������ɹ�
	if(handle == NULL) {
		MessageBoxA (NULL, "�޷���Ŀ�����������Զ���̡߳�", "", MB_ICONERROR | MB_OK);

		return 0;
	}

	// �ȴ��߳�ִ�����
	WaitForSingleObject (handle, INFINITE);


	// ��ȡ�����˳�������Ϊ�߳���������ַ
	uintptr_t	base_loader	= 0;

	// ��ȡ�����˳�����
	if(GetExitCodeThread (handle, (LPDWORD) & base_loader) == FALSE) {
		MessageBoxA (NULL, "�޷���ȡԶ���߳��˳����롣", "", MB_ICONERROR | MB_OK);

		return 0;
	}

	// �ж���������ַ�Ƿ���Ч
	if((base_loader <= 0xFFFF) || (base_loader >= 0x7FFF0000)) {
		// ��ʱ�� tracker_install ����������
		return 0;
	}

	// �ж���������ַ�Ƿ񲻴����ڵ�ǰ�ڴ��
	if((base_loader < base) || (base_loader >= (base + ALLOCATE))) {
		// �ͷŵ�ǰ�ڴ��
		VirtualFree ((LPVOID) base, 0, MEM_RELEASE);
	}


	// �߳������Ļ���
	CONTEXT	context;

	// ���û�ȡ���мĴ�����Ϣ
	context.ContextFlags	= CONTEXT_ALL;

	// ��ȡ�̼߳Ĵ�����Ϣ
	if(GetThreadContext (thread, & context) == FALSE) {
		MessageBoxA (NULL, "�޷���ȡ�̵߳ļĴ�����Ϣ��", "", MB_ICONERROR | MB_OK);

		return 0;
	}


	// TEB ���Ե�ַ
	LDT_ENTRY	teb_base;

	// ��ȡ TEB ���Ե�ַ
	if(GetThreadSelectorEntry (thread, context.SegFs, & teb_base) == FALSE) {
		MessageBoxA (NULL, "�޷���ȡ�߳���Ϣ�����Ե�ַ��", "", MB_ICONERROR | MB_OK);

		return 0;
	}


	// ���� TEB ��ַ
	uintptr_t	teb		= (teb_base.HighWord.Bytes.BaseHi << 24) | (teb_base.HighWord.Bytes.BaseMid << 16) | teb_base.BaseLow;
	// �̵߳�ǰִ�е�ַ
	uintptr_t	pointer	= (uintptr_t) context.Eip;

	// ����ǰִ�е�ַ���浽 TEB ��
	if(remote::write (process, teb + OFFSET_TEB, & pointer, 0x04) == false) {
		MessageBoxA (NULL, "�޷����߳�ִ����ڱ��浽�߳���Ϣ���С�", "", MB_ICONERROR | MB_OK);

		return 0;
	}


	// �����µ�ִ�е�ַΪ�߳�������
	context.Eip	= (DWORD) base_loader;

	// ����Ĵ�����Ϣ
	if(SetThreadContext (thread, & context) == FALSE) {
		MessageBoxA (NULL, "�޷������̵߳ļĴ�����Ϣ��", "", MB_ICONERROR | MB_OK);

		return 0;
	}


	// �ر�Զ���߳̾��
	CloseHandle (handle);
	// �رս��̾��
	CloseHandle (process);
	// �ر��߳̾��
	CloseHandle (thread);

	return 1;
}




/* ������������, �������ӽ��� */
extern "C" __declspec(dllexport) int __cdecl tracker_install (uintptr_t base) {
	// �����ַ
	install::update (base);

	// ���ؿ�����ģ��
	if(install::controller ( ) == false) {
		return 0;
	}

	// �������
	install::build_address		( );
	install::build_information	( );

	// �����ص���
	if(install::install_table ( ) == false) {
		MessageBoxA (NULL, "�޷���ʼ�������ص�������׷�ٿ�������̬���ӿ��ļ��Ƿ��������ı�д�淶����ص������Ƿ���ڲ�����ȷ������", "", MB_ICONERROR | MB_OK);

		return 0;
	}


	// ��װ������
	install::install_handler	( );

	// ӳ�亯��
	install::map_function	( );


	// ���浽 PEB
	__asm {
		mov		eax,	install::base_function_loader		; address of		loader
		mov		ebx,	fs:[0x30]							; linear address of	PEB

		mov		[ebx + OFFSET_PEB],	eax
	}


	// �����߳���������ַ
	return (int) install::base_function_loader;
}

